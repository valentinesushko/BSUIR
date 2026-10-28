#include "clothing/Clothing.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace clothing {

namespace {

double clampValue(double value, double low, double high) {
    return std::max(low, std::min(high, value));
}

std::string toLowerCopy(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

} // namespace

Sportswear::Sportswear(ActivityType activity, bool moistureWicking,
                       double stretchFactor)
    : activity_(activity),
      moistureWicking_(moistureWicking),
      stretchFactor_(stretchFactor) {}

ActivityType Sportswear::activity() const { return activity_; }
bool Sportswear::moistureWicking() const { return moistureWicking_; }
double Sportswear::stretchFactor() const { return stretchFactor_; }

bool Sportswear::isHighImpact() const {
    const bool explosive = activity_ == ActivityType::Running ||
                           activity_ == ActivityType::Hiking ||
                           activity_ == ActivityType::Cycling;
    return explosive && stretchFactor_ > 10.0;
}

double Sportswear::sweatManagementScore() const {
    const double base = moistureWicking_ ? 7.0 : 3.0;
    return clampValue(base + stretchFactor_ / 20.0, 0.0, 10.0);
}

double Sportswear::flexibilityRating() const {
    return clampValue(stretchFactor_ / 5.0, 0.0, 10.0);
}

std::string Sportswear::describeSport() const {
    std::ostringstream out;
    out << toString(activity_) << " sportswear, "
        << (moistureWicking_ ? "moisture wicking" : "not moisture wicking")
        << ", stretch " << stretchFactor_ << "%";
    return out.str();
}

SportJacket::SportJacket(const GarmentSpec& spec, ActivityType activity,
                         bool moistureWicking, double stretchFactor,
                         int reflectiveStrips, bool packable)
    : Jacket(spec, "sport", 3, false),
      Sportswear(activity, moistureWicking, stretchFactor),
      reflectiveStrips_(reflectiveStrips),
      packable_(packable) {}

int SportJacket::reflectiveStrips() const { return reflectiveStrips_; }
bool SportJacket::isPackable() const { return packable_; }

double SportJacket::visibilityScore() const {
    return clampValue(3.0 + reflectiveStrips_ * 2.0, 0.0, 10.0);
}

double SportJacket::activitySuitability() const {
    double score = isHighImpact() ? 7.0 : 5.0;
    if (isPackable()) score += 1.0;
    if (hasHood()) score += 1.0;
    return clampValue(score, 0.0, 10.0);
}

std::string SportJacket::describe() const {
    return Jacket::describe() + ", " + describeSport() + ", reflective strips " +
           std::to_string(reflectiveStrips_);
}

SportSneaker::SportSneaker(const GarmentSpec& spec, ActivityType activity,
                           bool carbonPlate, std::string terrainType)
    : Sneaker(spec, "foam", "lugged", 6.0),
      Sportswear(activity, true, 25.0),
      carbonPlate_(carbonPlate),
      terrainType_(std::move(terrainType)) {}

bool SportSneaker::carbonPlate() const { return carbonPlate_; }
const std::string& SportSneaker::terrainType() const { return terrainType_; }

double SportSneaker::raceReadiness() const {
    double score = runningScore();
    if (carbonPlate_) score += 1.5;
    if (toLowerCopy(terrainType_) == "road") score += 0.5;
    return clampValue(score, 0.0, 10.0);
}

std::string SportSneaker::sportProfile() const {
    return describeSport() + ", " + terrainType_ + " terrain" +
           (carbonPlate_ ? ", carbon plate" : ", no plate");
}

double SportSneaker::energyReturn() const {
    return clampValue((carbonPlate_ ? 8.0 : 5.0) + flexibilityRating() / 5.0, 0.0, 10.0);
}

std::string SportSneaker::describe() const {
    return Sneaker::describe() + ", " + sportProfile();
}

} // namespace clothing
