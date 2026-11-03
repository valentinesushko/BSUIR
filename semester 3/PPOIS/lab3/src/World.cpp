#include "animalworld/World.h"

#include "animalworld/Exceptions.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace animalworld {

FieldCell::FieldCell() : FieldCell(Position(0, 0), 4) {}

FieldCell::FieldCell(const Position& position, int maxOccupants)
    : position_(position), occupants_(), maxOccupants_(maxOccupants) {
    if (maxOccupants_ <= 0) {
        throw InvalidConfigurationException(
            "Cell capacity must be positive");
    }
}

const Position& FieldCell::position() const { return position_; }

bool FieldCell::add(const std::shared_ptr<Inhabitant>& inhabitant) {
    if (!inhabitant || !inhabitant->alive()) {
        throw InvalidConfigurationException(
            "Only living inhabitants can be placed");
    }
    if (std::dynamic_pointer_cast<Plant>(inhabitant) && hasPlant()) {
        throw DuplicatePlantException("A cell can contain only one plant");
    }
    if (isFull()) {
        throw CellOvercrowdedException("A cell can contain at most four inhabitants");
    }
    occupants_.push_back(inhabitant);
    return true;
}

bool FieldCell::remove(const std::shared_ptr<Inhabitant>& inhabitant) {
    const auto it = std::find_if(
        occupants_.begin(), occupants_.end(),
        [&inhabitant](const std::shared_ptr<Inhabitant>& current) {
            return current.get() == inhabitant.get();
        });
    if (it == occupants_.end()) {
        return false;
    }
    occupants_.erase(it);
    return true;
}

std::shared_ptr<Plant> FieldCell::plant() const {
    for (const auto& occupant : occupants_) {
        if (const auto plant = std::dynamic_pointer_cast<Plant>(occupant)) {
            return plant;
        }
    }
    return nullptr;
}

int FieldCell::animalCount() const {
    return static_cast<int>(std::count_if(
        occupants_.begin(), occupants_.end(),
        [](const std::shared_ptr<Inhabitant>& occupant) {
            return occupant->isAnimal() && occupant->alive();
        }));
}

bool FieldCell::isFull() const {
    return static_cast<int>(occupants_.size()) >= maxOccupants_;
}

int FieldCell::freeCapacity() const {
    return std::max(0, maxOccupants_ - static_cast<int>(occupants_.size()));
}

bool FieldCell::empty() const { return occupants_.empty(); }

bool FieldCell::refreshPlant() {
    const auto current = plant();
    if (!current) {
        return false;
    }
    current->regrow();
    return true;
}

std::vector<std::shared_ptr<Inhabitant>> FieldCell::inhabitants() const {
    return occupants_;
}

bool FieldCell::hasPlant() const { return plant() != nullptr; }

void FieldCell::removeDead() {
    occupants_.erase(
        std::remove_if(occupants_.begin(), occupants_.end(),
                       [](const std::shared_ptr<Inhabitant>& occupant) {
                           return !occupant->alive();
                       }),
        occupants_.end());
}

GameField::GameField() : GameField(FieldSize(8, 8)) {}

GameField::GameField(const FieldSize& size) : size_(size), cells_() {
    cells_.reserve(static_cast<std::size_t>(size_.area()));
    for (int row = 0; row < size_.rows(); ++row) {
        for (int col = 0; col < size_.cols(); ++col) {
            cells_.emplace_back(Position(row, col), 4);
        }
    }
}

FieldCell& GameField::cellAt(const Position& position) {
    if (!size_.isInside(position)) {
        throw InvalidPositionException("Cell " + position.toString() +
                                       " is outside the field");
    }
    const std::size_t index =
        static_cast<std::size_t>(position.row() * size_.cols() +
                                 position.col());
    return cells_.at(index);
}

