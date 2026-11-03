#include "animalworld/World.h"

#include "animalworld/Exceptions.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>

namespace animalworld {

namespace {

int signOf(int value) {
    if (value < 0) {
        return -1;
    }
    if (value > 0) {
        return 1;
    }
    return 0;
}

Position stepTowards(const Position& from, const Position& to, int maxSteps) {
    Position result = from;
    const int distance = from.chebyshevDistanceTo(to);
    const int steps = std::min(std::max(0, maxSteps), distance);
    for (int i = 0; i < steps; ++i) {
        const int rowStep = signOf(to.row() - result.row());
        const int colStep = signOf(to.col() - result.col());
        if (rowStep != 0) {
            result.setRow(result.row() + rowStep);
        }
        if (colStep != 0) {
            result.setCol(result.col() + colStep);
        }
    }
    return result;
}

Position randomTarget(RandomSource& random, const Position& origin,
                      int speed, const FieldSize& size) {
    for (int attempt = 0; attempt < 16; ++attempt) {
        const int dr = random.nextInt(-speed, speed);
        const int dc = random.nextInt(-speed, speed);
        if (dr == 0 && dc == 0) {
            continue;
        }
        const int row = std::max(0, std::min(size.rows() - 1, origin.row() + dr));
        const int col = std::max(0, std::min(size.cols() - 1, origin.col() + dc));
        Position candidate(row, col);
        if (candidate != origin) {
            return candidate;
        }
    }
    return origin;
}

std::vector<std::shared_ptr<Animal>> collectAnimals(
    const GameField& field) {
    std::vector<std::shared_ptr<Animal>> animals;
    for (const auto& inhabitant : field.allInhabitants()) {
        if (!inhabitant->isAnimal()) {
            continue;
        }
        if (const auto animal = std::dynamic_pointer_cast<Animal>(inhabitant)) {
            animals.push_back(animal);
        }
    }
    return animals;
}

std::vector<std::shared_ptr<Plant>> collectPlants(const GameField& field) {
    std::vector<std::shared_ptr<Plant>> plants;
    for (const auto& inhabitant : field.allInhabitants()) {
        if (const auto plant = std::dynamic_pointer_cast<Plant>(inhabitant)) {
            plants.push_back(plant);
        }
    }
    return plants;
}

std::shared_ptr<Plant> nearestPlant(const GameField& field,
                                    const Position& origin, int radius) {
    std::shared_ptr<Plant> best;
    int bestDistance = radius + 1;
    for (int dr = -radius; dr <= radius; ++dr) {
        for (int dc = -radius; dc <= radius; ++dc) {
            const int row = origin.row() + dr;
            const int col = origin.col() + dc;
            if (row < 0 || col < 0 || row >= field.size().rows() ||
                col >= field.size().cols()) {
                continue;
            }
            const Position position(row, col);
            if (!field.isInside(position)) {
                continue;
            }
            const auto plant = field.cellAt(position).plant();
            if (!plant || !plant->alive()) {
                continue;
            }
            const int distance = origin.distanceTo(position);
            if (distance < bestDistance) {
                bestDistance = distance;
                best = plant;
            }
        }
    }
    return best;
}

std::shared_ptr<Herbivore> nearestPrey(const GameField& field,
                                       const Position& origin, int radius,
                                       const Predator& predator) {
    std::shared_ptr<Herbivore> best;
    int bestDistance = radius + 1;
    for (int dr = -radius; dr <= radius; ++dr) {
        for (int dc = -radius; dc <= radius; ++dc) {
            const int row = origin.row() + dr;
            const int col = origin.col() + dc;
            if (row < 0 || col < 0 || row >= field.size().rows() ||
                col >= field.size().cols()) {
                continue;
            }
            const Position position(row, col);
            if (!field.isInside(position)) {
                continue;
            }
            for (const auto& occupant : field.cellAt(position).inhabitants()) {
                const auto prey = std::dynamic_pointer_cast<Herbivore>(occupant);
                if (!prey || !prey->alive() || !predator.canHunt(*prey)) {
                    continue;
                }
                const int distance = origin.distanceTo(position);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    best = prey;
                }
            }
        }
    }
    return best;
}

} // namespace

Simulation::Simulation()
    : field_(FieldSize(8, 8)),
      config_(),
      stats_(),
      log_(),
      random_(12345UL),
      environment_(),
      stepNumber_(0) {}

Simulation::Simulation(GameField field, SimulationConfig config,
                       unsigned long seed)
    : field_(std::move(field)),
      config_(config),
      stats_(),
      log_(),
      random_(seed),
      environment_(),
      stepNumber_(0) {
    config_.validate();
}

void Simulation::step() {
    ++stepNumber_;
    movementPhase();
    feedingPhase();
    reproductionPhase();
    deathPhase();
    stats_.recordStep();
    stats_.updatePeak(field_.animals());
    log_.add(WorldEvent(EventType::System,
                        "step " + std::to_string(stepNumber_) + " completed",
                        stepNumber_, Position(0, 0)));
}

