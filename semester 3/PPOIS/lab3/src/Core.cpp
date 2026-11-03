#include "animalworld/Core.h"

#include "animalworld/Exceptions.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace animalworld {

namespace {

double clampValue(double value, double low, double high) {
    return std::max(low, std::min(high, value));
}

int clampInt(int value, int low, int high) {
    return std::max(low, std::min(high, value));
}

} // namespace

std::string toString(Sex value) {
    switch (value) {
    case Sex::Male: return "male";
    case Sex::Female: return "female";
    }
    return "unknown";
}

std::string toString(DietType value) {
    switch (value) {
    case DietType::Herbivore: return "herbivore";
    case DietType::Carnivore: return "carnivore";
    case DietType::Omnivore: return "omnivore";
    }
    return "unknown";
}

std::string toString(Direction value) {
    switch (value) {
    case Direction::North: return "north";
    case Direction::South: return "south";
    case Direction::East: return "east";
    case Direction::West: return "west";
    case Direction::None: return "none";
    }
    return "none";
}

std::string toString(Season value) {
    switch (value) {
    case Season::Spring: return "spring";
    case Season::Summer: return "summer";
    case Season::Autumn: return "autumn";
    case Season::Winter: return "winter";
    }
    return "unknown";
}

std::string toString(EventType value) {
    switch (value) {
    case EventType::Movement: return "movement";
    case EventType::Feeding: return "feeding";
    case EventType::Birth: return "birth";
    case EventType::Death: return "death";
    case EventType::Reproduction: return "reproduction";
    case EventType::System: return "system";
    }
    return "unknown";
}

Position::Position() : row_(0), col_(0) {}

Position::Position(int row, int col) : row_(row), col_(col) {
    if (row_ < 0 || col_ < 0) {
        throw InvalidPositionException("Position coordinates must not be negative");
    }
}

int Position::row() const { return row_; }
int Position::col() const { return col_; }

void Position::setRow(int row) {
    if (row < 0) {
        throw InvalidPositionException("Row must not be negative");
    }
    row_ = row;
}

void Position::setCol(int col) {
    if (col < 0) {
        throw InvalidPositionException("Column must not be negative");
    }
    col_ = col;
}

int Position::distanceTo(const Position& other) const {
    return std::abs(row_ - other.row_) + std::abs(col_ - other.col_);
}

int Position::chebyshevDistanceTo(const Position& other) const {
    return std::max(std::abs(row_ - other.row_), std::abs(col_ - other.col_));
}

bool Position::isAdjacentTo(const Position& other) const {
    if (*this == other) {
        return false;
    }
    return chebyshevDistanceTo(other) == 1;
}

Position Position::movedBy(Direction direction, int steps) const {
    const int amount = std::max(0, steps);
    switch (direction) {
    case Direction::North: return Position(row_ - amount, col_);
    case Direction::South: return Position(row_ + amount, col_);
    case Direction::East: return Position(row_, col_ + amount);
    case Direction::West: return Position(row_, col_ - amount);
    case Direction::None: return *this;
    }
    return *this;
}

Position Position::clampedTo(int rows, int cols) const {
    if (rows <= 0 || cols <= 0) {
        throw InvalidFieldSizeException("Cannot clamp into an empty field");
    }
    return Position(clampInt(row_, 0, rows - 1), clampInt(col_, 0, cols - 1));
}

bool Position::isWithin(int rows, int cols) const {
    return row_ >= 0 && row_ < rows && col_ >= 0 && col_ < cols;
}

std::vector<Position> Position::neighbors(int rows, int cols) const {
    std::vector<Position> result;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) {
                continue;
            }
            const int nr = row_ + dr;
            const int nc = col_ + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                result.emplace_back(nr, nc);
            }
        }
    }
    return result;
}

Direction Position::directionTowards(const Position& other) const {
    if (other.row_ < row_) {
        return Direction::North;
    }
    if (other.row_ > row_) {
        return Direction::South;
    }
    if (other.col_ < col_) {
        return Direction::West;
    }
    if (other.col_ > col_) {
        return Direction::East;
    }
    return Direction::None;
}

