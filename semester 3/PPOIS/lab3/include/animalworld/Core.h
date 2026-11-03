#ifndef ANIMALWORLD_CORE_H
#define ANIMALWORLD_CORE_H

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

/**
 * @file Core.h
 * @brief Value objects, trait profiles, configuration and bookkeeping types
 *        used by the animal world model.
 */

namespace animalworld {

class Animal;
class RandomSource;

enum class Sex { Male, Female };
enum class DietType { Herbivore, Carnivore, Omnivore };
enum class Direction { North, South, East, West, None };
enum class Season { Spring, Summer, Autumn, Winter };
enum class EventType { Movement, Feeding, Birth, Death, Reproduction, System };

std::string toString(Sex value);
std::string toString(DietType value);
std::string toString(Direction value);
std::string toString(Season value);
std::string toString(EventType value);

/** @brief An immutable row/column coordinate on the rectangular field. */
class Position {
public:
    Position();
    Position(int row, int col);

    int row() const;
    int col() const;
    void setRow(int row);
    void setCol(int col);

    int distanceTo(const Position& other) const;
    int chebyshevDistanceTo(const Position& other) const;
    bool isAdjacentTo(const Position& other) const;
    Position movedBy(Direction direction, int steps = 1) const;
    Position clampedTo(int rows, int cols) const;
    bool isWithin(int rows, int cols) const;
    std::vector<Position> neighbors(int rows, int cols) const;
    Direction directionTowards(const Position& other) const;
    std::string toString() const;

    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

private:
    int row_;
    int col_;
};

/** @brief Dimensions of the rectangular game field. */
class FieldSize {
public:
    FieldSize();
    FieldSize(int rows, int cols);

    int rows() const;
    int cols() const;
    int area() const;
    bool isInside(const Position& position) const;
    Position clamp(const Position& position) const;
    Position randomPosition(RandomSource& random) const;
    bool isBorder(const Position& position) const;
    int neighborhoodCount(const Position& position) const;

private:
    int rows_;
    int cols_;
};

/** @brief Health and hunger points of a living creature. */
class VitalStats {
public:
    VitalStats();
    VitalStats(int health, int maxHealth, int hunger, int maxHunger);

    int health() const;
    int maxHealth() const;
    int hunger() const;
    int maxHunger() const;

    void damage(int amount);
    void heal(int amount);
    void feed(int amount);
    void starve();
    bool isDead() const;
    bool isStarving() const;
    int healthPercent() const;
    int hungerPercent() const;
    void reset();

private:
    int health_;
    int maxHealth_;
    int hunger_;
    int maxHunger_;
};

/** @brief Description of what and how much a species eats. */
class DietProfile {
public:
    DietProfile();
    DietProfile(DietType type, int foodPerTurn, int hungerThreshold,
                std::string preferredFood);

    DietType type() const;
    int foodPerTurn() const;
    int hungerThreshold() const;
    const std::string& preferredFood() const;

    bool canEatPlant() const;
    bool canEatAnimal() const;
    bool isStarvingAt(int turnsWithoutFood) const;
    int nutritionFor(int foodMass) const;
    std::string describe() const;

private:
    DietType type_;
    int foodPerTurn_;
    int hungerThreshold_;
    std::string preferredFood_;
};

/** @brief How far and how a species can travel during one step. */
class MovementProfile {
public:
    MovementProfile();
    MovementProfile(int maxSteps, bool canFly, bool canSwim, int stamina);

    int maxSteps() const;
    bool canFly() const;
    bool canSwim() const;
    int stamina() const;

    int effectiveRange(int staminaLeft) const;
    bool canReach(const Position& from, const Position& to) const;
    int staminaCost(int distance) const;
    bool requiresRest(int staminaLeft) const;
    std::string describe() const;

private:
    int maxSteps_;
    bool canFly_;
    bool canSwim_;
    int stamina_;
};

/** @brief Timing and size of the reproductive cycle. */
class ReproductionProfile {
public:
    ReproductionProfile();
    ReproductionProfile(int maturityAge, int gestationTurns, int litterSize,
                        int cooldown, Season breedingSeason);