void Simulation::movementPhase() {
    auto animals = collectAnimals(field_);
    random_.shuffle(animals);

    for (const auto& animal : animals) {
        if (!animal->alive()) {
            continue;
        }
        const Position current = animal->position();
        Position target = current;

        if (const auto herbivore =
                std::dynamic_pointer_cast<Herbivore>(animal)) {
            (void)herbivore;
            const auto plant = nearestPlant(field_, current, 4);
            if (plant) {
                target = plant->position();
            }
        } else if (const auto predator =
                       std::dynamic_pointer_cast<Predator>(animal)) {
            const auto prey = nearestPrey(field_, current, 5, *predator);
            if (prey) {
                target = prey->position();
            }
        }

        if (target == current) {
            target = randomTarget(random_, current, animal->speed(),
                                  field_.size());
        }
        Position candidate =
            stepTowards(current, target, animal->speed());
        candidate = field_.size().clamp(candidate);

        bool moved = false;
        if (candidate != current && field_.canPlace(candidate, animal)) {
            field_.remove(animal);
            animal->setPosition(candidate);
            field_.add(animal);
            moved = true;
        }
        if (!moved) {
            for (int attempt = 0; attempt < 4 && !moved; ++attempt) {
                const Position alternative =
                    randomTarget(random_, current, animal->speed(),
                                 field_.size());
                if (field_.canPlace(alternative, animal)) {
                    field_.remove(animal);
                    animal->setPosition(alternative);
                    field_.add(animal);
                    moved = true;
                }
            }
        }
    }
}

void Simulation::feedingPhase() {
    auto animals = collectAnimals(field_);
    random_.shuffle(animals);

    for (const auto& animal : animals) {
        if (!animal->alive()) {
            continue;
        }
        const Position position = animal->position();
        FieldCell& cell = field_.cellAt(position);
        bool fed = false;

        if (const auto herbivore =
                std::dynamic_pointer_cast<Herbivore>(animal)) {
            const auto plant = cell.plant();
            if (plant && plant->alive() && herbivore->canGraze(*plant)) {
                const int eaten = herbivore->graze(*plant);
                stats_.recordMeal();
                fed = true;
                log_.add(WorldEvent(
                    EventType::Feeding,
                    herbivore->species() + " ate " + std::to_string(eaten) +
                        " plant points",
                    stepNumber_, position));
            }
        } else if (const auto predator =
                       std::dynamic_pointer_cast<Predator>(animal)) {
            const auto occupants = cell.inhabitants();
            for (const auto& occupant : occupants) {
                const auto prey =
                    std::dynamic_pointer_cast<Herbivore>(occupant);
                if (!prey || !prey->alive() || !predator->canHunt(*prey)) {
                    continue;
                }
                const double escape =
                    static_cast<double>(prey->fleeChance()) / 100.0;
                if (random_.chance(escape)) {
                    log_.add(WorldEvent(
                        EventType::Feeding,
                        prey->species() + " escaped from " +
                            predator->species(),
                        stepNumber_, position));
                    break;
                }
                const int nutrition = predator->hunt(*prey);
                stats_.recordMeal();
                fed = true;
                log_.add(WorldEvent(
                    EventType::Feeding,
                    predator->species() + " killed " + prey->species() +
                        " for " + std::to_string(nutrition) + " nutrition",
                    stepNumber_, position));
                break;
            }
        }

        if (!fed) {
            animal->growHunger();
        }
    }
}