std::string Position::toString() const {
    std::ostringstream out;
    out << '(' << row_ << ',' << col_ << ')';
    return out.str();
}

bool Position::operator==(const Position& other) const {
    return row_ == other.row_ && col_ == other.col_;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

FieldSize::FieldSize() : rows_(1), cols_(1) {}

FieldSize::FieldSize(int rows, int cols) : rows_(rows), cols_(cols) {
    if (rows_ <= 0 || cols_ <= 0) {
        throw InvalidFieldSizeException("Field dimensions must be positive");
    }
}

int FieldSize::rows() const { return rows_; }
int FieldSize::cols() const { return cols_; }

int FieldSize::area() const { return rows_ * cols_; }

bool FieldSize::isInside(const Position& position) const {
    return position.isWithin(rows_, cols_);
}

Position FieldSize::clamp(const Position& position) const {
    return position.clampedTo(rows_, cols_);
}

Position FieldSize::randomPosition(RandomSource& random) const {
    return Position(random.nextInt(0, rows_ - 1), random.nextInt(0, cols_ - 1));
}

bool FieldSize::isBorder(const Position& position) const {
    return position.row() == 0 || position.col() == 0 ||
           position.row() == rows_ - 1 || position.col() == cols_ - 1;
}

int FieldSize::neighborhoodCount(const Position& position) const {
    return static_cast<int>(position.neighbors(rows_, cols_).size());
}

VitalStats::VitalStats()
    : health_(10), maxHealth_(10), hunger_(0), maxHunger_(10) {}

VitalStats::VitalStats(int health, int maxHealth, int hunger, int maxHunger)
    : health_(health), maxHealth_(maxHealth), hunger_(hunger),
      maxHunger_(maxHunger) {
    if (maxHealth <= 0 || maxHunger <= 0) {
        throw InvalidVitalException("Maximum health and hunger must be positive");
    }
    if (health_ < 0 || health_ > maxHealth_) {
        throw InvalidVitalException("Health must be in [0, maxHealth]");
    }
    if (hunger_ < 0 || hunger_ > maxHunger_) {
        throw InvalidVitalException("Hunger must be in [0, maxHunger]");
    }
}

int VitalStats::health() const { return health_; }
int VitalStats::maxHealth() const { return maxHealth_; }
int VitalStats::hunger() const { return hunger_; }
int VitalStats::maxHunger() const { return maxHunger_; }

void VitalStats::damage(int amount) {
    health_ = clampInt(health_ - std::max(0, amount), 0, maxHealth_);
}

void VitalStats::heal(int amount) {
    health_ = clampInt(health_ + std::max(0, amount), 0, maxHealth_);
}

void VitalStats::feed(int amount) {
    hunger_ = clampInt(hunger_ - std::max(0, amount), 0, maxHunger_);
}

void VitalStats::starve() {
    hunger_ = clampInt(hunger_ + 1, 0, maxHunger_);
}

bool VitalStats::isDead() const { return health_ <= 0; }

bool VitalStats::isStarving() const { return hunger_ >= maxHunger_; }

int VitalStats::healthPercent() const {
    return maxHealth_ == 0 ? 0 : (health_ * 100) / maxHealth_;
}

int VitalStats::hungerPercent() const {
    return maxHunger_ == 0 ? 0 : (hunger_ * 100) / maxHunger_;
}

void VitalStats::reset() {
    health_ = maxHealth_;
    hunger_ = 0;
}

DietProfile::DietProfile()
    : type_(DietType::Herbivore), foodPerTurn_(1), hungerThreshold_(3),
      preferredFood_("plants") {}

DietProfile::DietProfile(DietType type, int foodPerTurn, int hungerThreshold,
                         std::string preferredFood)
    : type_(type), foodPerTurn_(foodPerTurn), hungerThreshold_(hungerThreshold),
      preferredFood_(std::move(preferredFood)) {
    if (foodPerTurn_ <= 0 || hungerThreshold_ <= 0) {
        throw InvalidDietException("Diet parameters must be positive");
    }
    if (preferredFood_.empty()) {
        throw InvalidDietException("Preferred food must not be empty");
    }
}

DietType DietProfile::type() const { return type_; }
int DietProfile::foodPerTurn() const { return foodPerTurn_; }
int DietProfile::hungerThreshold() const { return hungerThreshold_; }
const std::string& DietProfile::preferredFood() const { return preferredFood_; }

bool DietProfile::canEatPlant() const {
    return type_ == DietType::Herbivore || type_ == DietType::Omnivore;
}

bool DietProfile::canEatAnimal() const {
    return type_ == DietType::Carnivore || type_ == DietType::Omnivore;
}

bool DietProfile::isStarvingAt(int turnsWithoutFood) const {
    return turnsWithoutFood >= hungerThreshold_;
}

int DietProfile::nutritionFor(int foodMass) const {
    if (foodMass <= 0) {
        return 0;
    }
    return std::max(1, (foodMass * foodPerTurn_) / 10);
}

std::string DietProfile::describe() const {
    std::ostringstream out;
    out << toString(type_) << " eating " << preferredFood_ << " ("
        << foodPerTurn_ << " units/turn)";
    return out.str();
}

MovementProfile::MovementProfile()
    : maxSteps_(1), canFly_(false), canSwim_(false), stamina_(10) {}

MovementProfile::MovementProfile(int maxSteps, bool canFly, bool canSwim,
                                 int stamina)
    : maxSteps_(maxSteps), canFly_(canFly), canSwim_(canSwim),
      stamina_(stamina) {
    if (maxSteps_ <= 0 || stamina_ <= 0) {
        throw InvalidConfigurationException(
            "Movement range and stamina must be positive");
    }
}

int MovementProfile::maxSteps() const { return maxSteps_; }
bool MovementProfile::canFly() const { return canFly_; }
bool MovementProfile::canSwim() const { return canSwim_; }
int MovementProfile::stamina() const { return stamina_; }

int MovementProfile::effectiveRange(int staminaLeft) const {
    if (staminaLeft <= 0) {
        return 0;
    }
    if (staminaLeft * 2 < stamina_) {
        return std::max(1, maxSteps_ / 2);
    }
    return maxSteps_;
}

bool MovementProfile::canReach(const Position& from,
                               const Position& to) const {
    return from.chebyshevDistanceTo(to) <= maxSteps_;
}

int MovementProfile::staminaCost(int distance) const {
    return std::max(1, distance);
}

bool MovementProfile::requiresRest(int staminaLeft) const {
    return staminaLeft * 4 < stamina_;
}

std::string MovementProfile::describe() const {
    std::ostringstream out;
    out << maxSteps_ << " cells/turn";
    if (canFly_) {
        out << ", flying";
    }
    if (canSwim_) {
        out << ", swimming";
    }
    return out.str();
}

ReproductionProfile::ReproductionProfile()
    : maturityAge_(2), gestationTurns_(1), litterSize_(1), cooldown_(2),
      breedingSeason_(Season::Spring) {}

ReproductionProfile::ReproductionProfile(int maturityAge, int gestationTurns,
                                         int litterSize, int cooldown,
                                         Season breedingSeason)
    : maturityAge_(maturityAge), gestationTurns_(gestationTurns),
      litterSize_(litterSize), cooldown_(cooldown),
      breedingSeason_(breedingSeason) {
    if (maturityAge_ < 0 || gestationTurns_ < 0 || litterSize_ <= 0 ||
        cooldown_ < 0) {
        throw ReproductionException("Invalid reproduction profile");
    }
}

int ReproductionProfile::maturityAge() const { return maturityAge_; }
int ReproductionProfile::gestationTurns() const { return gestationTurns_; }
int ReproductionProfile::litterSize() const { return litterSize_; }
int ReproductionProfile::cooldown() const { return cooldown_; }
Season ReproductionProfile::breedingSeason() const { return breedingSeason_; }

bool ReproductionProfile::canBreedAtAge(int age) const {
    return age >= maturityAge_;
}

bool ReproductionProfile::isInSeason(Season season) const {
    return breedingSeason_ == season || breedingSeason_ == Season::Spring;
}

int ReproductionProfile::offspringCount(int availableSlots) const {
    return std::max(0, std::min(litterSize_, availableSlots));
}

int ReproductionProfile::cooldownAfterBirth() const { return cooldown_; }

std::string ReproductionProfile::describe() const {
    std::ostringstream out;
    out << "matures at " << maturityAge_ << ", litter " << litterSize_
        << ", cooldown " << cooldown_ << " (" << toString(breedingSeason_)
        << ')';
    return out.str();
}

CombatProfile::CombatProfile()
    : bodySize_(1), attackPower_(1), defense_(0), escapeChance_(0.0) {}

CombatProfile::CombatProfile(int bodySize, int attackPower, int defense,
                             double escapeChance)
    : bodySize_(bodySize), attackPower_(attackPower), defense_(defense),
      escapeChance_(escapeChance) {
    if (bodySize_ <= 0 || attackPower_ <= 0 || defense_ < 0) {
        throw InvalidConfigurationException("Invalid combat profile");
    }
    if (escapeChance_ < 0.0 || escapeChance_ > 1.0) {
        throw InvalidConfigurationException(
            "Escape chance must be in [0, 1]");
    }
}

int CombatProfile::bodySize() const { return bodySize_; }
int CombatProfile::attackPower() const { return attackPower_; }
int CombatProfile::defense() const { return defense_; }
double CombatProfile::escapeChance() const { return escapeChance_; }

bool CombatProfile::canDefeat(int preySize) const {
    return bodySize_ >= preySize;
}

double CombatProfile::escapeProbability(int predatorSize) const {
    const int gap = std::max(0, predatorSize - bodySize_);
    return clampValue(escapeChance_ - gap * 0.05, 0.0, 1.0);
}

int CombatProfile::damageFor(int attack) const {
    return std::max(1, attack + attackPower_ - defense_);
}

bool CombatProfile::isLargerThan(const CombatProfile& other) const {
    return bodySize_ > other.bodySize_;
}

std::string CombatProfile::describe() const {
    std::ostringstream out;
    out << "size " << bodySize_ << ", attack " << attackPower_ << ", defense "
        << defense_ << ", escape "
        << static_cast<int>(escapeChance_ * 100.0) << '%';
    return out.str();
}

Genome::Genome() : aggression_(0.5), speedBonus_(0.0), resilience_(0.5) {}

Genome::Genome(double aggression, double speedBonus, double resilience)
    : aggression_(aggression), speedBonus_(speedBonus),
      resilience_(resilience) {
    const auto valid = [](double value) {
        return value >= -1.0 && value <= 1.0;
    };
    if (!valid(aggression_) || !valid(speedBonus_) || !valid(resilience_)) {
        throw InvalidGenomeException("Genome coefficients must be in [-1, 1]");
    }
}

double Genome::aggression() const { return aggression_; }
double Genome::speedBonus() const { return speedBonus_; }
double Genome::resilience() const { return resilience_; }

Genome Genome::mutatedCopy(RandomSource& random) const {
    const auto mutate = [&random](double base) {
        const double delta = (random.nextDouble() - 0.5) * 0.2;
        return clampValue(base + delta, -1.0, 1.0);
    };
    return Genome(mutate(aggression_), mutate(speedBonus_),
                  mutate(resilience_));
}

double Genome::similarity(const Genome& other) const {
    const double distance = std::abs(aggression_ - other.aggression_) +
                            std::abs(speedBonus_ - other.speedBonus_) +
                            std::abs(resilience_ - other.resilience_);
    return clampValue(1.0 - distance / 6.0, 0.0, 1.0);
}

double Genome::dominantGene() const {
    return std::max({std::abs(aggression_), std::abs(speedBonus_),
                     std::abs(resilience_)});
}

double Genome::express(double environmentalPressure) const {
    const double pressure = clampValue(environmentalPressure, 0.0, 1.0);
    return clampValue(aggression_ * pressure +
                          resilience_ * (1.0 - pressure) + speedBonus_ * 0.1,
                      -1.0, 1.0);
}

std::string Genome::describe() const {
    std::ostringstream out;
    out << "aggression " << aggression_ << ", speed " << speedBonus_
        << ", resilience " << resilience_;
    return out.str();
}

SpeciesProfile::SpeciesProfile()
    : name_("Unknown"), size_(1), maxAge_(1), speed_(1) {}

SpeciesProfile::SpeciesProfile(std::string name, int size, int maxAge,
                               int speed, DietProfile diet,
                               MovementProfile movement,
                               ReproductionProfile reproduction,
                               CombatProfile combat)
    : name_(std::move(name)), size_(size), maxAge_(maxAge), speed_(speed),
      diet_(std::move(diet)), movement_(std::move(movement)),
      reproduction_(std::move(reproduction)), combat_(std::move(combat)) {}

const std::string& SpeciesProfile::name() const { return name_; }
int SpeciesProfile::size() const { return size_; }
int SpeciesProfile::maxAge() const { return maxAge_; }
int SpeciesProfile::speed() const { return speed_; }
const DietProfile& SpeciesProfile::diet() const { return diet_; }
const MovementProfile& SpeciesProfile::movement() const { return movement_; }
const ReproductionProfile& SpeciesProfile::reproduction() const {
    return reproduction_;
}
const CombatProfile& SpeciesProfile::combat() const { return combat_; }

bool SpeciesProfile::validate() const {
    if (name_.empty() || size_ <= 0 || maxAge_ <= 0 || speed_ <= 0) {
        return false;
    }
    if (diet_.type() == DietType::Herbivore && !diet_.canEatPlant()) {
        return false;
    }
    if (diet_.type() == DietType::Carnivore && !diet_.canEatAnimal()) {
        return false;
    }
    return true;
}

bool SpeciesProfile::isPredator() const {
    return diet_.type() == DietType::Carnivore ||
           diet_.type() == DietType::Omnivore;
}

bool SpeciesProfile::isHerbivore() const {
    return diet_.type() == DietType::Herbivore;
}

std::string SpeciesProfile::summary() const {
    std::ostringstream out;
    out << name_ << " [size " << size_ << ", speed " << speed_ << ", "
        << diet_.describe() << ']';
    return out.str();
}

bool SpeciesProfile::compatibleWith(const SpeciesProfile& other) const {
    const int larger = std::max(size_, other.size_);
    const int smaller = std::min(size_, other.size_);
    return diet_.type() == other.diet_.type() && larger <= smaller * 2;
}

int SpeciesProfile::expectedLongevity() const {
    return std::max(1, maxAge_ - reproduction_.maturityAge());
}

SpeciesRegistry::SpeciesRegistry() = default;

void SpeciesRegistry::registerSpecies(const SpeciesProfile& profile,
                                      Factory factory) {
    if (!profile.validate()) {
        throw InvalidConfigurationException("Cannot register invalid species '" +
                                           profile.name() + "'");
    }
    if (!factory) {
        throw InvalidConfigurationException("Species factory must be callable");
    }
    profiles_[profile.name()] = profile;
    factories_[profile.name()] = std::move(factory);
}

std::shared_ptr<Animal> SpeciesRegistry::create(const std::string& name,
                                                const Position& position,
                                                Sex sex) const {
    const auto it = factories_.find(name);
    if (it == factories_.end()) {
        throw UnknownSpeciesException("Unknown species '" + name + "'");
    }
    return it->second(position, sex);
}

bool SpeciesRegistry::hasSpecies(const std::string& name) const {
    return factories_.find(name) != factories_.end();
}

int SpeciesRegistry::speciesCount() const {
    return static_cast<int>(factories_.size());
}

std::vector<std::string> SpeciesRegistry::names() const {
    std::vector<std::string> result;
    result.reserve(factories_.size());
    for (const auto& entry : factories_) {
        result.push_back(entry.first);
    }
    return result;
}

SpeciesProfile SpeciesRegistry::profileOf(const std::string& name) const {
    const auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        throw UnknownSpeciesException("Unknown species '" + name + "'");
    }
    return it->second;
}