    int maturityAge() const;
    int gestationTurns() const;
    int litterSize() const;
    int cooldown() const;
    Season breedingSeason() const;

    bool canBreedAtAge(int age) const;
    bool isInSeason(Season season) const;
    int offspringCount(int availableSlots) const;
    int cooldownAfterBirth() const;
    std::string describe() const;

private:
    int maturityAge_;
    int gestationTurns_;
    int litterSize_;
    int cooldown_;
    Season breedingSeason_;
};

/** @brief Predation strength and escape chances of a species. */
class CombatProfile {
public:
    CombatProfile();
    CombatProfile(int bodySize, int attackPower, int defense,
                  double escapeChance);

    int bodySize() const;
    int attackPower() const;
    int defense() const;
    double escapeChance() const;

    bool canDefeat(int preySize) const;
    double escapeProbability(int predatorSize) const;
    int damageFor(int attack) const;
    bool isLargerThan(const CombatProfile& other) const;
    std::string describe() const;

private:
    int bodySize_;
    int attackPower_;
    int defense_;
    double escapeChance_;
};

/** @brief Heritable modifiers that make individuals slightly different. */
class Genome {
public:
    Genome();
    Genome(double aggression, double speedBonus, double resilience);

    double aggression() const;
    double speedBonus() const;
    double resilience() const;

    Genome mutatedCopy(RandomSource& random) const;
    double similarity(const Genome& other) const;
    double dominantGene() const;
    double express(double environmentalPressure) const;
    std::string describe() const;

private:
    double aggression_;
    double speedBonus_;
    double resilience_;
};

/** @brief Aggregated description of one species in the ecosystem. */
class SpeciesProfile {
public:
    SpeciesProfile();
    SpeciesProfile(std::string name, int size, int maxAge, int speed,
                   DietProfile diet, MovementProfile movement,
                   ReproductionProfile reproduction, CombatProfile combat);

    const std::string& name() const;
    int size() const;
    int maxAge() const;
    int speed() const;
    const DietProfile& diet() const;
    const MovementProfile& movement() const;
    const ReproductionProfile& reproduction() const;
    const CombatProfile& combat() const;

    bool validate() const;
    bool isPredator() const;
    bool isHerbivore() const;
    std::string summary() const;
    bool compatibleWith(const SpeciesProfile& other) const;
    int expectedLongevity() const;

private:
    std::string name_;
    int size_;
    int maxAge_;
    int speed_;
    DietProfile diet_;
    MovementProfile movement_;
    ReproductionProfile reproduction_;
    CombatProfile combat_;
};

/** @brief Registry of species profiles and their object factories. */
class SpeciesRegistry {
public:
    using Factory = std::function<std::shared_ptr<Animal>(const Position&, Sex)>;

    SpeciesRegistry();

    void registerSpecies(const SpeciesProfile& profile, Factory factory);
    std::shared_ptr<Animal> create(const std::string& name,
                                   const Position& position, Sex sex) const;
    bool hasSpecies(const std::string& name) const;
    int speciesCount() const;
    std::vector<std::string> names() const;
    SpeciesProfile profileOf(const std::string& name) const;

private:
    std::map<std::string, SpeciesProfile> profiles_;
    std::map<std::string, Factory> factories_;
};

/** @brief Weather and season of the simulated world. */
class Environment {
public:
    Environment();
    Environment(Season season, double temperature, double rainfall, int dayLength);

    Season season() const;
    double temperature() const;
    double rainfall() const;
    int dayLength() const;

    bool isHarsh() const;
    double plantGrowthFactor() const;
    int affectsHunger(int baseHunger) const;
    std::string describe() const;
    std::string seasonName() const;

private:
    Season season_;
    double temperature_;
    double rainfall_;
    int dayLength_;
};

/** @brief Terrain properties of the whole playing field. */
class Habitat {
public:
    Habitat();
    Habitat(std::string biome, double vegetationDensity, bool waterAccess,
            int temperature);

