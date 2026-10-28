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

std::string toLowerCopy(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

std::string sleeveName(SleeveLength value) {
    switch (value) {
    case SleeveLength::Sleeveless: return "sleeveless";
    case SleeveLength::Short: return "short sleeve";
    case SleeveLength::ThreeQuarter: return "three-quarter sleeve";
    case SleeveLength::Long: return "long sleeve";
    }
    return "unknown sleeve";
}

std::string necklineName(Neckline value) {
    switch (value) {
    case Neckline::Crew: return "crew neck";
    case Neckline::VNeck: return "V neck";
    case Neckline::Polo: return "polo neck";
    case Neckline::Turtleneck: return "turtleneck";
    case Neckline::Boat: return "boat neck";
    }
    return "unknown neckline";
}

std::string fitName(FitType value) {
    switch (value) {
    case FitType::Slim: return "slim";
    case FitType::Regular: return "regular";
    case FitType::Loose: return "loose";
    case FitType::Oversized: return "oversized";
    }
    return "unknown";
}

std::string riseName(WaistRise value) {
    switch (value) {
    case WaistRise::Low: return "low rise";
    case WaistRise::Mid: return "mid rise";
    case WaistRise::High: return "high rise";
    }
    return "unknown rise";
}

std::string legName(LegShape value) {
    switch (value) {
    case LegShape::Straight: return "straight";
    case LegShape::Skinny: return "skinny";
    case LegShape::Bootcut: return "bootcut";
    case LegShape::Wide: return "wide";
    case LegShape::Tapered: return "tapered";
    }
    return "unknown";
}

std::string closureName(ClosureType value) {
    switch (value) {
    case ClosureType::Button: return "buttons";
    case ClosureType::Zipper: return "zipper";
    case ClosureType::Lace: return "laces";
    case ClosureType::Snap: return "snaps";
    case ClosureType::Buckle: return "buckle";
    case ClosureType::None: return "no closure";
    }
    return "unknown closure";
}

} // namespace

void Garment::initialize(const GarmentSpec& spec) {
    name_ = spec.name;
    brand_ = spec.brand;
    size_ = spec.size;
    color_ = spec.color;
    if (spec.material) {
        material_ = spec.material;
    } else {
        material_ = std::make_shared<CottonFabric>(
            "Cotton jersey", 150.0, 0.7, 2.0, WeaveType::Plain, 150.0, 120, true,
            true, "Belarus");
    }
    price_ = spec.price;
    care_ = spec.care;
    manufacturer_ = spec.manufacturer;
    season_ = spec.season;
    stockCount_ = std::max(0, spec.stockCount);
}

const std::string& Garment::name() const { return name_; }
const std::string& Garment::brand() const { return brand_; }
const Size& Garment::size() const { return size_; }
const Color& Garment::color() const { return color_; }
const std::shared_ptr<Material>& Garment::material() const { return material_; }
const PriceTag& Garment::price() const { return price_; }
const CareInstructions& Garment::care() const { return care_; }
const Manufacturer& Garment::manufacturer() const { return manufacturer_; }
Season Garment::season() const { return season_; }
bool Garment::inStock() const { return stockCount_ > 0; }
int Garment::stockCount() const { return stockCount_; }

std::string Garment::describe() const {
    std::ostringstream out;
    out << name_ << " by " << brand_ << " [" << size_.label() << " / "
        << color_.name() << "], " << (material_ ? material_->name() : "no material")
        << ", " << toString(season_) << ", " << std::fixed << std::setprecision(2)
        << totalPrice() << " " << toString(price_.currency());
    return out.str();
}

double Garment::suitabilityScore(Season season) const {
    double score = 5.0;
    if (season_ == Season::AllSeason || season == season_ || season == Season::AllSeason) {
        score += 3.0;
    }
    if (material_ && material_->suitableFor(season)) {
        score += 1.5;
    }
    if (color_.isNeutral()) {
        score += 0.5;
    }
    return clampValue(score, 0.0, 10.0);
}

bool Garment::isWaterproof() const {
    return material_ && material_->isWaterproof();
}

std::string Garment::cleanMethod() const {
    if (care_.dryCleanOnly()) {
        return "professional dry cleaning";
    }
    return care_.recommendedCycle();
}

bool Garment::foldable() const {
    return !(material_ && material_->requiresDryCleaning());
}

double Garment::totalPrice() const {
    return price_.finalPrice();
}