Environment::Environment()
    : season_(Season::Spring), temperature_(18.0), rainfall_(0.4),
      dayLength_(12) {}

Environment::Environment(Season season, double temperature, double rainfall,
                         int dayLength)
    : season_(season), temperature_(temperature), rainfall_(rainfall),
      dayLength_(dayLength) {
    if (rainfall_ < 0.0 || rainfall_ > 1.0 || dayLength_ < 0 ||
        dayLength_ > 24) {
        throw InvalidConfigurationException("Invalid environment parameters");
    }
}

Season Environment::season() const { return season_; }
double Environment::temperature() const { return temperature_; }
double Environment::rainfall() const { return rainfall_; }
int Environment::dayLength() const { return dayLength_; }

bool Environment::isHarsh() const {
    return temperature_ < 0.0 || temperature_ > 35.0 || rainfall_ > 0.85;
}

double Environment::plantGrowthFactor() const {
    double factor = 1.0;
    switch (season_) {
    case Season::Spring: factor = 1.4; break;
    case Season::Summer: factor = 1.2; break;
    case Season::Autumn: factor = 0.8; break;
    case Season::Winter: factor = 0.3; break;
    }
    factor *= 0.5 + rainfall_;
    if (temperature_ < 5.0) {
        factor *= 0.5;
    }
    return clampValue(factor, 0.05, 2.0);
}

