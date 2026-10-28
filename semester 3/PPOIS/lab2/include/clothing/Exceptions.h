#ifndef CLOTHING_EXCEPTIONS_H
#define CLOTHING_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/**
 * @file Exceptions.h
 * @brief Domain-specific exception hierarchy for the clothing model.
 */

namespace clothing {

/**
 * @brief Root of every exception thrown by the clothing domain.
 */
class ClothingException : public std::runtime_error {
public:
    explicit ClothingException(const std::string& message);
    virtual std::string errorCode() const;
};

/** @brief A size label, system, or body measurement is not valid. */
class InvalidSizeException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A price, discount, or tax value is out of its allowed range. */
class InvalidPriceException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A request tries to sell more units than are in stock. */
class OutOfStockException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief Two parts of an outfit cannot be combined for a domain reason. */
class IncompatibleMaterialException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A care label contains contradictory or impossible instructions. */
class CareInstructionException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A wardrobe cannot accept another garment because it is full. */
class WardrobeFullException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A garment with the requested identifier was not found. */
class GarmentNotFoundException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief An RGB color component or a color name is invalid. */
class InvalidColorException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief Two sizes use different systems and cannot be compared directly. */
class SizeMismatchException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A body measurement is negative, zero, or physically impossible. */
class InvalidMeasurementException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A wardrobe or collection already contains the same garment. */
class DuplicateGarmentException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A warehouse or inventory operation cannot remove the requested units. */
class InsufficientStockException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief A textual season name cannot be converted to a Season value. */
class InvalidSeasonException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

/** @brief The requested operation is not defined for the given object state. */
class UnsupportedOperationException : public ClothingException {
public:
    using ClothingException::ClothingException;
    std::string errorCode() const override;
};

} // namespace clothing

#endif // CLOTHING_EXCEPTIONS_H
