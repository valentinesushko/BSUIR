#include "animalworld/Inhabitants.h"

#include "animalworld/Exceptions.h"

#include <algorithm>
#include <sstream>

namespace animalworld {

long Inhabitant::nextId_ = 1000;

Inhabitant::Inhabitant(std::string species, const Position& position, int size,
                       int maxAge, char symbol)
    : id_("AW-" + std::to_string(++nextId_)),
      speciesName_(std::move(species)),
      position_(position),
      size_(size),
      age_(0),
      maxAge_(maxAge),
      alive_(true),
      symbol_(symbol) {
    if (speciesName_.empty()) {
        throw InvalidConfigurationException("Species name must not be empty");
    }
    if (size_ <= 0 || maxAge_ <= 0) {
        throw InvalidConfigurationException(
            "Inhabitant size and lifespan must be positive");
    }
}

const std::string& Inhabitant::id() const { return id_; }
const std::string& Inhabitant::species() const { return speciesName_; }
const Position& Inhabitant::position() const { return position_; }

void Inhabitant::setPosition(const Position& position) {
    position_ = position;
}

int Inhabitant::size() const { return size_; }
int Inhabitant::age() const { return age_; }
int Inhabitant::maxAge() const { return maxAge_; }
bool Inhabitant::alive() const { return alive_; }

void Inhabitant::kill() { alive_ = false; }

void Inhabitant::ageOneTurn() { ++age_; }

bool Inhabitant::isMature() const { return age_ >= maturityAge(); }

char Inhabitant::symbol() const { return symbol_; }

bool Inhabitant::canShareCell() const { return true; }
bool Inhabitant::isAnimal() const { return false; }

std::string Inhabitant::statusLine() const { return describe(); }

int Inhabitant::distanceTo(const Inhabitant& other) const {
    return position_.distanceTo(other.position_);
}

Plant::Plant()
    : Plant(Position(0, 0), 60) {}

Plant::Plant(const Position& position)
    : Plant(position, 100) {}

Plant::Plant(const Position& position, int health)
    : Inhabitant("Plant", position, 1, 40, '*'),
      health_(health),
      maxHealth_(100),
      decayRate_(4),
      seedCount_(3),
      spreadRadius_(1) {
    if (health_ < 0 || health_ > maxHealth_) {
        throw InvalidVitalException("Plant health must be in [0, 100]");
    }
}

void Plant::ageOneTurn() {
    Inhabitant::ageOneTurn();
    takeDamage(decayRate_);
}

int Plant::maturityAge() const { return 1; }

void Plant::wither(int amount) { takeDamage(amount); }

void Plant::regrow() {
    health_ = maxHealth_;
    seedCount_ = std::max(seedCount_, 1);
}

bool Plant::canSpreadTo(const Position& target, const FieldSize& size) const {
    if (!size.isInside(target)) {
        return false;
    }
    return position_.chebyshevDistanceTo(target) <= spreadRadius_;
}

int Plant::spreadRange() const { return spreadRadius_; }

int Plant::nutrition() const {
    return std::max(1, health_ / 10);
}

int Plant::healthPercent() const {
    return maxHealth_ == 0 ? 0 : (health_ * 100) / maxHealth_;
}

std::string Plant::describe() const {
    std::ostringstream out;
    out << "Plant " << id_ << " at " << position_.toString() << " (health "
        << healthPercent() << "%, seeds " << seedCount_ << ')';
    return out.str();
}

void Plant::takeDamage(int amount) {
    health_ = std::max(0, health_ - std::max(0, amount));
}

bool Plant::isWilted() const { return healthPercent() < 30; }

int Plant::health() const { return health_; }
int Plant::maxHealth() const { return maxHealth_; }

void Plant::setHealth(int health) {
    if (health < 0 || health > maxHealth_) {
        throw InvalidVitalException("Plant health must be in [0, 100]");
    }
    health_ = health;
}

int Plant::seedCount() const { return seedCount_; }

void Plant::setSeedCount(int seeds) {
    if (seeds < 0) {
        throw InvalidVitalException("Seed count must not be negative");
    }
    seedCount_ = seeds;
}

int Plant::decayRate() const { return decayRate_; }

Animal::Animal(std::string species, const Position& position, Sex sex, int size,
               int maxAge, int maturityAge, char symbol, DietType diet,
               int hungerLimit, int speed)
    : Inhabitant(std::move(species), position, size, maxAge, symbol),
      sex_(sex),
      vital_(1, 1, 0, hungerLimit),
      genome_(),
      hungerTurns_(0),
      hungerTurnsLimit_(hungerLimit),
      breedingCooldown_(0),
      maturityAge_(maturityAge),
      diet_(diet),
      speed_(speed) {
    if (hungerLimit <= 0 || speed <= 0 || maturityAge < 0) {
        throw InvalidConfigurationException("Invalid animal traits");
    }
}

Sex Animal::sex() const { return sex_; }
void Animal::setSex(Sex sex) { sex_ = sex; }
VitalStats& Animal::vital() { return vital_; }
const VitalStats& Animal::vital() const { return vital_; }
Genome& Animal::genome() { return genome_; }
const Genome& Animal::genome() const { return genome_; }

bool Animal::isHungry() const {
    return vital_.hunger() * 2 >= vital_.maxHunger();
}

bool Animal::isStarving() const {
    return vital_.isStarving() || hungerTurns_ >= hungerTurnsLimit_;
}

int Animal::speed() const { return speed_; }

int Animal::maturityAge() const { return maturityAge_; }
int Animal::hungerLimit() const { return hungerTurnsLimit_; }
DietType Animal::diet() const { return diet_; }

void Animal::growHunger() {
    vital_.starve();
    ++hungerTurns_;
}

void Animal::feed(int nutrition) {
    vital_.feed(nutrition);
    resetHungerTurns();
}

void Animal::ageOneTurn() {
    Inhabitant::ageOneTurn();
    tickCooldown();
}

bool Animal::canBreed() const {
    return alive_ && age_ >= maturityAge_ && breedingCooldown_ == 0 &&
           !isStarving();
}

bool Animal::canBreedWith(const Animal& other) const {
    return speciesName_ == other.speciesName_ && sex_ != other.sex_ &&
           position_ == other.position_ && canBreed() && other.canBreed();
}

void Animal::startBreedingCooldown() { breedingCooldown_ = 2; }

int Animal::breedingCooldown() const { return breedingCooldown_; }

void Animal::tickCooldown() {
    if (breedingCooldown_ > 0) {
        --breedingCooldown_;
    }
}

bool Animal::canReach(const Position& target) const {
    return position_.chebyshevDistanceTo(target) <= speed_;
}

bool Animal::moveTowards(const Position& target, const FieldSize& fieldSize) {
    Position next = position_;
    const int steps =
        std::min(speed(), position_.chebyshevDistanceTo(target));
    for (int i = 0; i < steps; ++i) {
        if (next.row() < target.row()) {
            next.setRow(next.row() + 1);
        } else if (next.row() > target.row()) {
            next.setRow(next.row() - 1);
        }
        if (next.col() < target.col()) {
            next.setCol(next.col() + 1);
        } else if (next.col() > target.col()) {
            next.setCol(next.col() - 1);
        }
    }
    const Position clamped = fieldSize.clamp(next);
    const bool moved = clamped != position_;
    position_ = clamped;
    return moved;
}

std::string Animal::statusLine() const {
    std::ostringstream out;
    out << speciesName_ << ' ' << id_ << " at " << position_.toString()
        << ", " << toString(sex_) << ", age " << age_ << '/' << maxAge_
        << ", hunger " << vital_.hunger() << '/' << vital_.maxHunger();
    return out.str();
}

std::string Animal::describe() const {
    std::ostringstream out;
    out << speciesName_ << " #" << id_ << " at " << position_.toString()
        << " [" << toString(sex_) << ", size " << size_ << ", age " << age_
        << '/' << maxAge_ << ", " << toString(diet_) << ']';
    return out.str();
}

SpeciesProfile Animal::buildProfile() const {
    const std::string food =
        diet_ == DietType::Herbivore ? "plants" : "meat";
    DietProfile diet(diet_, std::max(1, size_ / 2 + 1), hungerTurnsLimit_,
                     food);
    MovementProfile movement(speed_, false, false, 10 + size_);
    ReproductionProfile reproduction(maturityAge_, 1, 1, 2, Season::Spring);
    CombatProfile combat(size_, size_ + 1, size_ / 2,
                         diet_ == DietType::Herbivore ? 0.35 : 0.15);
    return SpeciesProfile(speciesName_, size_, maxAge_, speed_, diet, movement,
                          reproduction, combat);
}

void Animal::resetHungerTurns() { hungerTurns_ = 0; }
int Animal::hungerTurns() const { return hungerTurns_; }
bool Animal::isAnimal() const { return true; }

Herbivore::Herbivore(std::string species, const Position& position, Sex sex,
                     int size, int maxAge, int maturityAge, char symbol,
                     int speed, int hungerLimit, int grazeRate,
                     int fleeInstinct, int herdSize)
    : Animal(std::move(species), position, sex, size, maxAge, maturityAge,
             symbol, DietType::Herbivore, hungerLimit, speed),
      grazeRate_(grazeRate),
      fleeInstinct_(fleeInstinct),
      herdSize_(herdSize) {}

int Herbivore::plantBite() const { return grazeRate_; }
int Herbivore::fleeChance() const { return std::min(95, fleeInstinct_); }

int Herbivore::graze(Plant& plant) {
    if (!canGraze(plant)) {
        throw InvalidDietException("Herbivore cannot graze a dead plant");
    }
    const int nutrition = nutritionFrom(plant);
    const int bite = std::min(plant.health(), plantBite());
    plant.takeDamage(bite);
    feed(nutrition);
    return bite;
}

bool Herbivore::canGraze(const Plant& plant) const {
    return plant.alive() && plant.health() > 0;
}

bool Herbivore::fleeFrom(const Predator& predator) const {
    return fleeChance() >= 40 || predator.size() < size_;
}

int Herbivore::preferredPlantHealth() const { return 60; }

int Herbivore::nutritionFrom(const Plant& plant) const {
    return std::max(1, plant.nutrition());
}

std::string Herbivore::grazeReport() const {
    std::ostringstream out;
    out << speciesName_ << " grazes up to " << plantBite()
        << " plant points, flee chance " << fleeChance() << "%, herd "
        << herdSize_;
    return out.str();
}

Predator::Predator(std::string species, const Position& position, Sex sex,
                   int size, int maxAge, int maturityAge, char symbol,
                   int speed, int hungerLimit, int huntingSkill,
                   int territoryRadius)
    : Animal(std::move(species), position, sex, size, maxAge, maturityAge,
             symbol, DietType::Carnivore, hungerLimit, speed),
      huntingSkill_(huntingSkill),
      killCount_(0),
      territoryRadius_(territoryRadius) {}

int Predator::maxPreySize() const { return size_ + 1; }
double Predator::huntingSkill() const {
    return std::min(100, huntingSkill_) / 100.0;
}

bool Predator::canHunt(const Herbivore& prey) const {
    return prey.alive() && prey.size() <= maxPreySize();
}

int Predator::hunt(Herbivore& prey) {
    if (!canHunt(prey)) {
        throw InvalidDietException("Prey is too large or already dead");
    }
    const int nutrition = std::max(2, prey.size() * 2);
    prey.kill();
    ++killCount_;
    feed(nutrition);
    return nutrition;
}

bool Predator::chase(const Herbivore& prey) {
    return canHunt(prey) && huntingSkill_ >= 30;
}

int Predator::hungerAfterKill() const {
    return std::max(0, hungerTurnsLimit_ - 2);
}

int Predator::territoryRadius() const { return territoryRadius_; }

std::string Predator::huntReport() const {
    std::ostringstream out;
    out << speciesName_ << " hunts prey up to size " << maxPreySize()
        << ", skill " << static_cast<int>(huntingSkill() * 100.0)
        << "%, kills " << killCount_ << ", territory "
        << territoryRadius_;
    return out.str();
}

} // namespace animalworld
