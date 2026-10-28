#include "clothing/Clothing.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace clothing {

namespace {

double clampValue(double value, double low, double high) {
    return std::max(low, std::min(high, value));
}

int clampChannel(int value) {
    return std::max(0, std::min(255, value));
}

double channelRatio(int value) {
    return static_cast<double>(clampChannel(value)) / 255.0;
}

} // namespace

namespace garment_care {

int recommendedTemperature(textile::Fiber fiber) {
    switch (fiber) {
    case textile::Fiber::Cotton:
    case textile::Fiber::Linen:
    case textile::Fiber::Denim:
        return 40;
    case textile::Fiber::Wool:
        return 30;
    case textile::Fiber::Silk:
        return 30;
    case textile::Fiber::Synthetic:
        return 30;
    case textile::Fiber::Leather:
        return 0;
    }
    return 30;
}

std::string cycleName(int temperature) {
    if (temperature <= 0) {
        return "specialist dry clean";
    }
    if (temperature <= 30) {
        return "delicate cycle";
    }
    if (temperature <= 40) {
        return "normal cycle";
    }
    if (temperature <= 60) {
        return "intensive cycle";
    }
    return "boil wash";
}

} // namespace garment_care

std::string toString(Season value) {
    switch (value) {
    case Season::Spring: return "Spring";
    case Season::Summer: return "Summer";
    case Season::Autumn: return "Autumn";
    case Season::Winter: return "Winter";
    case Season::AllSeason: return "AllSeason";
    }
    return "Unknown";
}

std::string toString(textile::Fiber value) {
    switch (value) {
    case textile::Fiber::Cotton: return "Cotton";
    case textile::Fiber::Wool: return "Wool";
    case textile::Fiber::Silk: return "Silk";
    case textile::Fiber::Denim: return "Denim";
    case textile::Fiber::Linen: return "Linen";
    case textile::Fiber::Synthetic: return "Synthetic";
    case textile::Fiber::Leather: return "Leather";
    }
    return "Unknown";
}

std::string toString(SizeSystem value) {
    switch (value) {
    case SizeSystem::EU: return "EU";
    case SizeSystem::US: return "US";
    case SizeSystem::UK: return "UK";
    case SizeSystem::International: return "International";
    }
    return "Unknown";
}

std::string toString(MeasurementType value) {
    switch (value) {
    case MeasurementType::Chest: return "chest";
    case MeasurementType::Waist: return "waist";
    case MeasurementType::Hip: return "hip";
    case MeasurementType::Inseam: return "inseam";
    case MeasurementType::Length: return "length";
    }
    return "unknown";
}

std::string toString(WeaveType value) {
    switch (value) {
    case WeaveType::Plain: return "plain";
    case WeaveType::Twill: return "twill";
    case WeaveType::Satin: return "satin";
    case WeaveType::Pique: return "pique";
    case WeaveType::Rib: return "rib";
    }
    return "unknown";
}

std::string toString(LeatherType value) {
    switch (value) {
    case LeatherType::Cow: return "cow";
    case LeatherType::Sheep: return "sheep";
    case LeatherType::Goat: return "goat";
    case LeatherType::Pig: return "pig";
    }
    return "unknown";
}

std::string toString(PolymerType value) {
    switch (value) {
    case PolymerType::Polyester: return "polyester";
    case PolymerType::Nylon: return "nylon";
    case PolymerType::Elastane: return "elastane";
    case PolymerType::Acrylic: return "acrylic";
    }
    return "unknown";
}

std::string toString(Currency value) {
    switch (value) {
    case Currency::USD: return "USD";
    case Currency::EUR: return "EUR";
    case Currency::BYN: return "BYN";
    }
    return "???";
}

std::string toString(ActivityType value) {
    switch (value) {
    case ActivityType::Running: return "running";
    case ActivityType::Yoga: return "yoga";
    case ActivityType::Hiking: return "hiking";
    case ActivityType::Cycling: return "cycling";
    case ActivityType::Gym: return "gym";
    }
    return "unknown";
}

Measurement::Measurement() : centimeters_(0.0), type_(MeasurementType::Chest) {}

Measurement::Measurement(double centimeters, MeasurementType type)
    : centimeters_(centimeters), type_(type) {
    validate();
}

double Measurement::centimeters() const {
    return centimeters_;
}

void Measurement::setCentimeters(double centimeters) {
    centimeters_ = centimeters;
    validate();
}

MeasurementType Measurement::type() const {
    return type_;
}

double Measurement::toInches() const {
    return centimeters_ / 2.54;
}