    const std::string& biome() const;
    double vegetationDensity() const;
    bool waterAccess() const;
    int temperature() const;

    double suitabilityFor(const SpeciesProfile& profile) const;
    int qualityRating() const;
    int plantCapacity() const;
    bool hazardLevelHigh() const;
    std::string describe() const;

private:
    std::string biome_;
    double vegetationDensity_;
    bool waterAccess_;
    int temperature_;
};

/** @brief Deterministic pseudo-random generator wrapper. */
class RandomSource {
public:
    RandomSource();
    explicit RandomSource(unsigned long seed);

    int nextInt(int low, int high);
    double nextDouble();
    bool chance(double probability);
    unsigned long seed() const;
    long calls() const;
    void reseed(unsigned long seed);

    template <typename T>
    int pickIndex(const std::vector<T>& items) {
        if (items.empty()) {
            return -1;
        }
        return nextInt(0, static_cast<int>(items.size()) - 1);
    }

    template <typename T>
    void shuffle(std::vector<T>& items) {
        for (std::size_t i = items.size(); i > 1; --i) {
            const int j = nextInt(0, static_cast<int>(i) - 1);
            std::swap(items[i - 1], items[static_cast<std::size_t>(j)]);
        }
    }

private:
    unsigned long seed_;
    std::mt19937 engine_;
    long calls_;
};

/** @brief Global parameters of a simulation run. */
class SimulationConfig {
public:
    SimulationConfig();
    SimulationConfig(int rows, int cols, int steps, double plantGrowthChance,
                     bool autoMode);

    int rows() const;
    int cols() const;
    int steps() const;
    double plantGrowthChance() const;
    bool autoMode() const;

    void setRows(int rows);
    void setCols(int cols);
    void setSteps(int steps);
    void setPlantGrowthChance(double chance);
    void setAutoMode(bool autoMode);

    void validate() const;
    bool isAutoMode() const;
    std::string describe() const;
    int totalCells() const;
    int recommendedPlants() const;

private:
    int rows_;
    int cols_;
    int steps_;
    double plantGrowthChance_;
    bool autoMode_;
};

/** @brief Counters collected while the simulation runs. */
class SimulationStatistics {
public:
    SimulationStatistics();

    void recordBirth();
    void recordDeath(int step);
    void recordMeal();
    void recordStep();
    void updatePeak(int population);
    double averageDeaths(int steps) const;
    std::string summary() const;
    void reset();

    int births() const;
    int deaths() const;
    int meals() const;
    int stepsElapsed() const;
    int peakPopulation() const;
    int lastDeathStep() const;

private:
    int births_;
    int deaths_;
    int meals_;
    int stepsElapsed_;
    int peakPopulation_;
    int lastDeathStep_;
};

/** @brief A single notable event of the simulation. */
class WorldEvent {
public:
    WorldEvent();
    WorldEvent(EventType type, std::string message, int step,
               Position position);

    EventType type() const;
    const std::string& message() const;
    int step() const;
    const Position& position() const;

    std::string describe() const;
    bool isCritical() const;
    bool isBirth() const;
    bool isDeath() const;
    std::string timestampLine() const;

private:
    EventType type_;
    std::string message_;
    int step_;
    Position position_;
};

/** @brief Bounded rolling log of world events. */
class EventLog {
public:
    EventLog();
    explicit EventLog(std::size_t capacity);

    void add(const WorldEvent& event);
    std::vector<WorldEvent> recent(std::size_t count) const;
    bool containsType(EventType type) const;
    std::size_t size() const;
    void clear();
    std::vector<std::string> exportLines() const;

private:
    std::vector<WorldEvent> events_;
    std::size_t capacity_;
};

} // namespace animalworld

#endif // ANIMALWORLD_CORE_H