int Environment::affectsHunger(int baseHunger) const {
    const int penalty = isHarsh() ? 2 : (temperature_ < 10.0 ? 1 : 0);
    return std::max(0, baseHunger + penalty);
}

std::string Environment::describe() const {
    std::ostringstream out;
    out << seasonName() << ", " << temperature_ << "C, rainfall "
        << static_cast<int>(rainfall_ * 100.0) << "%, day " << dayLength_
        << "h";
    return out.str();
}

std::string Environment::seasonName() const {
    return toString(season_);
}

Habitat::Habitat()
    : biome_("meadow"), vegetationDensity_(0.5), waterAccess_(true),
      temperature_(18) {}

Habitat::Habitat(std::string biome, double vegetationDensity, bool waterAccess,
                 int temperature)
    : biome_(std::move(biome)), vegetationDensity_(vegetationDensity),
      waterAccess_(waterAccess), temperature_(temperature) {
    if (biome_.empty() || vegetationDensity_ < 0.0 ||
        vegetationDensity_ > 1.0) {
        throw InvalidHabitatException("Invalid habitat parameters");
    }
}

const std::string& Habitat::biome() const { return biome_; }
double Habitat::vegetationDensity() const { return vegetationDensity_; }
bool Habitat::waterAccess() const { return waterAccess_; }
int Habitat::temperature() const { return temperature_; }

