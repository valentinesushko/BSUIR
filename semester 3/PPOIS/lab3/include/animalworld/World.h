#ifndef ANIMALWORLD_WORLD_H
#define ANIMALWORLD_WORLD_H

#include "animalworld/Core.h"
#include "animalworld/Inhabitants.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * @file World.h
 * @brief The rectangular playing field, its loader, console renderer and the
 *        step-by-step simulation engine.
 */

namespace animalworld {

class Simulation;

/** @brief One square of the field; holds up to four inhabitants and one plant. */
class FieldCell {
public:
    FieldCell();
    explicit FieldCell(const Position& position, int maxOccupants = 4);

    const Position& position() const;
    bool add(const std::shared_ptr<Inhabitant>& inhabitant);
    bool remove(const std::shared_ptr<Inhabitant>& inhabitant);
    std::shared_ptr<Plant> plant() const;
    int animalCount() const;
    bool isFull() const;
    int freeCapacity() const;
    bool empty() const;
    bool refreshPlant();
    std::vector<std::shared_ptr<Inhabitant>> inhabitants() const;
    bool hasPlant() const;
    void removeDead();

private:
    Position position_;
    std::vector<std::shared_ptr<Inhabitant>> occupants_;
    int maxOccupants_;
};

/** @brief The rectangular board made of FieldCell objects. */
class GameField {
public:
    GameField();
    explicit GameField(const FieldSize& size);

    FieldCell& cellAt(const Position& position);
    const FieldCell& cellAt(const Position& position) const;
    bool canPlace(const Position& position,
                  const std::shared_ptr<Inhabitant>& inhabitant) const;
    void add(const std::shared_ptr<Inhabitant>& inhabitant);
    bool remove(const std::shared_ptr<Inhabitant>& inhabitant);
    int plants() const;
    int animals() const;
    std::vector<std::shared_ptr<Inhabitant>> allInhabitants() const;
    int countSpecies(const std::string& species) const;
    bool isInside(const Position& position) const;
    int occupancy(const Position& position) const;
    void clearDead();
    const FieldSize& size() const;

private:
    FieldSize size_;
    std::vector<FieldCell> cells_;
};

/** @brief Reads a world description from a text file. */
class WorldLoader {
public:
    WorldLoader();
    WorldLoader(std::string filename, const SpeciesRegistry& registry);

    GameField load(SimulationConfig& config, RandomSource& random) const;
    SimulationConfig parseLine(const std::string& line,
                               SimulationConfig& config) const;
    std::shared_ptr<Animal> spawnSpecies(const std::string& name,
                                         const Position& position,
                                         Sex sex) const;
    int parseSize(const std::string& value) const;
    int parseCount(const std::string& value) const;
    void validate(const SimulationConfig& config) const;
    SimulationConfig readConfig() const;
    void buildField(GameField& field,
                    const std::map<std::string, int>& counts,
                    RandomSource& random) const;

private:
    std::string filename_;
    const SpeciesRegistry* registry_;
    mutable int lineNumber_;
};

/** @brief Text-mode drawing of the field and its inhabitants. */
class ConsoleRenderer {
public:
    ConsoleRenderer();
    ConsoleRenderer(char emptySymbol, bool showLegend);

    std::string render(const GameField& field) const;
    std::string legend() const;
    char cellSymbol(const FieldCell& cell) const;
    std::string border(int cols) const;
    std::string summary(const Simulation& simulation) const;

private:
    char emptySymbol_;
    bool showLegend_;
};

/** @brief Drives the four phases of every simulation step. */
class Simulation {
public:
    Simulation();
    Simulation(GameField field, SimulationConfig config,
               unsigned long seed = 12345UL);

    void step();
    void run(int steps, bool interactive);
    void movementPhase();
    void feedingPhase();
    void reproductionPhase();
    void deathPhase();
    std::string snapshot() const;
    std::string summary() const;
    void reset(unsigned long seed);
    void addAnimal(const std::shared_ptr<Animal>& animal);
    int stepNumber() const;

    const GameField& field() const;
    GameField& field();
    const SimulationConfig& config() const;
    SimulationConfig& config();
    const SimulationStatistics& statistics() const;
    const EventLog& eventLog() const;
    const Environment& environment() const;

private:
    GameField field_;
    SimulationConfig config_;
    SimulationStatistics stats_;
    EventLog log_;
    RandomSource random_;
    Environment environment_;
    int stepNumber_;
};

} // namespace animalworld

#endif // ANIMALWORLD_WORLD_H