const FieldCell& GameField::cellAt(const Position& position) const {
    if (!size_.isInside(position)) {
        throw InvalidPositionException("Cell " + position.toString() +
                                       " is outside the field");
    }
    const std::size_t index =
        static_cast<std::size_t>(position.row() * size_.cols() +
                                 position.col());
    return cells_.at(index);
}

bool GameField::canPlace(
    const Position& position,
    const std::shared_ptr<Inhabitant>& inhabitant) const {
    if (!inhabitant || !inhabitant->alive() || !size_.isInside(position)) {
        return false;
    }
    const FieldCell& cell = cellAt(position);
    if (std::dynamic_pointer_cast<Plant>(inhabitant) && cell.hasPlant()) {
        return false;
    }
    return !cell.isFull();
}

void GameField::add(const std::shared_ptr<Inhabitant>& inhabitant) {
    if (!inhabitant) {
        throw InvalidConfigurationException("Cannot add a null inhabitant");
    }
    if (!size_.isInside(inhabitant->position())) {
        throw InvalidPositionException("Inhabitant is outside the field");
    }
    cellAt(inhabitant->position()).add(inhabitant);
}

bool GameField::remove(const std::shared_ptr<Inhabitant>& inhabitant) {
    if (!inhabitant || !size_.isInside(inhabitant->position())) {
        return false;
    }
    return cellAt(inhabitant->position()).remove(inhabitant);
}

int GameField::plants() const {
    return static_cast<int>(std::count_if(
        cells_.begin(), cells_.end(),
        [](const FieldCell& cell) { return cell.hasPlant(); }));
}

int GameField::animals() const {
    int total = 0;
    for (const auto& cell : cells_) {
        total += cell.animalCount();
    }
    return total;
}

std::vector<std::shared_ptr<Inhabitant>> GameField::allInhabitants() const {
    std::vector<std::shared_ptr<Inhabitant>> result;
    for (const auto& cell : cells_) {
        const auto occupants = cell.inhabitants();
        result.insert(result.end(), occupants.begin(), occupants.end());
    }
    return result;
}

int GameField::countSpecies(const std::string& species) const {
    int total = 0;
    for (const auto& cell : cells_) {
        for (const auto& occupant : cell.inhabitants()) {
            if (occupant->alive() && occupant->species() == species) {
                ++total;
            }
        }
    }
    return total;
}

bool GameField::isInside(const Position& position) const {
    return size_.isInside(position);
}

int GameField::occupancy(const Position& position) const {
    return static_cast<int>(cellAt(position).inhabitants().size());
}

void GameField::clearDead() {
    for (auto& cell : cells_) {
        cell.removeDead();
    }
}

const FieldSize& GameField::size() const { return size_; }

WorldLoader::WorldLoader() : filename_(), registry_(nullptr), lineNumber_(0) {}

WorldLoader::WorldLoader(std::string filename,
                         const SpeciesRegistry& registry)
    : filename_(std::move(filename)), registry_(&registry), lineNumber_(0) {}

int WorldLoader::parseSize(const std::string& value) const {
    try {
        const int parsed = std::stoi(value);
        if (parsed <= 0) {
            throw InvalidConfigurationException(
                "Size values must be positive");
        }
        return parsed;
    } catch (const AnimalWorldException&) {
        throw;
    } catch (const std::exception&) {
        throw WorldFormatException("Cannot parse size '" + value + "' on line " +
                                   std::to_string(lineNumber_));
    }
}

int WorldLoader::parseCount(const std::string& value) const {
    try {
        const int parsed = std::stoi(value);
        if (parsed < 0) {
            throw WorldFormatException(
                "Counts must not be negative on line " +
                std::to_string(lineNumber_));
        }
        return parsed;
    } catch (const AnimalWorldException&) {
        throw;
    } catch (const std::exception&) {
        throw WorldFormatException("Cannot parse count '" + value + "' on line " +
                                   std::to_string(lineNumber_));
    }
}