double Habitat::suitabilityFor(const SpeciesProfile& profile) const {
    double score = vegetationDensity_;
    if (profile.isPredator()) {
        score *= 0.8;
    }
    if (waterAccess_) {
        score += 0.2;
    }
    if (profile.movement().canSwim() && !waterAccess_) {
        score -= 0.3;
    }
    return clampValue(score, 0.0, 1.0);
}

int Habitat::qualityRating() const {
    int rating = static_cast<int>(vegetationDensity_ * 60.0);
    if (waterAccess_) {
        rating += 20;
    }
    if (temperature_ >= 5 && temperature_ <= 28) {
        rating += 20;
    }
    return clampInt(rating, 0, 100);
}

int Habitat::plantCapacity() const {
    return static_cast<int>(vegetationDensity_ * 100.0);
}

bool Habitat::hazardLevelHigh() const {
    return temperature_ < 0 || temperature_ > 32 || vegetationDensity_ < 0.15;
}

std::string Habitat::describe() const {
    std::ostringstream out;
    out << biome_ << " (vegetation "
        << static_cast<int>(vegetationDensity_ * 100.0) << "%, water "
        << (waterAccess_ ? "yes" : "no") << ", " << temperature_ << "C)";
    return out.str();
}

RandomSource::RandomSource() : RandomSource(12345UL) {}

