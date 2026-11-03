#ifndef ANIMALWORLD_EXCEPTIONS_H
#define ANIMALWORLD_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/**
 * @file Exceptions.h
 * @brief Domain-specific exception hierarchy for the animal world model.
 */

namespace animalworld {

/** @brief Root of every exception thrown by the animal world domain. */
class AnimalWorldException : public std::runtime_error {
public:
    explicit AnimalWorldException(const std::string& message);
    virtual std::string errorCode() const;
};

/** @brief A board coordinate is outside the playable rectangle. */
class InvalidPositionException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A field has non-positive or otherwise unusable dimensions. */
class InvalidFieldSizeException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A cell already holds the maximum number of inhabitants. */
class CellOvercrowdedException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A second plant is placed into a cell that already has one. */
class DuplicatePlantException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A species name from the world file is not registered. */
class UnknownSpeciesException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief The world description file cannot be opened or read. */
class WorldFileException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A line or value in the world description has an invalid format. */
class WorldFormatException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief The simulation configuration is inconsistent. */
class InvalidConfigurationException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief Health, hunger, or another vital value is out of range. */
class InvalidVitalException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A reproduction request cannot be fulfilled. */
class ReproductionException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A diet description is inconsistent with its diet type. */
class InvalidDietException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A genome value is outside the allowed coefficient range. */
class InvalidGenomeException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A simulation operation is illegal in the current state. */
class SimulationStateException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A habitat description contains impossible parameters. */
class InvalidHabitatException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

/** @brief A command-line argument cannot be parsed. */
class InvalidModeException : public AnimalWorldException {
public:
    using AnimalWorldException::AnimalWorldException;
    std::string errorCode() const override;
};

} // namespace animalworld

#endif // ANIMALWORLD_EXCEPTIONS_H