bool Garment::isAvailable() const {
    return stockCount_ > 0;
}

void Garment::restock(int quantity) {
    if (quantity <= 0) {
        throw InvalidPriceException("Restock quantity must be positive");
    }
    stockCount_ += quantity;
}

bool Garment::markSold(int quantity) {
    if (quantity <= 0) {
        throw OutOfStockException("Sale quantity must be positive");
    }
    if (stockCount_ < quantity) {
        throw OutOfStockException("Not enough units in stock to complete the sale");
    }
    stockCount_ -= quantity;
    return true;
}

void Garment::applyDiscount(double percent) {
    price_ = price_.applyDiscount(percent);
}

bool Garment::isCompatibleWith(const Garment& other) const {
    if (color_.isNeutral() || other.color_.isNeutral()) {
        return true;
    }
    if (!material_ || !other.material_) {
        return true;
    }
    return material_->fiber() == other.material_->fiber() || color_ == other.color_;
}

std::string Garment::careSummary() const {
    return name_ + ": " + care_.summary();
}

bool Garment::isSameSeason(Season season) const {
    return season_ == Season::AllSeason || season == Season::AllSeason || season_ == season;
}

Season Garment::parseSeason(const std::string& text) {
    const std::string value = toLowerCopy(text);
    if (value == "spring") return Season::Spring;
    if (value == "summer") return Season::Summer;
    if (value == "autumn" || value == "fall") return Season::Autumn;
    if (value == "winter") return Season::Winter;
    if (value == "all" || value == "allseason" || value == "all-season") {
        return Season::AllSeason;
    }
    throw InvalidSeasonException("Unknown season: " + text);
}

Top::Top(const GarmentSpec& spec, SleeveLength sleeve, Neckline neckline, FitType fit)
    : sleeveLength_(sleeve), neckline_(neckline), fit_(fit) {
    initialize(spec);
}

SleeveLength Top::sleeveLength() const { return sleeveLength_; }
Neckline Top::neckline() const { return neckline_; }
FitType Top::fit() const { return fit_; }

bool Top::coversArms() const {
    return sleeveLength_ != SleeveLength::Sleeveless;
}

bool Top::isFormal() const {
    return sleeveLength_ == SleeveLength::Long || neckline_ == Neckline::Turtleneck ||
           neckline_ == Neckline::Polo;
}

int Top::layerCount() const {
    return fit_ == FitType::Oversized ? 2 : 1;
}

bool Top::matchesBottom(const Bottom& bottom) const {
    if (color_.isNeutral() || bottom.color().isNeutral()) {
        return true;
    }
    return color_ == bottom.color();
}

std::string Top::describe() const {
    return Garment::describe() + ", " + sleeveName(sleeveLength_) + ", " +
           necklineName(neckline_) + ", " + fitName(fit_) + " fit";
}

Bottom::Bottom(const GarmentSpec& spec, WaistRise rise, LegShape shape,
               double lengthCm)
    : waistRise_(rise), legShape_(shape), lengthCm_(lengthCm) {
    initialize(spec);
}

WaistRise Bottom::waistRise() const { return waistRise_; }
LegShape Bottom::legShape() const { return legShape_; }
double Bottom::lengthCm() const { return lengthCm_; }

bool Bottom::isLong() const {
    return lengthCm_ >= 95.0;
}

bool Bottom::suitsFootwear(const Footwear& footwear) const {
    if (footwear.isHeeled()) {
        return lengthCm_ <= 105.0;
    }
    return true;
}

double Bottom::waistToHipRatio() const {
    return size_.waist().ratioTo(size_.hip());
}

std::string Bottom::describe() const {
    std::ostringstream out;
    out << Garment::describe() << ", " << riseName(waistRise_) << ", "
        << legName(legShape_) << ", " << lengthCm_ << " cm";
    return out.str();
}

Dress::Dress(const GarmentSpec& spec, bool hasTrain, std::string dressStyle)
    : Top(spec, SleeveLength::Sleeveless, Neckline::VNeck, FitType::Regular),
      Bottom(spec, WaistRise::High, LegShape::Wide, 110.0),
      hasTrain_(hasTrain),
      dressStyle_(std::move(dressStyle)) {}

bool Dress::hasTrain() const { return hasTrain_; }
const std::string& Dress::dressStyle() const { return dressStyle_; }

bool Dress::isEveningWear() const {
    return hasTrain_ || toLowerCopy(dressStyle_) == "evening";
}