Measurement Measurement::add(const Measurement& other) const {
    if (type_ != other.type_) {
        throw InvalidMeasurementException("Cannot add measurements of different types");
    }
    return Measurement(centimeters_ + other.centimeters_, type_);
}

bool Measurement::isWithinRange(double minCm, double maxCm) const {
    return centimeters_ >= minCm && centimeters_ <= maxCm;
}

double Measurement::ratioTo(const Measurement& other) const {
    if (other.centimeters_ <= 0.0) {
        throw InvalidMeasurementException("Cannot compute ratio to a non-positive measurement");
    }
    return centimeters_ / other.centimeters_;
}

void Measurement::validate() const {
    if (centimeters_ <= 0.0 || centimeters_ > 300.0) {
        throw InvalidMeasurementException("Measurement must be in (0, 300] cm");
    }
}

Color::Color() : red_(0), green_(0), blue_(0), name_("Black") {}

Color::Color(int red, int green, int blue, std::string name)
    : red_(red), green_(green), blue_(blue), name_(std::move(name)) {
    validate();
}

int Color::red() const { return red_; }
int Color::green() const { return green_; }
int Color::blue() const { return blue_; }
const std::string& Color::name() const { return name_; }

std::string Color::toHex() const {
    std::ostringstream out;
    out << '#' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << red_
        << std::setw(2) << green_ << std::setw(2) << blue_;
    return out.str();
}

Color Color::mix(const Color& other, double ratio) const {
    const double r = clampValue(ratio, 0.0, 1.0);
    const int red = static_cast<int>(std::round(red_ + (other.red_ - red_) * r));
    const int green = static_cast<int>(std::round(green_ + (other.green_ - green_) * r));
    const int blue = static_cast<int>(std::round(blue_ + (other.blue_ - blue_) * r));
    return Color(clampChannel(red), clampChannel(green), clampChannel(blue),
                 name_ + "-" + other.name_);
}

Color Color::lighten(double factor) const {
    return mix(Color(255, 255, 255, "White"), clampValue(factor, 0.0, 1.0));
}

Color Color::darken(double factor) const {
    return mix(Color(0, 0, 0, "Black"), clampValue(factor, 0.0, 1.0));
}

double Color::contrastWith(const Color& other) const {
    const auto luminance = [](const Color& color) {
        return 0.2126 * channelRatio(color.red_) + 0.7152 * channelRatio(color.green_) +
               0.0722 * channelRatio(color.blue_);
    };
    return std::abs(luminance(*this) - luminance(other));
}

bool Color::isNeutral() const {
    const int maximum = std::max({red_, green_, blue_});
    const int minimum = std::min({red_, green_, blue_});
    return maximum - minimum < 20;
}

void Color::validate() const {
    const auto valid = [](int component) { return component >= 0 && component <= 255; };
    if (!valid(red_) || !valid(green_) || !valid(blue_)) {
        throw InvalidColorException("RGB components must be in [0, 255]");
    }
    if (name_.empty()) {
        throw InvalidColorException("Color name must not be empty");
    }
}

bool Color::operator==(const Color& other) const {
    return red_ == other.red_ && green_ == other.green_ && blue_ == other.blue_;
}

bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

Size::Size()
    : label_("M"),
      system_(SizeSystem::EU),
      chest_(96.0, MeasurementType::Chest),
      waist_(80.0, MeasurementType::Waist),
      hip_(100.0, MeasurementType::Hip) {}

Size::Size(std::string label, SizeSystem system, Measurement chest,
           Measurement waist, Measurement hip)
    : label_(std::move(label)), system_(system), chest_(chest), waist_(waist), hip_(hip) {
    validate();
}

const std::string& Size::label() const { return label_; }
SizeSystem Size::system() const { return system_; }
const Measurement& Size::chest() const { return chest_; }
const Measurement& Size::waist() const { return waist_; }
const Measurement& Size::hip() const { return hip_; }

bool Size::fits(const Measurement& bodyChest) const {
    if (bodyChest.type() != MeasurementType::Chest) {
        throw InvalidSizeException("Size::fits expects a chest measurement");
    }
    return std::abs(chest_.centimeters() - bodyChest.centimeters()) <= 4.0;
}

bool Size::isCompatibleWith(const Size& other) const {
    if (system_ != other.system_) {
        return false;
    }
    return std::abs(waist_.centimeters() - other.waist_.centimeters()) <= 6.0 &&
           std::abs(hip_.centimeters() - other.hip_.centimeters()) <= 6.0;
}