RandomSource::RandomSource(unsigned long seed)
    : seed_(seed), engine_(static_cast<std::mt19937::result_type>(seed)),
      calls_(0) {}

int RandomSource::nextInt(int low, int high) {
    if (low > high) {
        std::swap(low, high);
    }
    std::uniform_int_distribution<int> distribution(low, high);
    ++calls_;
    return distribution(engine_);
}

double RandomSource::nextDouble() {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    ++calls_;
    return distribution(engine_);
}

bool RandomSource::chance(double probability) {
    return nextDouble() < clampValue(probability, 0.0, 1.0);
}

unsigned long RandomSource::seed() const { return seed_; }
long RandomSource::calls() const { return calls_; }

void RandomSource::reseed(unsigned long seed) {
    seed_ = seed;
    engine_.seed(static_cast<std::mt19937::result_type>(seed));
    calls_ = 0;
}

SimulationConfig::SimulationConfig()
    : rows_(8), cols_(8), steps_(10), plantGrowthChance_(0.25),
      autoMode_(true) {}

SimulationConfig::SimulationConfig(int rows, int cols, int steps,
                                   double plantGrowthChance, bool autoMode)
    : rows_(rows), cols_(cols), steps_(steps),
      plantGrowthChance_(plantGrowthChance), autoMode_(autoMode) {
    validate();
}