std::string Dress::silhouette() const {
    if (legShape_ == LegShape::Wide) {
        return "A-line";
    }
    if (fit_ == FitType::Slim) {
        return "sheath";
    }
    return "straight";
}

bool Dress::requiresSpecialCare() const {
    return hasTrain_ || care_.dryCleanOnly() ||
           (material_ && material_->requiresDryCleaning());
}

std::string Dress::describe() const {
    return Garment::describe() + ", dress, " + dressStyle_ + ", silhouette " +
           silhouette() + (hasTrain_ ? ", with train" : "");
}

std::string Dress::cleanMethod() const {
    if (requiresSpecialCare()) {
        return "specialist dry cleaning with train protection";
    }
    return Garment::cleanMethod();
}

Shirt::Shirt(const GarmentSpec& spec, std::string collar, int buttonCount,
             std::string cuff)
    : Top(spec, SleeveLength::Long, Neckline::Crew, FitType::Regular),
      collar_(std::move(collar)),
      buttonCount_(buttonCount),
      cuff_(std::move(cuff)) {}

const std::string& Shirt::collar() const { return collar_; }
int Shirt::buttonCount() const { return buttonCount_; }
const std::string& Shirt::cuff() const { return cuff_; }

bool Shirt::isButtonedUp() const {
    return buttonCount_ > 0;
}

std::string Shirt::rollSleeves() const {
    if (sleeveLength_ == SleeveLength::Long) {
        return "sleeves rolled to the forearm";
    }
    return "sleeves are too short to roll";
}

double Shirt::formalRating() const {
    double score = 4.0;
    if (toLowerCopy(collar_) == "dress") score += 3.0;
    if (toLowerCopy(cuff_) == "french") score += 2.0;
    if (buttonCount_ >= 6) score += 1.0;
    return clampValue(score, 0.0, 10.0);
}

std::string Shirt::describe() const {
    return Top::describe() + ", shirt with " + collar_ + " collar";
}

TShirt::TShirt(const GarmentSpec& spec, std::string printText,
               std::string graphicTheme)
    : Shirt(spec, "crew", 0, "none"),
      printText_(std::move(printText)),
      graphicTheme_(std::move(graphicTheme)) {}

const std::string& TShirt::printText() const { return printText_; }
const std::string& TShirt::graphicTheme() const { return graphicTheme_; }

bool TShirt::isGraphic() const {
    return !printText_.empty() || graphicTheme_ != "none";
}

int TShirt::sloganLength() const {
    return static_cast<int>(printText_.size());
}

double TShirt::casualRating() const {
    return clampValue(7.0 + (isGraphic() ? 2.0 : 0.0), 0.0, 10.0);
}

std::string TShirt::describe() const {
    return Shirt::describe() + ", T-shirt" +
           (isGraphic() ? " with print \"" + printText_ + "\"" : "");
}

Blouse::Blouse(const GarmentSpec& spec, double sheen, bool tieOption)
    : Top(spec, SleeveLength::Long, Neckline::VNeck, FitType::Slim),
      sheen_(sheen),
      tieOption_(tieOption) {}

double Blouse::sheen() const { return sheen_; }
bool Blouse::hasTie() const { return tieOption_; }

bool Blouse::isElegant() const {
    return sheen_ > 0.5 && neckline_ == Neckline::VNeck;
}

bool Blouse::canTuck() const {
    return fit_ != FitType::Oversized;
}

double Blouse::drapeScore() const {
    return clampValue(sheen_ * 6.0 + (canTuck() ? 2.0 : 0.0), 0.0, 10.0);
}

Sweater::Sweater(const GarmentSpec& spec, int knitGauge, std::string collar,
                 int woolContent)
    : Top(spec, SleeveLength::Long, Neckline::Crew, FitType::Regular),
      knitGauge_(knitGauge),
      collar_(std::move(collar)),
      woolContent_(woolContent) {}

int Sweater::knitGauge() const { return knitGauge_; }
const std::string& Sweater::collar() const { return collar_; }
int Sweater::woolContent() const { return woolContent_; }

double Sweater::warmthLevel() const {
    return clampValue(knitGauge_ / 2.0 + woolContent_ / 20.0, 0.0, 10.0);
}

bool Sweater::isHandWashOnly() const {
    return woolContent_ > 50 || care_.dryCleanOnly();
}

