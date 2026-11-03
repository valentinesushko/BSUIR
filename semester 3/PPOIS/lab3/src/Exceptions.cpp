#include "animalworld/Exceptions.h"

namespace animalworld {

AnimalWorldException::AnimalWorldException(const std::string& message)
    : std::runtime_error(message) {}

std::string AnimalWorldException::errorCode() const {
    return "AW-GENERAL";
}

std::string InvalidPositionException::errorCode() const {
    return "AW-POSITION";
}

std::string InvalidFieldSizeException::errorCode() const {
    return "AW-FIELD-SIZE";
}

std::string CellOvercrowdedException::errorCode() const {
    return "AW-CELL-FULL";
}

std::string DuplicatePlantException::errorCode() const {
    return "AW-PLANT-DUPLICATE";
}

std::string UnknownSpeciesException::errorCode() const {
    return "AW-SPECIES";
}

std::string WorldFileException::errorCode() const {
    return "AW-FILE";
}

std::string WorldFormatException::errorCode() const {
    return "AW-FORMAT";
}

std::string InvalidConfigurationException::errorCode() const {
    return "AW-CONFIG";
}

std::string InvalidVitalException::errorCode() const {
    return "AW-VITAL";
}

std::string ReproductionException::errorCode() const {
    return "AW-REPRODUCTION";
}

std::string InvalidDietException::errorCode() const {
    return "AW-DIET";
}

std::string InvalidGenomeException::errorCode() const {
    return "AW-GENOME";
}

std::string SimulationStateException::errorCode() const {
    return "AW-STATE";
}

std::string InvalidHabitatException::errorCode() const {
    return "AW-HABITAT";
}

std::string InvalidModeException::errorCode() const {
    return "AW-MODE";
}

} // namespace animalworld