Size Size::convertedTo(SizeSystem system) const {
    if (system == system_) {
        return *this;
    }
    const int base = numericLabel();
    int converted = base;
    switch (system) {
    case SizeSystem::EU:
        converted = base + (system_ == SizeSystem::US ? 32 : 30);
        break;
    case SizeSystem::US:
        converted = base - (system_ == SizeSystem::EU ? 32 : 0);
        break;
    case SizeSystem::UK:
        converted = base - 2;
        break;
    case SizeSystem::International:
        converted = base;
        break;
    }
    return Size(std::to_string(std::max(0, converted)), system, chest_, waist_, hip_);
}

int Size::numericLabel() const {
    try {
        return std::stoi(label_);
    } catch (const std::exception&) {
        if (label_ == "XS") return 44;
        if (label_ == "S") return 46;
        if (label_ == "M") return 48;
        if (label_ == "L") return 50;
        if (label_ == "XL") return 52;
        if (label_ == "XXL") return 54;
        return 48;
    }
}

void Size::validate() const {
    if (label_.empty()) {
        throw InvalidSizeException("Size label must not be empty");
    }
    if (chest_.centimeters() <= waist_.centimeters()) {
        throw InvalidSizeException("Chest must be larger than waist");
    }
    if (hip_.centimeters() <= waist_.centimeters()) {
        throw InvalidSizeException("Hip must be larger than waist");
    }
}

void Size::ensureSameSystem(const Size& other) const {
    if (system_ != other.system_) {
        throw SizeMismatchException("Sizes use different sizing systems");
    }
}

CareInstructions::CareInstructions()
    : washTemperature_(30),
      canTumbleDry_(true),
      canBleach_(false),
      ironTemperature_(110),
      dryCleanOnly_(false) {
    validate();
}

CareInstructions::CareInstructions(int washTemperature, bool canTumbleDry,
                                   bool canBleach, int ironTemperature,
                                   bool dryCleanOnly)
    : washTemperature_(washTemperature),
      canTumbleDry_(canTumbleDry),
      canBleach_(canBleach),
      ironTemperature_(ironTemperature),
      dryCleanOnly_(dryCleanOnly) {
    validate();
}

int CareInstructions::washTemperature() const { return washTemperature_; }
bool CareInstructions::canTumbleDry() const { return canTumbleDry_; }
bool CareInstructions::canBleach() const { return canBleach_; }
int CareInstructions::ironTemperature() const { return ironTemperature_; }
bool CareInstructions::dryCleanOnly() const { return dryCleanOnly_; }

CareInstructions CareInstructions::merge(const CareInstructions& other) const {
    return CareInstructions(std::min(washTemperature_, other.washTemperature_),
                            canTumbleDry_ && other.canTumbleDry_,
                            canBleach_ && other.canBleach_,
                            std::min(ironTemperature_, other.ironTemperature_),
                            dryCleanOnly_ || other.dryCleanOnly_);
}

std::string CareInstructions::recommendedCycle() const {
    using namespace garment_care;
    if (dryCleanOnly_) {
        return "dry clean only";
    }
    return cycleName(washTemperature_);
}

bool CareInstructions::isSafeFor(const Material& material) const {
    using namespace garment_care;
    if (dryCleanOnly_) {
        return material.requiresDryCleaning();
    }
    if (material.requiresDryCleaning()) {
        return false;
    }
    return washTemperature_ <= recommendedTemperature(material.fiber());
}

std::string CareInstructions::summary() const {
    std::ostringstream out;
    out << (dryCleanOnly_ ? "dry clean" : std::to_string(washTemperature_) + "C wash")
        << ", " << (canTumbleDry_ ? "tumble dry" : "line dry")
        << ", " << (canBleach_ ? "bleach allowed" : "no bleach")
        << ", iron at " << ironTemperature_ << "C";
    return out.str();
}

bool CareInstructions::isGentle() const {
    return washTemperature_ <= 30 && ironTemperature_ <= 110 && !canBleach_;
}

void CareInstructions::validate() const {
    if (washTemperature_ < 0 || washTemperature_ > 95) {
        throw CareInstructionException("Wash temperature must be in [0, 95]");
    }
    if (ironTemperature_ < 0 || ironTemperature_ > 250) {
        throw CareInstructionException("Iron temperature must be in [0, 250]");
    }
    if (dryCleanOnly_ && canTumbleDry_) {
        throw CareInstructionException("Dry-clean-only garments cannot be tumble dried");
    }
}