int SimulationConfig::rows() const { return rows_; }
int SimulationConfig::cols() const { return cols_; }
int SimulationConfig::steps() const { return steps_; }
double SimulationConfig::plantGrowthChance() const {
    return plantGrowthChance_;
}
bool SimulationConfig::autoMode() const { return autoMode_; }

void SimulationConfig::setRows(int rows) { rows_ = rows; }
void SimulationConfig::setCols(int cols) { cols_ = cols; }
void SimulationConfig::setSteps(int steps) { steps_ = steps; }
void SimulationConfig::setPlantGrowthChance(double chance) {
    plantGrowthChance_ = chance;
}
void SimulationConfig::setAutoMode(bool autoMode) { autoMode_ = autoMode; }

void SimulationConfig::validate() const {
    if (rows_ <= 0 || cols_ <= 0) {
        throw InvalidConfigurationException(
            "Field dimensions must be positive");
    }
    if (rows_ > 1000 || cols_ > 1000) {
        throw InvalidConfigurationException("Field dimensions are too large");
    }
    if (steps_ < 0) {
        throw InvalidConfigurationException("Step count must not be negative");
    }
    if (plantGrowthChance_ < 0.0 || plantGrowthChance_ > 1.0) {
        throw InvalidConfigurationException(
            "Plant growth chance must be in [0, 1]");
    }
}

bool SimulationConfig::isAutoMode() const { return autoMode_; }

std::string SimulationConfig::describe() const {
    std::ostringstream out;
    out << rows_ << 'x' << cols_ << " field, " << steps_ << " steps, growth "
        << static_cast<int>(plantGrowthChance_ * 100.0) << "%, "
        << (autoMode_ ? "automatic" : "step-by-step") << " mode";
    return out.str();
}

int SimulationConfig::totalCells() const { return rows_ * cols_; }

int SimulationConfig::recommendedPlants() const {
    return std::max(1, totalCells() / 4);
}