double Sweater::pillRisk() const {
    return clampValue(woolContent_ / 100.0 + (knitGauge_ < 10 ? 0.2 : 0.0), 0.0, 1.0);
}

Hoodie::Hoodie(const GarmentSpec& spec, bool kangarooPocket,
               double drawstringLength)
    : Sweater(spec, 10, "hooded", 40),
      hasKangarooPocket_(kangarooPocket),
      drawstringLength_(drawstringLength) {}

bool Hoodie::hasKangarooPocket() const { return hasKangarooPocket_; }
double Hoodie::drawstringLength() const { return drawstringLength_; }

double Hoodie::pocketCapacity() const {
    return hasKangarooPocket_ ? 1.5 : 0.0;
}

std::string Hoodie::toggleDrawstring() const {
    return drawstringLength_ > 0.0 ? "drawstring tightened" : "no drawstring available";
}

double Hoodie::streetStyleScore() const {
    return clampValue(6.0 + pocketCapacity() * 2.0, 0.0, 10.0);
}

Pants::Pants(const GarmentSpec& spec, ClosureType closure, int pocketCount,
             bool pleated)
    : Bottom(spec, WaistRise::Mid, LegShape::Straight, 105.0),
      closure_(closure),
      pocketCount_(pocketCount),
      pleated_(pleated) {}

ClosureType Pants::closure() const { return closure_; }
int Pants::pocketCount() const { return pocketCount_; }
bool Pants::isPleated() const { return pleated_; }

bool Pants::hasPockets() const {
    return pocketCount_ > 0;
}

bool Pants::canRollCuffs() const {
    return closure_ != ClosureType::Buckle;
}

double Pants::formalityScore() const {
    double score = pleated_ ? 6.0 : 4.0;
    if (closure_ == ClosureType::Button) score += 1.0;
    if (closure_ == ClosureType::Zipper) score += 0.5;
    return clampValue(score, 0.0, 10.0);
}

Jeans::Jeans(const GarmentSpec& spec, std::string washType, int rivetCount,
             bool selvedge)
    : Pants(spec, ClosureType::Button, 5, false),
      washType_(std::move(washType)),
      rivetCount_(rivetCount),
      selvedge_(selvedge) {}

const std::string& Jeans::washType() const { return washType_; }
int Jeans::rivetCount() const { return rivetCount_; }
bool Jeans::isSelvedge() const { return selvedge_; }

double Jeans::fadeLevel() const {
    const std::string wash = toLowerCopy(washType_);
    if (wash == "raw") return 0.0;
    if (wash == "dark") return 1.0;
    if (wash == "medium") return 4.0;
    if (wash == "light") return 7.0;
    return 3.0;
}

int Jeans::recommendWashInterval() const {
    const std::string wash = toLowerCopy(washType_);
    if (wash == "raw") return 12;
    if (wash == "dark") return 8;
    if (wash == "light") return 4;
    return 6;
}

Shorts::Shorts(const GarmentSpec& spec, double inseamCm, bool cuffed)
    : Bottom(spec, WaistRise::Mid, LegShape::Straight, 45.0),
      inseamCm_(inseamCm),
      cuffed_(cuffed) {}

double Shorts::inseamCm() const { return inseamCm_; }
bool Shorts::isCuffed() const { return cuffed_; }

bool Shorts::isSporty() const {
    return inseamCm_ < 12.0;
}

bool Shorts::canConvertToLong() const {
    return false;
}

double Shorts::summerSuitability() const {
    return clampValue(10.0 - inseamCm_ / 5.0, 0.0, 10.0);
}

Skirt::Skirt(const GarmentSpec& spec, double flare, double slitHeight,
             std::string waistband)
    : Bottom(spec, WaistRise::High, LegShape::Wide, 60.0),
      flare_(flare),
      slitHeight_(slitHeight),
      waistband_(std::move(waistband)) {}

double Skirt::flare() const { return flare_; }
double Skirt::slitHeight() const { return slitHeight_; }
const std::string& Skirt::waistband() const { return waistband_; }

double Skirt::twirlRadius() const {
    return flare_ * 1.5 + lengthCm_;
}

bool Skirt::isPencil() const {
    return flare_ < 10.0;
}

double Skirt::occasionRating() const {
    double score = isPencil() ? 7.0 : 5.0;
    if (slitHeight_ > 20.0) score += 1.0;
    return clampValue(score, 0.0, 10.0);
}

