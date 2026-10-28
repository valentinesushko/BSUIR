#include "clothing/Exceptions.h"

namespace clothing {

ClothingException::ClothingException(const std::string& message)
    : std::runtime_error(message) {}

std::string ClothingException::errorCode() const {
    return "CLOTHING-GENERAL";
}

std::string InvalidSizeException::errorCode() const {
    return "CLOTHING-SIZE";
}

std::string InvalidPriceException::errorCode() const {
    return "CLOTHING-PRICE";
}

std::string OutOfStockException::errorCode() const {
    return "CLOTHING-STOCK";
}

std::string IncompatibleMaterialException::errorCode() const {
    return "CLOTHING-MATERIAL";
}

std::string CareInstructionException::errorCode() const {
    return "CLOTHING-CARE";
}

std::string WardrobeFullException::errorCode() const {
    return "CLOTHING-WARDROBE-FULL";
}

std::string GarmentNotFoundException::errorCode() const {
    return "CLOTHING-NOT-FOUND";
}

std::string InvalidColorException::errorCode() const {
    return "CLOTHING-COLOR";
}

std::string SizeMismatchException::errorCode() const {
    return "CLOTHING-SIZE-MISMATCH";
}

std::string InvalidMeasurementException::errorCode() const {
    return "CLOTHING-MEASUREMENT";
}

std::string DuplicateGarmentException::errorCode() const {
    return "CLOTHING-DUPLICATE";
}

std::string InsufficientStockException::errorCode() const {
    return "CLOTHING-STOCK-LOW";
}

std::string InvalidSeasonException::errorCode() const {
    return "CLOTHING-SEASON";
}

std::string UnsupportedOperationException::errorCode() const {
    return "CLOTHING-UNSUPPORTED";
}

} // namespace clothing