SimulationStatistics::SimulationStatistics()
    : births_(0), deaths_(0), meals_(0), stepsElapsed_(0),
      peakPopulation_(0), lastDeathStep_(0) {}

void SimulationStatistics::recordBirth() { ++births_; }
void SimulationStatistics::recordDeath(int step) {
    ++deaths_;
    lastDeathStep_ = step;
}
void SimulationStatistics::recordMeal() { ++meals_; }
void SimulationStatistics::recordStep() { ++stepsElapsed_; }

void SimulationStatistics::updatePeak(int population) {
    peakPopulation_ = std::max(peakPopulation_, population);
}

double SimulationStatistics::averageDeaths(int steps) const {
    if (steps <= 0) {
        return 0.0;
    }
    return static_cast<double>(deaths_) / static_cast<double>(steps);
}

std::string SimulationStatistics::summary() const {
    std::ostringstream out;
    out << "steps " << stepsElapsed_ << ", births " << births_ << ", deaths "
        << deaths_ << ", meals " << meals_ << ", peak population "
        << peakPopulation_;
    return out.str();
}

void SimulationStatistics::reset() {
    births_ = 0;
    deaths_ = 0;
    meals_ = 0;
    stepsElapsed_ = 0;
    peakPopulation_ = 0;
    lastDeathStep_ = 0;
}

int SimulationStatistics::births() const { return births_; }
int SimulationStatistics::deaths() const { return deaths_; }
int SimulationStatistics::meals() const { return meals_; }
int SimulationStatistics::stepsElapsed() const { return stepsElapsed_; }
int SimulationStatistics::peakPopulation() const { return peakPopulation_; }
int SimulationStatistics::lastDeathStep() const { return lastDeathStep_; }

WorldEvent::WorldEvent()
    : type_(EventType::System), message_(), step_(0), position_() {}

WorldEvent::WorldEvent(EventType type, std::string message, int step,
                       Position position)
    : type_(type), message_(std::move(message)), step_(step),
      position_(position) {}

EventType WorldEvent::type() const { return type_; }
const std::string& WorldEvent::message() const { return message_; }
int WorldEvent::step() const { return step_; }
const Position& WorldEvent::position() const { return position_; }

std::string WorldEvent::describe() const {
    std::ostringstream out;
    out << '[' << toString(type_) << "] " << message_ << " at "
        << position_.toString();
    return out.str();
}

bool WorldEvent::isCritical() const {
    return type_ == EventType::Death || type_ == EventType::Birth;
}

bool WorldEvent::isBirth() const { return type_ == EventType::Birth; }
bool WorldEvent::isDeath() const { return type_ == EventType::Death; }

std::string WorldEvent::timestampLine() const {
    std::ostringstream out;
    out << "step " << step_ << ": " << describe();
    return out.str();
}

EventLog::EventLog() : events_(), capacity_(256) {}

EventLog::EventLog(std::size_t capacity) : events_(), capacity_(capacity) {}

void EventLog::add(const WorldEvent& event) {
    if (capacity_ == 0) {
        return;
    }
    events_.push_back(event);
    while (events_.size() > capacity_) {
        events_.erase(events_.begin());
    }
}

std::vector<WorldEvent> EventLog::recent(std::size_t count) const {
    if (count >= events_.size()) {
        return events_;
    }
    return std::vector<WorldEvent>(events_.end() -
                                       static_cast<std::ptrdiff_t>(count),
                                   events_.end());
}

bool EventLog::containsType(EventType type) const {
    return std::any_of(events_.begin(), events_.end(),
                       [type](const WorldEvent& event) {
                           return event.type() == type;
                       });
}

std::size_t EventLog::size() const { return events_.size(); }
void EventLog::clear() { events_.clear(); }

std::vector<std::string> EventLog::exportLines() const {
    std::vector<std::string> lines;
    lines.reserve(events_.size());
    for (const auto& event : events_) {
        lines.push_back(event.timestampLine());
    }
    return lines;
}

} // namespace animalworld