Manufacturer::Manufacturer()
    : name_("Unknown"), country_("Belarus"), contactEmail_("info@example.com"),
      foundedYear_(2000), ethicalRating_(5.0) {}

Manufacturer::Manufacturer(std::string name, std::string country,
                           std::string contactEmail, int foundedYear,
                           double ethicalRating)
    : name_(std::move(name)),
      country_(std::move(country)),
      contactEmail_(std::move(contactEmail)),
      foundedYear_(foundedYear),
      ethicalRating_(ethicalRating) {}

const std::string& Manufacturer::name() const { return name_; }
const std::string& Manufacturer::country() const { return country_; }
const std::string& Manufacturer::contactEmail() const { return contactEmail_; }
int Manufacturer::foundedYear() const { return foundedYear_; }
double Manufacturer::ethicalRating() const { return ethicalRating_; }

bool Manufacturer::isEuropean() const {
    static const std::vector<std::string> countries = {
        "Belarus", "Italy", "France", "Germany", "Poland", "Spain",
        "Portugal", "Lithuania", "Latvia", "Estonia", "Czech Republic"};
    return std::find(countries.begin(), countries.end(), country_) != countries.end();
}

double Manufacturer::reputationScore() const {
    const int age = yearsInBusiness(2026);
    return clampValue(ethicalRating_ * 2.0 + std::min(10, age / 5), 0.0, 100.0);
}

std::string Manufacturer::contactInfo() const {
    return name_ + " (" + country_ + "): " + contactEmail_;
}

int Manufacturer::yearsInBusiness(int currentYear) const {
    if (currentYear <= foundedYear_) {
        return 0;
    }
    return currentYear - foundedYear_;
}

PriceTag::PriceTag()
    : amount_(0.0), currency_(Currency::BYN), discountPercent_(0.0), taxPercent_(0.0) {}

PriceTag::PriceTag(double amount, Currency currency, double discountPercent,
                   double taxPercent)
    : amount_(amount), currency_(currency), discountPercent_(discountPercent),
      taxPercent_(taxPercent) {
    validate();
}

double PriceTag::amount() const { return amount_; }
Currency PriceTag::currency() const { return currency_; }
double PriceTag::discountPercent() const { return discountPercent_; }
double PriceTag::taxPercent() const { return taxPercent_; }

double PriceTag::finalPrice() const {
    return amount_ * (1.0 - discountPercent_ / 100.0) * (1.0 + taxPercent_ / 100.0);
}

PriceTag PriceTag::applyDiscount(double percent) const {
    if (percent < 0.0 || percent > 100.0) {
        throw InvalidPriceException("Discount percent must be in [0, 100]");
    }
    return PriceTag(amount_, currency_, clampValue(discountPercent_ + percent, 0.0, 100.0),
                    taxPercent_);
}

PriceTag PriceTag::convertTo(Currency target, double rate) const {
    if (target == currency_) {
        return *this;
    }
    if (rate <= 0.0) {
        throw InvalidPriceException("Currency conversion rate must be positive");
    }
    return PriceTag(amount_ * rate, target, discountPercent_, taxPercent_);
}

bool PriceTag::isAffordable(double budget) const {
    return finalPrice() <= budget;
}

double PriceTag::taxAmount() const {
    return amount_ * (1.0 - discountPercent_ / 100.0) * (taxPercent_ / 100.0);
}

void PriceTag::validate() const {
    if (amount_ < 0.0) {
        throw InvalidPriceException("Price amount must not be negative");
    }
    if (discountPercent_ < 0.0 || discountPercent_ > 100.0) {
        throw InvalidPriceException("Discount percent must be in [0, 100]");
    }
    if (taxPercent_ < 0.0 || taxPercent_ > 100.0) {
        throw InvalidPriceException("Tax percent must be in [0, 100]");
    }
}

Identifiable::Identifiable() : id_(), createdAt_(0) {
    regenerateId();
}

Identifiable::Identifiable(std::string id, long createdAt)
    : id_(std::move(id)), createdAt_(createdAt) {}

const std::string& Identifiable::id() const { return id_; }
long Identifiable::createdAt() const { return createdAt_; }

void Identifiable::regenerateId() {
    static long counter = 0;
    ++counter;
    createdAt_ = counter;
    id_ = "G-" + std::to_string(100000 + counter);
}

bool Identifiable::isSame(const Identifiable& other) const {
    return id_ == other.id_;
}

std::string Identifiable::identitySummary() const {
    return id_ + " created at sequence " + std::to_string(createdAt_);
}

} // namespace clothing