Outerwear::Outerwear(const GarmentSpec& spec, std::string lining, bool hood,
                     ClosureType closure)
    : lining_(std::move(lining)), hood_(hood), closure_(closure) {
    initialize(spec);
}

const std::string& Outerwear::lining() const { return lining_; }
bool Outerwear::hasHood() const { return hood_; }
ClosureType Outerwear::closure() const { return closure_; }

bool Outerwear::isLayered() const {
    return hood_ || !lining_.empty();
}

double Outerwear::warmthIndex() const {
    const double base = material_ ? material_->warmthIndex() : 3.0;
    return clampValue(base + (hood_ ? 1.5 : 0.0), 0.0, 10.0);
}

bool Outerwear::protectsFromRain() const {
    if (material_ && material_->isWaterproof()) {
        return true;
    }
    return hood_ && material_ && material_->waterproofRating() >= 5.0;
}

Jacket::Jacket(const GarmentSpec& spec, std::string lapel, int pocketCount,
               bool quilted)
    : Outerwear(spec, "polyester", true, ClosureType::Zipper),
      lapel_(std::move(lapel)),
      pocketCount_(pocketCount),
      quilted_(quilted) {}

const std::string& Jacket::lapel() const { return lapel_; }
int Jacket::pocketCount() const { return pocketCount_; }
bool Jacket::isQuilted() const { return quilted_; }

bool Jacket::isBikerStyle() const {
    return toLowerCopy(lapel_) == "biker";
}

bool Jacket::zipUp() const {
    return closure_ == ClosureType::Zipper;
}

bool Jacket::layerOver(const Garment& other) const {
    return isCompatibleWith(other) && other.season() != Season::Winter;
}

std::string Jacket::describe() const {
    return Garment::describe() + ", jacket with " + lapel_ + " lapel, " +
           closureName(closure_);
}

Coat::Coat(const GarmentSpec& spec, double lengthCm, bool beltIncluded,
           bool trenchStyle)
    : Outerwear(spec, "wool", false, ClosureType::Button),
      lengthCm_(lengthCm),
      beltIncluded_(beltIncluded),
      trenchStyle_(trenchStyle) {}

double Coat::lengthCm() const { return lengthCm_; }
bool Coat::beltIncluded() const { return beltIncluded_; }
bool Coat::isTrench() const { return trenchStyle_; }

bool Coat::coversKnees() const {
    return lengthCm_ >= 100.0;
}

double Coat::formalityRating() const {
    double score = coversKnees() ? 7.0 : 5.0;
    if (beltIncluded_) score += 1.5;
    return clampValue(score, 0.0, 10.0);
}

Raincoat::Raincoat(const GarmentSpec& spec, int waterproofRating,
                   bool sealedSeams)
    : Coat(spec, 100.0, false, false),
      waterproofRating_(waterproofRating),
      sealedSeams_(sealedSeams) {}

int Raincoat::waterproofRating() const { return waterproofRating_; }
bool Raincoat::sealedSeams() const { return sealedSeams_; }

double Raincoat::rainProtectionScore() const {
    return clampValue(waterproofRating_ + (sealedSeams_ ? 2.0 : 0.0), 0.0, 10.0);
}

bool Raincoat::isSeamSealed() const {
    return sealedSeams_;
}

double Raincoat::packability() const {
    return clampValue(10.0 - (material_ ? material_->weightGsm() / 50.0 : 0.0), 0.0, 10.0);
}

Footwear::Footwear(const GarmentSpec& spec, double heelHeightCm,
                   std::string soleMaterial, ClosureType closure)
    : heelHeightCm_(heelHeightCm),
      soleMaterial_(std::move(soleMaterial)),
      closure_(closure) {
    initialize(spec);
}

double Footwear::heelHeightCm() const { return heelHeightCm_; }
const std::string& Footwear::soleMaterial() const { return soleMaterial_; }
ClosureType Footwear::closure() const { return closure_; }

bool Footwear::isHeeled() const {
    return heelHeightCm_ > 3.0;
}

double Footwear::cushioningScore() const {
    const std::string sole = toLowerCopy(soleMaterial_);
    if (sole == "eva") return 8.0;
    if (sole == "rubber") return 6.0;
    if (sole == "leather") return 4.0;
    return 5.0;
}

int Footwear::breakInDays() const {
    return isHeeled() ? 14 : 7;
}