std::shared_ptr<Animal> WorldLoader::spawnSpecies(const std::string& name,
                                                  const Position& position,
                                                  Sex sex) const {
    if (registry_ == nullptr) {
        throw SimulationStateException(
            "World loader has no species registry attached");
    }
    return registry_->create(name, position, sex);
}

SimulationConfig WorldLoader::parseLine(const std::string& line,
                                        SimulationConfig& config) const {
    std::istringstream stream(line);
    std::string key;
    std::string value;
    if (!(stream >> key)) {
        return config;
    }
    if (key.empty() || key[0] == '#') {
        return config;
    }
    if (key == "rows" || key == "height") {
        if (!(stream >> value)) {
            throw WorldFormatException("Missing rows value on line " +
                                       std::to_string(lineNumber_));
        }
        config.setRows(parseSize(value));
    } else if (key == "cols" || key == "width") {
        if (!(stream >> value)) {
            throw WorldFormatException("Missing cols value on line " +
                                       std::to_string(lineNumber_));
        }
        config.setCols(parseSize(value));
    } else if (key == "steps") {
        if (!(stream >> value)) {
            throw WorldFormatException("Missing steps value on line " +
                                       std::to_string(lineNumber_));
        }
        config.setSteps(parseCount(value));
    } else if (key == "growth") {
        if (!(stream >> value)) {
            throw WorldFormatException("Missing growth value on line " +
                                       std::to_string(lineNumber_));
        }
        try {
            config.setPlantGrowthChance(std::stod(value));
        } catch (const std::exception&) {
            throw WorldFormatException("Cannot parse growth '" + value +
                                       "' on line " +
                                       std::to_string(lineNumber_));
        }
    } else if (key == "mode") {
        if (!(stream >> value)) {
            throw WorldFormatException("Missing mode value on line " +
                                       std::to_string(lineNumber_));
        }
        if (value == "auto") {
            config.setAutoMode(true);
        } else if (value == "step") {
            config.setAutoMode(false);
        } else {
            throw InvalidModeException("Mode must be 'auto' or 'step'");
        }
    }
    return config;
}

SimulationConfig WorldLoader::readConfig() const {
    std::ifstream input(filename_);
    if (!input) {
        throw WorldFileException("Cannot open world file '" + filename_ + "'");
    }
    SimulationConfig config;
    std::string line;
    int number = 0;
    while (std::getline(input, line)) {
        ++number;
        lineNumber_ = number;
        const auto comment = line.find('#');
        if (comment != std::string::npos) {
            line = line.substr(0, comment);
        }
        const auto first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            continue;
        }
        parseLine(line, config);
    }
    config.validate();
    return config;
}

GameField WorldLoader::load(SimulationConfig& config,
                            RandomSource& random) const {
    std::ifstream input(filename_);
    if (!input) {
        throw WorldFileException("Cannot open world file '" + filename_ + "'");
    }

    config = SimulationConfig();
    std::map<std::string, int> counts;
    bool hasSeed = false;
    unsigned long seed = 0;
    std::string line;
    int number = 0;

    while (std::getline(input, line)) {
        ++number;
        lineNumber_ = number;
        const auto comment = line.find('#');
        if (comment != std::string::npos) {
            line = line.substr(0, comment);
        }
        std::istringstream stream(line);
        std::string key;
        std::string value;
        if (!(stream >> key)) {
            continue;
        }
        if (!(stream >> value)) {
            parseLine(line, config);
            continue;
        }

        if (key == "rows" || key == "height" || key == "cols" ||
            key == "width" || key == "steps" || key == "growth" ||
            key == "mode") {
            parseLine(line, config);
        } else if (key == "seed") {
            try {
                seed = static_cast<unsigned long>(std::stoul(value));
                hasSeed = true;
            } catch (const std::exception&) {
                throw WorldFormatException("Cannot parse seed '" + value +
                                           "' on line " +
                                           std::to_string(lineNumber_));
            }
        } else if (key == "plants" || key == "Plant") {
            counts["Plant"] = parseCount(value);
        } else if (registry_ != nullptr && registry_->hasSpecies(key)) {
            counts[key] = parseCount(value);
        } else {
            throw UnknownSpeciesException("Unknown species or key '" + key +
                                          "' on line " +
                                          std::to_string(lineNumber_));
        }
    }

    config.validate();
    if (hasSeed) {
        random.reseed(seed);
    }

    GameField field(FieldSize(config.rows(), config.cols()));
    buildField(field, counts, random);
    return field;
}