void Simulation::reproductionPhase() {
    auto animals = collectAnimals(field_);
    std::vector<bool> used(animals.size(), false);

    for (std::size_t i = 0; i < animals.size(); ++i) {
        if (used[i] || !animals[i]->alive() || !animals[i]->canBreed()) {
            continue;
        }
        for (std::size_t j = i + 1; j < animals.size(); ++j) {
            if (used[j] || !animals[j]->alive()) {
                continue;
            }
            if (!animals[i]->canBreedWith(*animals[j])) {
                continue;
            }
            const Position position = animals[i]->position();
            FieldCell& cell = field_.cellAt(position);
            if (cell.freeCapacity() <= 0) {
                continue;
            }
            auto child = animals[i]->makeOffspring(position);
            child->setSex(random_.chance(0.5) ? Sex::Male : Sex::Female);
            if (!field_.canPlace(position, child)) {
                continue;
            }
            field_.add(child);
            animals[i]->startBreedingCooldown();
            animals[j]->startBreedingCooldown();
            used[i] = true;
            used[j] = true;
            stats_.recordBirth();
            log_.add(WorldEvent(
                EventType::Birth,
                animals[i]->species() + " offspring born",
                stepNumber_, position));
            break;
        }
    }

    auto plants = collectPlants(field_);
    random_.shuffle(plants);
    const double growthChance = std::min(
        1.0, config_.plantGrowthChance() * environment_.plantGrowthFactor());

    for (const auto& plant : plants) {
        if (!plant->alive() || plant->health() < 50 ||
            plant->seedCount() <= 0) {
            continue;
        }
        if (!random_.chance(growthChance)) {
            continue;
        }
        const Position origin = plant->position();
        auto targets = origin.neighbors(field_.size().rows(),
                                        field_.size().cols());
        targets.push_back(origin);
        random_.shuffle(targets);

        bool spread = false;
        for (const auto& target : targets) {
            if (!plant->canSpreadTo(target, field_.size())) {
                continue;
            }
            FieldCell& cell = field_.cellAt(target);
            if (cell.hasPlant()) {
                const auto other = cell.plant();
                if (other.get() != plant.get() &&
                    other->healthPercent() < 100) {
                    cell.refreshPlant();
                    spread = true;
                    log_.add(WorldEvent(
                        EventType::Reproduction,
                        "plant refreshed a neighbouring plant",
                        stepNumber_, target));
                    break;
                }
                continue;
            }
            if (cell.freeCapacity() <= 0) {
                continue;
            }
            auto child =
                std::make_shared<Plant>(target, random_.nextInt(40, 80));
            if (field_.canPlace(target, child)) {
                field_.add(child);
                spread = true;
                log_.add(WorldEvent(EventType::Reproduction,
                                    "new plant sprouted", stepNumber_,
                                    target));
                break;
            }
        }
        if (spread) {
            plant->setSeedCount(std::max(0, plant->seedCount() - 1));
        }
    }
}

void Simulation::deathPhase() {
    auto inhabitants = field_.allInhabitants();

    for (const auto& inhabitant : inhabitants) {
        if (!inhabitant->alive()) {
            stats_.recordDeath(stepNumber_);
            log_.add(WorldEvent(EventType::Death,
                                inhabitant->species() + " removed",
                                stepNumber_, inhabitant->position()));
            continue;
        }

        inhabitant->ageOneTurn();
        bool died = false;
        std::string reason;

        if (const auto animal =
                std::dynamic_pointer_cast<Animal>(inhabitant)) {
            if (animal->age() > animal->maxAge()) {
                died = true;
                reason = "old age";
            } else if (animal->isStarving()) {
                died = true;
                reason = "starvation";
            }
        } else if (const auto plant =
                       std::dynamic_pointer_cast<Plant>(inhabitant)) {
            if (plant->health() <= 0) {
                died = true;
                reason = "withering";
            }
        }

        if (died) {
            inhabitant->kill();
            stats_.recordDeath(stepNumber_);
            log_.add(WorldEvent(EventType::Death,
                                inhabitant->species() + " died of " + reason,
                                stepNumber_, inhabitant->position()));
        }
    }

    field_.clearDead();
}

void Simulation::run(int steps, bool interactive) {
    if (steps < 0) {
        throw SimulationStateException("Step count must not be negative");
    }
    ConsoleRenderer renderer;
    for (int i = 0; i < steps; ++i) {
        step();
        std::cout << renderer.render(field_);
        std::cout << "Step " << stepNumber_ << ": " << stats_.summary()
                  << '\n';
        if (interactive) {
            std::cout << "Press Enter to continue..." << std::flush;
            std::string line;
            std::getline(std::cin, line);
        }
    }
}

std::string Simulation::snapshot() const {
    ConsoleRenderer renderer;
    std::ostringstream out;
    out << renderer.render(field_);
    out << "Step " << stepNumber_ << ": " << stats_.summary() << '\n';
    return out.str();
}

std::string Simulation::summary() const {
    std::ostringstream out;
    out << config_.describe() << '\n'
        << stats_.summary() << '\n'
        << "Environment: " << environment_.describe() << '\n'
        << "Logged events: " << log_.size() << '\n';
    return out.str();
}

void Simulation::reset(unsigned long seed) {
    field_ = GameField(field_.size());
    stats_.reset();
    log_.clear();
    random_.reseed(seed);
    stepNumber_ = 0;
}

void Simulation::addAnimal(const std::shared_ptr<Animal>& animal) {
    if (!animal) {
        throw InvalidConfigurationException("Cannot add a null animal");
    }
    field_.add(animal);
}

int Simulation::stepNumber() const { return stepNumber_; }

const GameField& Simulation::field() const { return field_; }
GameField& Simulation::field() { return field_; }
const SimulationConfig& Simulation::config() const { return config_; }
SimulationConfig& Simulation::config() { return config_; }
const SimulationStatistics& Simulation::statistics() const { return stats_; }
const EventLog& Simulation::eventLog() const { return log_; }
const Environment& Simulation::environment() const { return environment_; }

} // namespace animalworld