Shoe::Shoe(const GarmentSpec& spec, double shoeWidth, std::string lining,
           std::string laceType)
    : Footwear(spec, 1.5, "rubber", ClosureType::Lace),
      shoeWidth_(shoeWidth),
      lining_(std::move(lining)),
      laceType_(std::move(laceType)) {}

double Shoe::shoeWidth() const { return shoeWidth_; }
const std::string& Shoe::lining() const { return lining_; }
const std::string& Shoe::laceType() const { return laceType_; }

bool Shoe::isLaceUp() const {
    return closure_ == ClosureType::Lace && toLowerCopy(laceType_) != "none";
}

std::string Shoe::widthCategory() const {
    if (shoeWidth_ < 9.0) return "narrow";
    if (shoeWidth_ < 11.0) return "standard";
    return "wide";
}

std::string Shoe::polish() const {
    return soleMaterial_ == "leather" ? "cream polish applied" : "wipe with damp cloth";
}

std::string Shoe::describe() const {
    return Garment::describe() + ", shoe, width " + widthCategory() + ", " +
           closureName(closure_);
}

Boot::Boot(const GarmentSpec& spec, double shaftHeightCm, bool weatherproof,
           int treadDepth)
    : Shoe(spec, 10.0, "leather", "lace"),
      shaftHeightCm_(shaftHeightCm),
      weatherproof_(weatherproof),
      treadDepth_(treadDepth) {}

double Boot::shaftHeightCm() const { return shaftHeightCm_; }
bool Boot::weatherproof() const { return weatherproof_; }
int Boot::treadDepth() const { return treadDepth_; }

bool Boot::ankleSupport() const {
    return shaftHeightCm_ >= 15.0;
}

bool Boot::isWinterReady() const {
    return weatherproof_ && treadDepth_ >= 5;
}

double Boot::waterproofScore() const {
    return clampValue((weatherproof_ ? 6.0 : 2.0) + treadDepth_ * 0.4, 0.0, 10.0);
}

Sneaker::Sneaker(const GarmentSpec& spec, std::string cushioningUnit,
                 std::string outsolePattern, double dropMm)
    : Shoe(spec, 10.0, "mesh", "lace"),
      cushioningUnit_(std::move(cushioningUnit)),
      outsolePattern_(std::move(outsolePattern)),
      dropMm_(dropMm) {}

const std::string& Sneaker::cushioningUnit() const { return cushioningUnit_; }
const std::string& Sneaker::outsolePattern() const { return outsolePattern_; }
double Sneaker::dropMm() const { return dropMm_; }

double Sneaker::runningScore() const {
    double score = cushioningScore();
    if (toLowerCopy(cushioningUnit_) == "foam") score += 1.5;
    if (dropMm_ <= 8.0) score += 0.5;
    return clampValue(score, 0.0, 10.0);
}

bool Sneaker::isTrailReady() const {
    return toLowerCopy(outsolePattern_) == "lugged";
}

double Sneaker::impactAbsorption() const {
    return clampValue(cushioningScore() - dropMm_ / 10.0, 0.0, 10.0);
}

Sandal::Sandal(const GarmentSpec& spec, int strapCount, bool openToe,
               bool archSupport)
    : Shoe(spec, 10.0, "leather", "none"),
      strapCount_(strapCount),
      openToe_(openToe),
      archSupport_(archSupport) {}

int Sandal::strapCount() const { return strapCount_; }
bool Sandal::openToe() const { return openToe_; }
bool Sandal::archSupport() const { return archSupport_; }

bool Sandal::isBreathable() const {
    return openToe_;
}

bool Sandal::summerFriendly() const {
    return openToe_ && strapCount_ >= 2;
}

int Sandal::adjustmentCount() const {
    return strapCount_;
}

Headwear::Headwear(const GarmentSpec& spec, double circumferenceCm,
                   double brimWidthCm, bool adjustable)
    : circumferenceCm_(circumferenceCm),
      brimWidthCm_(brimWidthCm),
      adjustable_(adjustable) {
    initialize(spec);
}

double Headwear::circumferenceCm() const { return circumferenceCm_; }
double Headwear::brimWidthCm() const { return brimWidthCm_; }
bool Headwear::adjustable() const { return adjustable_; }

bool Headwear::fitsHead(double headCircumferenceCm) const {
    return std::abs(headCircumferenceCm - circumferenceCm_) <= 2.0;
}

bool Headwear::isAdjustable() const {
    return adjustable_;
}