void WorldLoader::validate(const SimulationConfig& config) const {
    config.validate();
}

void WorldLoader::buildField(GameField& field,
                             const std::map<std::string, int>& counts,
                             RandomSource& random) const {
    const auto plantEntry = counts.find("Plant");
    const int plantCount =
        plantEntry == counts.end() ? 0 : plantEntry->second;

    for (int i = 0; i < plantCount; ++i) {
        for (int attempt = 0; attempt < 200; ++attempt) {
            const Position position = field.size().randomPosition(random);
            auto plant = std::make_shared<Plant>(
                position, random.nextInt(50, 100));
            if (field.canPlace(position, plant)) {
                field.add(plant);
                break;
            }
        }
    }

    for (const auto& entry : counts) {
        if (entry.first == "Plant") {
            continue;
        }
        for (int i = 0; i < entry.second; ++i) {
            bool placed = false;
            for (int attempt = 0; attempt < 400 && !placed; ++attempt) {
                const Position position = field.size().randomPosition(random);
                const Sex sex =
                    random.chance(0.5) ? Sex::Male : Sex::Female;
                auto animal = spawnSpecies(entry.first, position, sex);
                if (field.canPlace(position, animal)) {
                    field.add(animal);
                    placed = true;
                }
            }
        }
    }
}

ConsoleRenderer::ConsoleRenderer() : ConsoleRenderer('.', true) {}

ConsoleRenderer::ConsoleRenderer(char emptySymbol, bool showLegend)
    : emptySymbol_(emptySymbol), showLegend_(showLegend) {}

char ConsoleRenderer::cellSymbol(const FieldCell& cell) const {
    if (cell.empty()) {
        return emptySymbol_;
    }
    const bool hasPlant = cell.hasPlant();
    const int animals = cell.animalCount();
    if (hasPlant && animals > 0) {
        return '&';
    }
    if (hasPlant) {
        return '*';
    }
    if (animals == 1) {
        for (const auto& occupant : cell.inhabitants()) {
            if (occupant->isAnimal() && occupant->alive()) {
                return occupant->symbol();
            }
        }
    }
    if (animals > 1) {
        return '#';
    }
    return emptySymbol_;
}

std::string ConsoleRenderer::border(int cols) const {
    std::string line = "+";
    for (int i = 0; i < cols; ++i) {
        line += "--";
    }
    line += "+\n";
    return line;
}

std::string ConsoleRenderer::render(const GameField& field) const {
    std::ostringstream out;
    const int rows = field.size().rows();
    const int cols = field.size().cols();
    out << border(cols);
    for (int row = 0; row < rows; ++row) {
        out << '|';
        for (int col = 0; col < cols; ++col) {
            out << cellSymbol(field.cellAt(Position(row, col))) << ' ';
        }
        out << "|\n";
    }
    out << border(cols);
    out << "Plants: " << field.plants() << ", animals: " << field.animals()
        << '\n';
    if (showLegend_) {
        out << legend() << '\n';
    }
    return out.str();
}

std::string ConsoleRenderer::legend() const {
    return "Legend: * plant, &. plant+animal, # several animals, letters are "
           "species symbols, . empty";
}

std::string ConsoleRenderer::summary(const Simulation& simulation) const {
    std::ostringstream out;
    out << simulation.config().describe() << "\n"
        << simulation.statistics().summary() << "\n"
        << "Environment: " << simulation.environment().describe();
    return out.str();
}

} // namespace animalworld
