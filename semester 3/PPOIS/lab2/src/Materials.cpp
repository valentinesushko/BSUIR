#include "clothing/Clothing.h"

#include <algorithm>
#include <sstream>

namespace clothing {

namespace {

double clampValue(double value, double low, double high) {
    return std::max(low, std::min(high, value));
}

} // namespace

Material::Material(std::string name, textile::Fiber fiber, double weightGsm,
                   double breathability, double waterproofRating)
    : name_(std::move(name)),
      fiber_(fiber),
      weightGsm_(weightGsm),
      breathability_(breathability),
      waterproofRating_(waterproofRating) {}

const std::string& Material::name() const { return name_; }
textile::Fiber Material::fiber() const { return fiber_; }
double Material::weightGsm() const { return weightGsm_; }
double Material::breathability() const { return breathability_; }
double Material::waterproofRating() const { return waterproofRating_; }

bool Material::requiresDryCleaning() const {
    return false;
}

std::string Material::describe() const {
    std::ostringstream out;
    out << name_ << " (" << toString(fiber_) << ", " << weightGsm_ << " gsm)";
    return out.str();
}

double Material::warmthIndex() const {
    return clampValue(weightGsm_ / 100.0 + breathability_ * 1.5, 0.0, 10.0);
}

bool Material::isLightweight() const {
    return weightGsm_ < 150.0;
}

bool Material::isWaterproof() const {
    return waterproofRating_ >= 7.0;
}

Fabric::Fabric(std::string name, textile::Fiber fiber, double weightGsm,
               double breathability, double waterproofRating, WeaveType weave,
               double widthCm)
    : Material(std::move(name), fiber, weightGsm, breathability, waterproofRating),
      weave_(weave),
      widthCm_(widthCm) {}

WeaveType Fabric::weave() const { return weave_; }
double Fabric::widthCm() const { return widthCm_; }

double Fabric::areaWeightFor(double meters) const {
    if (meters <= 0.0) {
        return 0.0;
    }
    return weightGsm_ * (widthCm_ / 100.0) * meters / 1000.0;
}

bool Fabric::canBeDyed() const {
    return weave_ != WeaveType::Satin;
}

double Fabric::shrinkagePercent() const {
    switch (fiber_) {
    case textile::Fiber::Cotton: return 5.0;
    case textile::Fiber::Wool: return 8.0;
    case textile::Fiber::Silk: return 3.0;
    case textile::Fiber::Denim: return 4.0;
    case textile::Fiber::Linen: return 6.0;
    case textile::Fiber::Synthetic: return 1.0;
    case textile::Fiber::Leather: return 0.0;
    }
    return 2.0;
}

bool Fabric::isWashable() const {
    return recommendedWashTemperature() > 0.0;
}

double Fabric::recommendedWashTemperature() const {
    return weightGsm_ > 250.0 ? 30.0 : 40.0;
}

bool Fabric::suitableFor(Season season) const {
    switch (season) {
    case Season::Winter: return weightGsm_ >= 250.0;
    case Season::Summer: return weightGsm_ <= 150.0;
    case Season::Spring:
    case Season::Autumn: return weightGsm_ >= 120.0 && weightGsm_ <= 300.0;
    case Season::AllSeason: return true;
    }
    return true;
}

double Fabric::stretchPercent() const {
    return weave_ == WeaveType::Rib ? 10.0 : 0.0;
}

std::string Fabric::describe() const {
    std::ostringstream out;
    out << Material::describe() << ", " << toString(weave_) << " weave, "
        << widthCm_ << " cm wide";
    return out.str();
}

KnitFabric::KnitFabric(std::string name, textile::Fiber fiber, double weightGsm,
                       double breathability, double waterproofRating,
                       WeaveType weave, double widthCm, int stitchDensity,
                       double elasticity)
    : Fabric(std::move(name), fiber, weightGsm, breathability, waterproofRating,
             weave, widthCm),
      stitchDensity_(stitchDensity),
      elasticity_(elasticity) {}

double KnitFabric::stretchPercent() const {
    return elasticity_;
}

double KnitFabric::stretchesTo(double lengthCm) const {
    return lengthCm * (1.0 + elasticity_ / 100.0);
}

bool KnitFabric::recoversAfterWash() const {
    return elasticity_ > 10.0;
}

int KnitFabric::recommendNeedleSize() const {
    return std::max(60, 120 - stitchDensity_);
}

WovenFabric::WovenFabric(std::string name, textile::Fiber fiber, double weightGsm,
                         double breathability, double waterproofRating,
                         WeaveType weave, double widthCm, int threadCount,
                         bool warpAligned)
    : Fabric(std::move(name), fiber, weightGsm, breathability, waterproofRating,
             weave, widthCm),
      threadCount_(threadCount),
      warpAligned_(warpAligned) {}

double WovenFabric::densityRating() const {
    return threadCount_ / 100.0;
}

bool WovenFabric::isFrayResistant() const {
    return threadCount_ >= 120 && warpAligned_;
}

int WovenFabric::recommendedIronTemperature() const {
    switch (fiber_) {
    case textile::Fiber::Cotton:
    case textile::Fiber::Denim:
    case textile::Fiber::Linen:
        return 200;
    case textile::Fiber::Wool:
    case textile::Fiber::Silk:
        return 110;
    case textile::Fiber::Synthetic:
        return 150;
    case textile::Fiber::Leather:
        return 0;
    }
    return 150;
}

double WovenFabric::shrinkagePercent() const {
    return warpAligned_ ? std::max(0.0, Fabric::shrinkagePercent() - 0.5)
                        : Fabric::shrinkagePercent() + 0.5;
}

CottonFabric::CottonFabric(std::string name, double weightGsm, double breathability,
                           double waterproofRating, WeaveType weave, double widthCm,
                           int threadCount, bool warpAligned, bool organic,
                           std::string originRegion)
    : WovenFabric(std::move(name), textile::Fiber::Cotton, weightGsm, breathability,
                  waterproofRating, weave, widthCm, threadCount, warpAligned),
      organic_(organic),
      originRegion_(std::move(originRegion)) {}

bool CottonFabric::isOrganic() const { return organic_; }
const std::string& CottonFabric::originRegion() const { return originRegion_; }

double CottonFabric::absorbencyRate() const {
    return clampValue(0.35 + breathability_ * 0.05, 0.0, 1.0);
}

bool CottonFabric::isHypoallergenic() const {
    return organic_ && originRegion_ != "unknown";
}

double CottonFabric::dyeAffinity() const {
    return clampValue(0.75 + (organic_ ? 0.1 : 0.0), 0.0, 1.0);
}

bool CottonFabric::suitableFor(Season season) const {
    if (season == Season::Summer) {
        return weightGsm_ <= 200.0;
    }
    return Fabric::suitableFor(season);
}

double CottonFabric::recommendedWashTemperature() const {
    return organic_ ? 40.0 : 60.0;
}

WoolFabric::WoolFabric(std::string name, double weightGsm, double breathability,
                       double waterproofRating, WeaveType weave, double widthCm,
                       int stitchDensity, double elasticity, int micronCount,
                       bool merino)
    : KnitFabric(std::move(name), textile::Fiber::Wool, weightGsm, breathability,
                 waterproofRating, weave, widthCm, stitchDensity, elasticity),
      micronCount_(micronCount),
      merino_(merino) {}

int WoolFabric::micronCount() const { return micronCount_; }
bool WoolFabric::isMerino() const { return merino_; }

double WoolFabric::warmthRating() const {
    return clampValue((merino_ ? 7.0 : 6.0) + weightGsm_ / 200.0, 0.0, 10.0);
}

bool WoolFabric::feltsInWash() const {
    return !merino_ || recommendedWashTemperature() > 30.0;
}

double WoolFabric::allergenRisk() const {
    return micronCount_ < 19 ? 0.1 : 0.6;
}

double WoolFabric::recommendedWashTemperature() const {
    return merino_ ? 30.0 : 40.0;
}

bool WoolFabric::requiresDryCleaning() const {
    return !merino_ && micronCount_ > 22;
}

bool WoolFabric::suitableFor(Season season) const {
    return season == Season::Autumn || season == Season::Winter ||
           season == Season::AllSeason;
}

SilkFabric::SilkFabric(std::string name, double weightGsm, double breathability,
                       double waterproofRating, WeaveType weave, double widthCm,
                       int threadCount, bool warpAligned, double momme,
                       double sheen)
    : WovenFabric(std::move(name), textile::Fiber::Silk, weightGsm, breathability,
                  waterproofRating, weave, widthCm, threadCount, warpAligned),
      momme_(momme),
      sheen_(sheen) {}

double SilkFabric::momme() const { return momme_; }
double SilkFabric::sheen() const { return sheen_; }

double SilkFabric::lusterScore() const {
    return clampValue(sheen_ * 10.0, 0.0, 10.0);
}

bool SilkFabric::needsDelicateWash() const {
    return true;
}

bool SilkFabric::drapesWell() const {
    return momme_ <= 19.0;
}

double SilkFabric::recommendedWashTemperature() const {
    return 30.0;
}

bool SilkFabric::requiresDryCleaning() const {
    return momme_ > 22.0;
}

bool SilkFabric::suitableFor(Season season) const {
    if (season == Season::Summer || season == Season::Spring) {
        return weightGsm_ <= 120.0;
    }
    return true;
}

DenimFabric::DenimFabric(std::string name, double weightGsm, double breathability,
                         double waterproofRating, WeaveType weave, double widthCm,
                         int threadCount, bool warpAligned, double ounceWeight,
                         double stretchLimit)
    : WovenFabric(std::move(name), textile::Fiber::Denim, weightGsm, breathability,
                  waterproofRating, weave, widthCm, threadCount, warpAligned),
      ounceWeight_(ounceWeight),
      stretchLimit_(stretchLimit) {}

double DenimFabric::ounceWeight() const { return ounceWeight_; }
double DenimFabric::stretchLimit() const { return stretchLimit_; }

double DenimFabric::fadeAfterWashes(int washes) const {
    return clampValue(washes * 1.2, 0.0, 100.0);
}

bool DenimFabric::isRigid() const {
    return stretchLimit_ < 3.0 && ounceWeight_ > 12.0;
}

double DenimFabric::recommendSoakHours() const {
    return ounceWeight_ > 14.0 ? 2.0 : 1.0;
}

double DenimFabric::stretchPercent() const {
    return stretchLimit_;
}

double DenimFabric::recommendedWashTemperature() const {
    return 30.0;
}

bool DenimFabric::suitableFor(Season season) const {
    if (season == Season::Summer) {
        return ounceWeight_ <= 10.0;
    }
    return true;
}

Leather::Leather(std::string name, LeatherType animal, double thicknessMm,
                 double breathability, double waterproofRating,
                 std::string tannery)
    : Material(std::move(name), textile::Fiber::Leather, 0.0, breathability,
               waterproofRating),
      animal_(animal),
      thicknessMm_(thicknessMm),
      tannery_(std::move(tannery)) {}

LeatherType Leather::animal() const { return animal_; }
double Leather::thicknessMm() const { return thicknessMm_; }
const std::string& Leather::tannery() const { return tannery_; }

double Leather::breakInHours() const {
    return thicknessMm_ * 10.0;
}

bool Leather::needsConditioning() const {
    return true;
}

double Leather::waterResistanceScore() const {
    return clampValue(6.0 + thicknessMm_, 0.0, 10.0);
}

double Leather::recommendedWashTemperature() const {
    return 0.0;
}

bool Leather::suitableFor(Season season) const {
    return season == Season::Autumn || season == Season::Winter ||
           season == Season::AllSeason;
}

double Leather::stretchPercent() const {
    return 2.0;
}

bool Leather::requiresDryCleaning() const {
    return true;
}

std::string Leather::describe() const {
    std::ostringstream out;
    out << Material::describe() << ", " << toString(animal_) << " leather, "
        << thicknessMm_ << " mm, tannery " << tannery_;
    return out.str();
}

SyntheticFabric::SyntheticFabric(std::string name, double weightGsm,
                                 double breathability, double waterproofRating,
                                 WeaveType weave, double widthCm,
                                 PolymerType polymer, double recycledPercent)
    : Fabric(std::move(name), textile::Fiber::Synthetic, weightGsm, breathability,
             waterproofRating, weave, widthCm),
      polymer_(polymer),
      recycledPercent_(recycledPercent) {}

PolymerType SyntheticFabric::polymer() const { return polymer_; }
double SyntheticFabric::recycledPercent() const { return recycledPercent_; }

double SyntheticFabric::meltRisk() const {
    return polymer_ == PolymerType::Polyester ? 0.6 : 0.4;
}

double SyntheticFabric::uvResistance() const {
    return recycledPercent_ > 50.0 ? 0.5 : 0.7;
}

double SyntheticFabric::microplasticShedding() const {
    return recycledPercent_ > 0.0 ? 0.4 : 0.8;
}

double SyntheticFabric::recommendedWashTemperature() const {
    return 30.0;
}

bool SyntheticFabric::suitableFor(Season season) const {
    if (season == Season::Summer) {
        return weightGsm_ <= 180.0;
    }
    return Fabric::suitableFor(season);
}

} // namespace clothing