double Headwear::sunProtectionFactor() const {
    const double materialFactor = material_ ? material_->weightGsm() / 100.0 : 0.0;
    return clampValue(brimWidthCm_ / 2.0 + materialFactor, 0.0, 10.0);
}

Hat::Hat(const GarmentSpec& spec, double crownHeightCm, std::string materialType,
         std::string brimStyle)
    : Headwear(spec, 57.0, 10.0, false),
      crownHeightCm_(crownHeightCm),
      materialType_(std::move(materialType)),
      brimStyle_(std::move(brimStyle)) {}

double Hat::crownHeightCm() const { return crownHeightCm_; }
const std::string& Hat::materialType() const { return materialType_; }
const std::string& Hat::brimStyle() const { return brimStyle_; }

bool Hat::isFedora() const {
    return toLowerCopy(brimStyle_) == "wide" && crownHeightCm_ >= 10.0;
}

double Hat::shapeRating() const {
    return clampValue(crownHeightCm_ / 2.0 + brimWidthCm_ / 3.0, 0.0, 10.0);
}

double Hat::packability() const {
    return clampValue(10.0 - crownHeightCm_ / 3.0, 0.0, 10.0);
}

Cap::Cap(const GarmentSpec& spec, std::string visorStyle, bool snapback,
         std::string teamLogo)
    : Hat(spec, 10.0, "cotton", "short"),
      visorStyle_(std::move(visorStyle)),
      snapback_(snapback),
      teamLogo_(std::move(teamLogo)) {}

const std::string& Cap::visorStyle() const { return visorStyle_; }
bool Cap::snapback() const { return snapback_; }
const std::string& Cap::teamLogo() const { return teamLogo_; }

bool Cap::isSnapback() const {
    return snapback_;
}

bool Cap::hasTeamLogo() const {
    return !teamLogo_.empty();
}

double Cap::casualScore() const {
    return clampValue(7.0 + (hasTeamLogo() ? 1.5 : 0.0), 0.0, 10.0);
}

Beanie::Beanie(const GarmentSpec& spec, double foldHeightCm, bool pomPom,
               std::string knitPattern)
    : Hat(spec, 20.0, "wool", "none"),
      foldHeightCm_(foldHeightCm),
      pomPom_(pomPom),
      knitPattern_(std::move(knitPattern)) {}

double Beanie::foldHeightCm() const { return foldHeightCm_; }
bool Beanie::hasPomPom() const { return pomPom_; }
const std::string& Beanie::knitPattern() const { return knitPattern_; }

bool Beanie::isSlouchy() const {
    return foldHeightCm_ > 5.0;
}

double Beanie::warmthRating() const {
    return clampValue(foldHeightCm_ + (material_ ? material_->warmthIndex() : 0.0),
                      0.0, 10.0);
}

bool Beanie::canFold() const {
    return foldHeightCm_ > 0.0;
}

Neckwear::Neckwear(const GarmentSpec& spec, double lengthCm, double widthCm,
                   std::string style)
    : lengthCm_(lengthCm), widthCm_(widthCm), style_(std::move(style)) {
    initialize(spec);
}

double Neckwear::lengthCm() const { return lengthCm_; }
double Neckwear::widthCm() const { return widthCm_; }
const std::string& Neckwear::style() const { return style_; }

int Neckwear::wrapCount() const {
    return std::max(1, static_cast<int>(lengthCm_ / 80.0));
}

double Neckwear::warmthRating() const {
    return clampValue(widthCm_ / 5.0 + (material_ ? material_->warmthIndex() : 0.0),
                      0.0, 10.0);
}

bool Neckwear::isInfinity() const {
    return toLowerCopy(style_) == "infinity";
}

Scarf::Scarf(const GarmentSpec& spec, double fringeLength, std::string pattern)
    : Neckwear(spec, 180.0, 30.0, "classic"),
      fringeLength_(fringeLength),
      pattern_(std::move(pattern)) {}

double Scarf::fringeLength() const { return fringeLength_; }
const std::string& Scarf::pattern() const { return pattern_; }

double Scarf::fringeDensity() const {
    return clampValue(fringeLength_ * 2.0, 0.0, 10.0);
}

std::string Scarf::knotStyle() const {
    return wrapCount() > 1 ? "double loop" : "single loop";
}

double Scarf::windProtection() const {
    return clampValue(warmthRating() + (isInfinity() ? 1.0 : 0.0), 0.0, 10.0);
}

Accessory::Accessory(const GarmentSpec& spec, std::string purpose, bool adjustable,
                     std::string metallicFinish)
    : purpose_(std::move(purpose)),
      adjustable_(adjustable),
      metallicFinish_(std::move(metallicFinish)) {
    initialize(spec);
}

const std::string& Accessory::purpose() const { return purpose_; }
bool Accessory::adjustable() const { return adjustable_; }
const std::string& Accessory::metallicFinish() const { return metallicFinish_; }

bool Accessory::isDecorative() const {
    return toLowerCopy(purpose_) == "decorative";
}

double Accessory::adjustabilityScore() const {
    return adjustable_ ? 8.0 : 3.0;
}

bool Accessory::matchesColor(const Color& other) const {
    return color_ == other || color_.isNeutral() || other.isNeutral();
}

Belt::Belt(const GarmentSpec& spec, std::string buckleType, double widthCm,
           int holeCount)
    : Accessory(spec, "functional", true, "metal"),
      buckleType_(std::move(buckleType)),
      widthCm_(widthCm),
      holeCount_(holeCount) {}

const std::string& Belt::buckleType() const { return buckleType_; }
double Belt::widthCm() const { return widthCm_; }
int Belt::holeCount() const { return holeCount_; }

bool Belt::isReversible() const {
    return toLowerCopy(buckleType_) == "reversible";
}

bool Belt::fitsWaist(double waistCm) const {
    const double center = size_.waist().centimeters();
    const double tolerance = holeCount_ * 2.0;
    return waistCm >= center - tolerance && waistCm <= center + tolerance;
}

double Belt::buckleShine() const {
    return toLowerCopy(metallicFinish_) == "polished" ? 9.0 : 5.0;
}

Gloves::Gloves(const GarmentSpec& spec, bool fingerless,
               bool touchscreenCompatible, std::string lining)
    : Accessory(spec, "protective", false, "none"),
      fingerless_(fingerless),
      touchscreenCompatible_(touchscreenCompatible),
      lining_(std::move(lining)) {}

bool Gloves::fingerless() const { return fingerless_; }
bool Gloves::touchscreenCompatible() const { return touchscreenCompatible_; }
const std::string& Gloves::lining() const { return lining_; }

bool Gloves::isWinterReady() const {
    return !fingerless_ && !lining_.empty();
}

double Gloves::touchAccuracy() const {
    return touchscreenCompatible_ ? 8.5 : 2.0;
}

double Gloves::dexterityScore() const {
    return clampValue((fingerless_ ? 9.0 : 6.0) - (lining_.empty() ? 0.0 : 1.0),
                      0.0, 10.0);
}

Socks::Socks(const GarmentSpec& spec, double crewHeight, int cushioning,
             bool moistureControl)
    : Accessory(spec, "protective", true, "none"),
      crewHeight_(crewHeight),
      cushioning_(cushioning),
      moistureControl_(moistureControl) {}

double Socks::crewHeight() const { return crewHeight_; }
int Socks::cushioning() const { return cushioning_; }
bool Socks::moistureControl() const { return moistureControl_; }

bool Socks::isAthletic() const {
    return cushioning_ >= 3 && moistureControl_;
}

double Socks::warmthRating() const {
    return clampValue(crewHeight_ / 5.0 + cushioning_ * 0.5, 0.0, 10.0);
}

double Socks::blisterRisk() const {
    return clampValue(0.8 - cushioning_ * 0.15 - (moistureControl_ ? 0.2 : 0.0),
                      0.0, 1.0);
}

Bag::Bag(const GarmentSpec& spec, double capacityLiters, std::string strapType,
         int compartments)
    : Accessory(spec, "carrying", true, "none"),
      capacityLiters_(capacityLiters),
      strapType_(std::move(strapType)),
      compartments_(compartments) {}

double Bag::capacityLiters() const { return capacityLiters_; }
const std::string& Bag::strapType() const { return strapType_; }
int Bag::compartments() const { return compartments_; }

bool Bag::isCarryOn() const {
    return capacityLiters_ <= 40.0;
}

std::string Bag::organize() const {
    return "organized across " + std::to_string(compartments_) + " compartments";
}

double Bag::loadScore() const {
    return clampValue(capacityLiters_ / 4.0 + compartments_, 0.0, 10.0);
}

} // namespace clothing
