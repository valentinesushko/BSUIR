#ifndef CLOTHING_CLOTHING_H
#define CLOTHING_CLOTHING_H

#include "clothing/Exceptions.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

/**
 * @file Clothing.h
 * @brief Object model of the "Clothing" domain.
 *
 * The header demonstrates public, protected, and private inheritance,
 * virtual and multiple inheritance, virtual methods, and name resolution
 * with a using-directive (see CareInstructions::isSafeFor in Core.cpp).
 */

namespace clothing {

/** @brief Textile vocabulary, resolved explicitly as textile::Fiber. */
namespace textile {
enum class Fiber { Cotton, Wool, Silk, Denim, Linen, Synthetic, Leather };
}

/** @brief Care vocabulary whose names are pulled in with a using-directive. */
namespace garment_care {
int recommendedTemperature(textile::Fiber fiber);
std::string cycleName(int temperature);
}

enum class Season { Spring, Summer, Autumn, Winter, AllSeason };
enum class SizeSystem { EU, US, UK, International };
enum class MeasurementType { Chest, Waist, Hip, Inseam, Length };
enum class WeaveType { Plain, Twill, Satin, Pique, Rib };
enum class LeatherType { Cow, Sheep, Goat, Pig };
enum class PolymerType { Polyester, Nylon, Elastane, Acrylic };
enum class Currency { USD, EUR, BYN };
enum class ClosureType { Button, Zipper, Lace, Snap, Buckle, None };
enum class FitType { Slim, Regular, Loose, Oversized };
enum class SleeveLength { Sleeveless, Short, ThreeQuarter, Long };
enum class Neckline { Crew, VNeck, Polo, Turtleneck, Boat };
enum class WaistRise { Low, Mid, High };
enum class LegShape { Straight, Skinny, Bootcut, Wide, Tapered };
enum class ActivityType { Running, Yoga, Hiking, Cycling, Gym };

std::string toString(Season value);
std::string toString(textile::Fiber value);
std::string toString(SizeSystem value);
std::string toString(MeasurementType value);
std::string toString(WeaveType value);
std::string toString(LeatherType value);
std::string toString(PolymerType value);
std::string toString(Currency value);
std::string toString(ActivityType value);

/**
 * @brief A typed body measurement stored in centimeters.
 */
class Measurement {
public:
    Measurement();
    Measurement(double centimeters, MeasurementType type);

    double centimeters() const;
    void setCentimeters(double centimeters);
    MeasurementType type() const;

    double toInches() const;
    Measurement add(const Measurement& other) const;
    bool isWithinRange(double minCm, double maxCm) const;
    double ratioTo(const Measurement& other) const;
    void validate() const;

private:
    double centimeters_;
    MeasurementType type_;
};

/**
 * @brief An RGB color with an optional human readable name.
 */
class Color {
public:
    Color();
    Color(int red, int green, int blue, std::string name);

    int red() const;
    int green() const;
    int blue() const;
    const std::string& name() const;

    std::string toHex() const;
    Color mix(const Color& other, double ratio) const;
    Color lighten(double factor) const;
    Color darken(double factor) const;
    double contrastWith(const Color& other) const;
    bool isNeutral() const;
    void validate() const;

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

private:
    int red_;
    int green_;
    int blue_;
    std::string name_;
};

/**
 * @brief A garment size in a concrete sizing system.
 */
class Size {
public:
    Size();
    Size(std::string label, SizeSystem system, Measurement chest,
         Measurement waist, Measurement hip);

    const std::string& label() const;
    SizeSystem system() const;
    const Measurement& chest() const;
    const Measurement& waist() const;
    const Measurement& hip() const;

    bool fits(const Measurement& bodyChest) const;
    bool isCompatibleWith(const Size& other) const;
    Size convertedTo(SizeSystem system) const;
    int numericLabel() const;
    void validate() const;
    void ensureSameSystem(const Size& other) const;

private:
    std::string label_;
    SizeSystem system_;
    Measurement chest_;
    Measurement waist_;
    Measurement hip_;
};

/**
 * @brief Abstract material with virtual care and suitability behavior.
 */
class Material {
public:
    virtual ~Material() = default;

    const std::string& name() const;
    textile::Fiber fiber() const;
    double weightGsm() const;
    double breathability() const;
    double waterproofRating() const;

    virtual double recommendedWashTemperature() const = 0;
    virtual bool suitableFor(Season season) const = 0;
    virtual double stretchPercent() const = 0;
    virtual bool requiresDryCleaning() const;
    virtual std::string describe() const;

    double warmthIndex() const;
    bool isLightweight() const;
    bool isWaterproof() const;

protected:
    Material(std::string name, textile::Fiber fiber, double weightGsm,
             double breathability, double waterproofRating);

    std::string name_;
    textile::Fiber fiber_;
    double weightGsm_;
    double breathability_;
    double waterproofRating_;
};

/**
 * @brief A woven or knitted fabric, publicly derived from Material.
 */
class Fabric : public Material {
public:
    Fabric(std::string name, textile::Fiber fiber, double weightGsm,
           double breathability, double waterproofRating, WeaveType weave,
           double widthCm);

    WeaveType weave() const;
    double widthCm() const;

    double areaWeightFor(double meters) const;
    bool canBeDyed() const;
    virtual double shrinkagePercent() const;
    bool isWashable() const;

    double recommendedWashTemperature() const override;
    bool suitableFor(Season season) const override;
    double stretchPercent() const override;
    std::string describe() const override;

protected:
    WeaveType weave_;
    double widthCm_;
};

/** @brief Knitted fabric with elasticity behavior. */
class KnitFabric : public Fabric {
public:
    KnitFabric(std::string name, textile::Fiber fiber, double weightGsm,
               double breathability, double waterproofRating, WeaveType weave,
               double widthCm, int stitchDensity, double elasticity);

    double stretchPercent() const override;
    double stretchesTo(double lengthCm) const;
    bool recoversAfterWash() const;
    int recommendNeedleSize() const;

protected:
    int stitchDensity_;
    double elasticity_;
};

/** @brief Woven fabric with thread-count behavior. */
class WovenFabric : public Fabric {
public:
    WovenFabric(std::string name, textile::Fiber fiber, double weightGsm,
                double breathability, double waterproofRating, WeaveType weave,
                double widthCm, int threadCount, bool warpAligned);

    double densityRating() const;
    bool isFrayResistant() const;
    int recommendedIronTemperature() const;
    double shrinkagePercent() const override;

protected:
    int threadCount_;
    bool warpAligned_;
};

/** @brief Cotton fabric with absorbency and allergy behavior. */
class CottonFabric : public WovenFabric {
public:
    CottonFabric(std::string name, double weightGsm, double breathability,
                 double waterproofRating, WeaveType weave, double widthCm,
                 int threadCount, bool warpAligned, bool organic,
                 std::string originRegion);

    bool isOrganic() const;
    const std::string& originRegion() const;
    double absorbencyRate() const;
    bool isHypoallergenic() const;
    double dyeAffinity() const;

    bool suitableFor(Season season) const override;
    double recommendedWashTemperature() const override;

private:
    bool organic_;
    std::string originRegion_;
};

/** @brief Wool fabric with warmth and felting behavior. */
class WoolFabric : public KnitFabric {
public:
    WoolFabric(std::string name, double weightGsm, double breathability,
               double waterproofRating, WeaveType weave, double widthCm,
               int stitchDensity, double elasticity, int micronCount,
               bool merino);

    int micronCount() const;
    bool isMerino() const;
    double warmthRating() const;
    bool feltsInWash() const;
    double allergenRisk() const;

    double recommendedWashTemperature() const override;
    bool requiresDryCleaning() const override;
    bool suitableFor(Season season) const override;

private:
    int micronCount_;
    bool merino_;
};

/** @brief Silk fabric with luster and drape behavior. */
class SilkFabric : public WovenFabric {
public:
    SilkFabric(std::string name, double weightGsm, double breathability,
               double waterproofRating, WeaveType weave, double widthCm,
               int threadCount, bool warpAligned, double momme, double sheen);

    double momme() const;
    double sheen() const;
    double lusterScore() const;
    bool needsDelicateWash() const;
    bool drapesWell() const;

    double recommendedWashTemperature() const override;
    bool requiresDryCleaning() const override;
    bool suitableFor(Season season) const override;

private:
    double momme_;
    double sheen_;
};

/** @brief Denim fabric with fading and rigidity behavior. */
class DenimFabric : public WovenFabric {
public:
    DenimFabric(std::string name, double weightGsm, double breathability,
                double waterproofRating, WeaveType weave, double widthCm,
                int threadCount, bool warpAligned, double ounceWeight,
                double stretchLimit);

    double ounceWeight() const;
    double stretchLimit() const;
    double fadeAfterWashes(int washes) const;
    bool isRigid() const;
    double recommendSoakHours() const;

    double stretchPercent() const override;
    double recommendedWashTemperature() const override;
    bool suitableFor(Season season) const override;

private:
    double ounceWeight_;
    double stretchLimit_;
};

/** @brief Leather material with conditioning and break-in behavior. */
class Leather : public Material {
public:
    Leather(std::string name, LeatherType animal, double thicknessMm,
            double breathability, double waterproofRating, std::string tannery);

    LeatherType animal() const;
    double thicknessMm() const;
    const std::string& tannery() const;

    double breakInHours() const;
    bool needsConditioning() const;
    double waterResistanceScore() const;

    double recommendedWashTemperature() const override;
    bool suitableFor(Season season) const override;
    double stretchPercent() const override;
    bool requiresDryCleaning() const override;
    std::string describe() const override;

private:
    LeatherType animal_;
    double thicknessMm_;
    std::string tannery_;
};

/** @brief Synthetic fabric with melt and microplastic behavior. */
class SyntheticFabric : public Fabric {
public:
    SyntheticFabric(std::string name, double weightGsm, double breathability,
                    double waterproofRating, WeaveType weave, double widthCm,
                    PolymerType polymer, double recycledPercent);

    PolymerType polymer() const;
    double recycledPercent() const;
    double meltRisk() const;
    double uvResistance() const;
    double microplasticShedding() const;

    double recommendedWashTemperature() const override;
    bool suitableFor(Season season) const override;

private:
    PolymerType polymer_;
    double recycledPercent_;
};

/** @brief Care label with washing, drying, bleaching, and ironing rules. */
class CareInstructions {
public:
    CareInstructions();
    CareInstructions(int washTemperature, bool canTumbleDry, bool canBleach,
                     int ironTemperature, bool dryCleanOnly);

    int washTemperature() const;
    bool canTumbleDry() const;
    bool canBleach() const;
    int ironTemperature() const;
    bool dryCleanOnly() const;

    CareInstructions merge(const CareInstructions& other) const;
    std::string recommendedCycle() const;
    bool isSafeFor(const Material& material) const;
    std::string summary() const;
    bool isGentle() const;
    void validate() const;

private:
    int washTemperature_;
    bool canTumbleDry_;
    bool canBleach_;
    int ironTemperature_;
    bool dryCleanOnly_;
};

/** @brief Manufacturer and provenance information. */
class Manufacturer {
public:
    Manufacturer();
    Manufacturer(std::string name, std::string country, std::string contactEmail,
                 int foundedYear, double ethicalRating);

    const std::string& name() const;
    const std::string& country() const;
    const std::string& contactEmail() const;
    int foundedYear() const;
    double ethicalRating() const;

    bool isEuropean() const;
    double reputationScore() const;
    std::string contactInfo() const;
    int yearsInBusiness(int currentYear) const;

private:
    std::string name_;
    std::string country_;
    std::string contactEmail_;
    int foundedYear_;
    double ethicalRating_;
};

/** @brief Price with discount and tax behavior. */
class PriceTag {
public:
    PriceTag();
    PriceTag(double amount, Currency currency, double discountPercent,
             double taxPercent);

    double amount() const;
    Currency currency() const;
    double discountPercent() const;
    double taxPercent() const;

    double finalPrice() const;
    PriceTag applyDiscount(double percent) const;
    PriceTag convertTo(Currency target, double rate) const;
    bool isAffordable(double budget) const;
    double taxAmount() const;
    void validate() const;

private:
    double amount_;
    Currency currency_;
    double discountPercent_;
    double taxPercent_;
};

/** @brief Base class that gives every domain object a stable identity. */
class Identifiable {
public:
    Identifiable();
    explicit Identifiable(std::string id, long createdAt);

    const std::string& id() const;
    long createdAt() const;

    void regenerateId();
    bool isSame(const Identifiable& other) const;
    std::string identitySummary() const;

private:
    std::string id_;
    long createdAt_;
};

class Top;
class Bottom;
class Dress;
class Outerwear;
class Footwear;
class Accessory;
class Outfit;

/**
 * @brief Data bundle passed to every garment constructor.
 */
struct GarmentSpec {
    std::string name{"Unnamed garment"};
    std::string brand{"Generic"};
    Size size{};
    Color color{};
    std::shared_ptr<Material> material{};
    PriceTag price{};
    CareInstructions care{};
    Manufacturer manufacturer{};
    Season season{Season::AllSeason};
    int stockCount{0};
};

/**
 * @brief Root of the garment hierarchy with virtual behavior.
 */
class Garment : public Identifiable {
public:
    Garment() = default;
    virtual ~Garment() = default;

    const std::string& name() const;
    const std::string& brand() const;
    const Size& size() const;
    const Color& color() const;
    const std::shared_ptr<Material>& material() const;
    const PriceTag& price() const;
    const CareInstructions& care() const;
    const Manufacturer& manufacturer() const;
    Season season() const;
    bool inStock() const;
    int stockCount() const;

    virtual std::string describe() const;
    virtual double suitabilityScore(Season season) const;
    virtual bool isWaterproof() const;
    virtual std::string cleanMethod() const;
    virtual bool foldable() const;

    double totalPrice() const;
    bool isAvailable() const;
    void restock(int quantity);
    bool markSold(int quantity);
    void applyDiscount(double percent);
    bool isCompatibleWith(const Garment& other) const;
    std::string careSummary() const;
    bool isSameSeason(Season season) const;
    static Season parseSeason(const std::string& text);

protected:
    void initialize(const GarmentSpec& spec);

    std::string name_;
    std::string brand_;
    Size size_;
    Color color_;
    std::shared_ptr<Material> material_;
    PriceTag price_;
    CareInstructions care_;
    Manufacturer manufacturer_;
    Season season_;
    int stockCount_;
};

/** @brief Upper-body garment; virtual inheritance from Garment. */
class Top : public virtual Garment {
public:
    Top() = default;
    Top(const GarmentSpec& spec, SleeveLength sleeve = SleeveLength::Long,
        Neckline neckline = Neckline::Crew, FitType fit = FitType::Regular);

    SleeveLength sleeveLength() const;
    Neckline neckline() const;
    FitType fit() const;

    bool coversArms() const;
    bool isFormal() const;
    int layerCount() const;
    bool matchesBottom(const Bottom& bottom) const;
    std::string describe() const override;

protected:
    SleeveLength sleeveLength_{SleeveLength::Long};
    Neckline neckline_{Neckline::Crew};
    FitType fit_{FitType::Regular};
};

/** @brief Lower-body garment; virtual inheritance from Garment. */
class Bottom : public virtual Garment {
public:
    Bottom() = default;
    Bottom(const GarmentSpec& spec, WaistRise rise = WaistRise::Mid,
           LegShape shape = LegShape::Straight, double lengthCm = 100.0);

    WaistRise waistRise() const;
    LegShape legShape() const;
    double lengthCm() const;

    bool isLong() const;
    bool suitsFootwear(const Footwear& footwear) const;
    double waistToHipRatio() const;
    std::string describe() const override;

protected:
    WaistRise waistRise_{WaistRise::Mid};
    LegShape legShape_{LegShape::Straight};
    double lengthCm_{100.0};
};

/** @brief Dress combining Top and Bottom; resolves the diamond virtually. */
class Dress : public Top, public Bottom {
public:
    Dress() = default;
    Dress(const GarmentSpec& spec, bool hasTrain = false,
          std::string dressStyle = "casual");

    bool hasTrain() const;
    const std::string& dressStyle() const;

    bool isEveningWear() const;
    std::string silhouette() const;
    bool requiresSpecialCare() const;
    std::string describe() const override;
    std::string cleanMethod() const override;

private:
    bool hasTrain_;
    std::string dressStyle_;
};

/** @brief Shirt derived from Top. */
class Shirt : public Top {
public:
    Shirt() = default;
    Shirt(const GarmentSpec& spec, std::string collar = "crew",
          int buttonCount = 5, std::string cuff = "button");

    const std::string& collar() const;
    int buttonCount() const;
    const std::string& cuff() const;

    bool isButtonedUp() const;
    std::string rollSleeves() const;
    double formalRating() const;
    std::string describe() const override;

private:
    std::string collar_;
    int buttonCount_;
    std::string cuff_;
};

/** @brief T-shirt derived from Shirt. */
class TShirt : public Shirt {
public:
    TShirt() = default;
    TShirt(const GarmentSpec& spec, std::string printText = "",
           std::string graphicTheme = "none");

    const std::string& printText() const;
    const std::string& graphicTheme() const;

    bool isGraphic() const;
    int sloganLength() const;
    double casualRating() const;
    std::string describe() const override;

private:
    std::string printText_;
    std::string graphicTheme_;
};

/** @brief Blouse derived from Top. */
class Blouse : public Top {
public:
    Blouse() = default;
    Blouse(const GarmentSpec& spec, double sheen = 0.6, bool tieOption = true);

    double sheen() const;
    bool hasTie() const;

    bool isElegant() const;
    bool canTuck() const;
    double drapeScore() const;

private:
    double sheen_;
    bool tieOption_;
};

/** @brief Sweater derived from Top. */
class Sweater : public Top {
public:
    Sweater() = default;
    Sweater(const GarmentSpec& spec, int knitGauge = 12,
            std::string collar = "crew", int woolContent = 60);

    int knitGauge() const;
    const std::string& collar() const;
    int woolContent() const;

    double warmthLevel() const;
    bool isHandWashOnly() const;
    double pillRisk() const;

private:
    int knitGauge_;
    std::string collar_;
    int woolContent_;
};

/** @brief Hoodie derived from Sweater. */
class Hoodie : public Sweater {
public:
    Hoodie() = default;
    Hoodie(const GarmentSpec& spec, bool kangarooPocket = true,
           double drawstringLength = 80.0);

    bool hasKangarooPocket() const;
    double drawstringLength() const;

    double pocketCapacity() const;
    std::string toggleDrawstring() const;
    double streetStyleScore() const;

private:
    bool hasKangarooPocket_;
    double drawstringLength_;
};

/** @brief Pants derived from Bottom. */
class Pants : public Bottom {
public:
    Pants() = default;
    Pants(const GarmentSpec& spec, ClosureType closure = ClosureType::Zipper,
          int pocketCount = 4, bool pleated = false);

    ClosureType closure() const;
    int pocketCount() const;
    bool isPleated() const;

    bool hasPockets() const;
    bool canRollCuffs() const;
    double formalityScore() const;

private:
    ClosureType closure_;
    int pocketCount_;
    bool pleated_;
};

/** @brief Jeans derived from Pants. */
class Jeans : public Pants {
public:
    Jeans() = default;
    Jeans(const GarmentSpec& spec, std::string washType = "dark",
          int rivetCount = 6, bool selvedge = false);

    const std::string& washType() const;
    int rivetCount() const;
    bool isSelvedge() const;

    double fadeLevel() const;
    int recommendWashInterval() const;

private:
    std::string washType_;
    int rivetCount_;
    bool selvedge_;
};

/** @brief Shorts derived from Bottom. */
class Shorts : public Bottom {
public:
    Shorts() = default;
    Shorts(const GarmentSpec& spec, double inseamCm = 20.0,
           bool cuffed = false);

    double inseamCm() const;
    bool isCuffed() const;

    bool isSporty() const;
    bool canConvertToLong() const;
    double summerSuitability() const;

private:
    double inseamCm_;
    bool cuffed_;
};

/** @brief Skirt derived from Bottom. */
class Skirt : public Bottom {
public:
    Skirt() = default;
    Skirt(const GarmentSpec& spec, double flare = 40.0,
          double slitHeight = 15.0, std::string waistband = "elastic");

    double flare() const;
    double slitHeight() const;
    const std::string& waistband() const;

    double twirlRadius() const;
    bool isPencil() const;
    double occasionRating() const;

private:
    double flare_;
    double slitHeight_;
    std::string waistband_;
};

/** @brief Outerwear layer; virtual inheritance from Garment. */
class Outerwear : public virtual Garment {
public:
    Outerwear() = default;
    Outerwear(const GarmentSpec& spec, std::string lining = "polyester",
              bool hood = true, ClosureType closure = ClosureType::Zipper);

    const std::string& lining() const;
    bool hasHood() const;
    ClosureType closure() const;

    bool isLayered() const;
    double warmthIndex() const;
    bool protectsFromRain() const;

protected:
    std::string lining_;
    bool hood_;
    ClosureType closure_;
};

/** @brief Jacket derived from Outerwear. */
class Jacket : public Outerwear {
public:
    Jacket() = default;
    Jacket(const GarmentSpec& spec, std::string lapel = "notch",
           int pocketCount = 2, bool quilted = false);

    const std::string& lapel() const;
    int pocketCount() const;
    bool isQuilted() const;

    bool isBikerStyle() const;
    bool zipUp() const;
    bool layerOver(const Garment& other) const;
    std::string describe() const override;

private:
    std::string lapel_;
    int pocketCount_;
    bool quilted_;
};

/** @brief Coat derived from Outerwear. */
class Coat : public Outerwear {
public:
    Coat() = default;
    Coat(const GarmentSpec& spec, double lengthCm = 110.0,
         bool beltIncluded = true, bool trenchStyle = false);

    double lengthCm() const;
    bool beltIncluded() const;
    bool isTrench() const;

    bool coversKnees() const;
    double formalityRating() const;

private:
    double lengthCm_;
    bool beltIncluded_;
    bool trenchStyle_;
};

/** @brief Raincoat derived from Coat. */
class Raincoat : public Coat {
public:
    Raincoat() = default;
    Raincoat(const GarmentSpec& spec, int waterproofRating = 9,
             bool sealedSeams = true);

    int waterproofRating() const;
    bool sealedSeams() const;

    double rainProtectionScore() const;
    bool isSeamSealed() const;
    double packability() const;

private:
    int waterproofRating_;
    bool sealedSeams_;
};

/** @brief Sportswear mixin used by multiple inheritance. */
class Sportswear {
public:
    Sportswear() = default;
    Sportswear(ActivityType activity, bool moistureWicking,
               double stretchFactor);

    ActivityType activity() const;
    bool moistureWicking() const;
    double stretchFactor() const;

    bool isHighImpact() const;
    double sweatManagementScore() const;
    double flexibilityRating() const;
    std::string describeSport() const;

private:
    ActivityType activity_;
    bool moistureWicking_;
    double stretchFactor_;
};

/** @brief Sport jacket via multiple inheritance. */
class SportJacket : public Jacket, public Sportswear {
public:
    SportJacket() = default;
    SportJacket(const GarmentSpec& spec, ActivityType activity = ActivityType::Running,
                bool moistureWicking = true, double stretchFactor = 20.0,
                int reflectiveStrips = 2, bool packable = true);

    int reflectiveStrips() const;
    bool isPackable() const;

    double visibilityScore() const;
    double activitySuitability() const;
    std::string describe() const override;

private:
    int reflectiveStrips_;
    bool packable_;
};

/** @brief Footwear base; virtual inheritance from Garment. */
class Footwear : public virtual Garment {
public:
    Footwear() = default;
    Footwear(const GarmentSpec& spec, double heelHeightCm = 1.5,
             std::string soleMaterial = "rubber",
             ClosureType closure = ClosureType::Lace);

    double heelHeightCm() const;
    const std::string& soleMaterial() const;
    ClosureType closure() const;

    bool isHeeled() const;
    double cushioningScore() const;
    int breakInDays() const;

protected:
    double heelHeightCm_;
    std::string soleMaterial_;
    ClosureType closure_;
};

/** @brief Shoe derived from Footwear. */
class Shoe : public Footwear {
public:
    Shoe() = default;
    Shoe(const GarmentSpec& spec, double shoeWidth = 10.0,
         std::string lining = "textile", std::string laceType = "flat");

    double shoeWidth() const;
    const std::string& lining() const;
    const std::string& laceType() const;

    bool isLaceUp() const;
    std::string widthCategory() const;
    std::string polish() const;
    std::string describe() const override;

protected:
    double shoeWidth_;
    std::string lining_;
    std::string laceType_;
};

/** @brief Boot derived from Shoe. */
class Boot : public Shoe {
public:
    Boot() = default;
    Boot(const GarmentSpec& spec, double shaftHeightCm = 15.0,
         bool weatherproof = true, int treadDepth = 6);

    double shaftHeightCm() const;
    bool weatherproof() const;
    int treadDepth() const;

    bool ankleSupport() const;
    bool isWinterReady() const;
    double waterproofScore() const;

private:
    double shaftHeightCm_;
    bool weatherproof_;
    int treadDepth_;
};

/** @brief Sneaker derived from Shoe. */
class Sneaker : public Shoe {
public:
    Sneaker() = default;
    Sneaker(const GarmentSpec& spec, std::string cushioningUnit = "foam",
            std::string outsolePattern = "flat", double dropMm = 8.0);

    const std::string& cushioningUnit() const;
    const std::string& outsolePattern() const;
    double dropMm() const;

    double runningScore() const;
    bool isTrailReady() const;
    double impactAbsorption() const;

private:
    std::string cushioningUnit_;
    std::string outsolePattern_;
    double dropMm_;
};

/** @brief Sandal derived from Shoe. */
class Sandal : public Shoe {
public:
    Sandal() = default;
    Sandal(const GarmentSpec& spec, int strapCount = 2, bool openToe = true,
           bool archSupport = true);

    int strapCount() const;
    bool openToe() const;
    bool archSupport() const;

    bool isBreathable() const;
    bool summerFriendly() const;
    int adjustmentCount() const;

private:
    int strapCount_;
    bool openToe_;
    bool archSupport_;
};

/** @brief Sport sneaker via multiple inheritance. */
class SportSneaker : public Sneaker, public Sportswear {
public:
    SportSneaker() = default;
    SportSneaker(const GarmentSpec& spec, ActivityType activity = ActivityType::Running,
                 bool carbonPlate = true, std::string terrainType = "road");

    bool carbonPlate() const;
    const std::string& terrainType() const;

    double raceReadiness() const;
    std::string sportProfile() const;
    double energyReturn() const;
    std::string describe() const override;

private:
    bool carbonPlate_;
    std::string terrainType_;
};

/** @brief Headwear base; virtual inheritance from Garment. */
class Headwear : public virtual Garment {
public:
    Headwear() = default;
    Headwear(const GarmentSpec& spec, double circumferenceCm = 57.0,
             double brimWidthCm = 0.0, bool adjustable = true);

    double circumferenceCm() const;
    double brimWidthCm() const;
    bool adjustable() const;

    bool fitsHead(double headCircumferenceCm) const;
    bool isAdjustable() const;
    double sunProtectionFactor() const;

protected:
    double circumferenceCm_;
    double brimWidthCm_;
    bool adjustable_;
};

/** @brief Hat derived from Headwear. */
class Hat : public Headwear {
public:
    Hat() = default;
    Hat(const GarmentSpec& spec, double crownHeightCm = 12.0,
        std::string materialType = "felt", std::string brimStyle = "wide");

    double crownHeightCm() const;
    const std::string& materialType() const;
    const std::string& brimStyle() const;

    bool isFedora() const;
    double shapeRating() const;
    double packability() const;

private:
    double crownHeightCm_;
    std::string materialType_;
    std::string brimStyle_;
};

/** @brief Cap derived from Hat. */
class Cap : public Hat {
public:
    Cap() = default;
    Cap(const GarmentSpec& spec, std::string visorStyle = "curved",
        bool snapback = true, std::string teamLogo = "");

    const std::string& visorStyle() const;
    bool snapback() const;
    const std::string& teamLogo() const;

    bool isSnapback() const;
    bool hasTeamLogo() const;
    double casualScore() const;

private:
    std::string visorStyle_;
    bool snapback_;
    std::string teamLogo_;
};

/** @brief Beanie derived from Hat. */
class Beanie : public Hat {
public:
    Beanie() = default;
    Beanie(const GarmentSpec& spec, double foldHeightCm = 5.0,
           bool pomPom = false, std::string knitPattern = "rib");

    double foldHeightCm() const;
    bool hasPomPom() const;
    const std::string& knitPattern() const;

    bool isSlouchy() const;
    double warmthRating() const;
    bool canFold() const;

private:
    double foldHeightCm_;
    bool pomPom_;
    std::string knitPattern_;
};

/** @brief Neckwear base; virtual inheritance from Garment. */
class Neckwear : public virtual Garment {
public:
    Neckwear() = default;
    Neckwear(const GarmentSpec& spec, double lengthCm = 180.0,
             double widthCm = 30.0, std::string style = "classic");

    double lengthCm() const;
    double widthCm() const;
    const std::string& style() const;

    int wrapCount() const;
    double warmthRating() const;
    bool isInfinity() const;

protected:
    double lengthCm_;
    double widthCm_;
    std::string style_;
};

/** @brief Scarf derived from Neckwear. */
class Scarf : public Neckwear {
public:
    Scarf() = default;
    Scarf(const GarmentSpec& spec, double fringeLength = 5.0,
          std::string pattern = "plaid");

    double fringeLength() const;
    const std::string& pattern() const;

    double fringeDensity() const;
    std::string knotStyle() const;
    double windProtection() const;

private:
    double fringeLength_;
    std::string pattern_;
};

/** @brief Accessory base; virtual inheritance from Garment. */
class Accessory : public virtual Garment {
public:
    Accessory() = default;
    Accessory(const GarmentSpec& spec, std::string purpose = "decorative",
              bool adjustable = true, std::string metallicFinish = "matte");

    const std::string& purpose() const;
    bool adjustable() const;
    const std::string& metallicFinish() const;

    bool isDecorative() const;
    double adjustabilityScore() const;
    bool matchesColor(const Color& other) const;

protected:
    std::string purpose_;
    bool adjustable_;
    std::string metallicFinish_;
};

/** @brief Belt derived from Accessory. */
class Belt : public Accessory {
public:
    Belt() = default;
    Belt(const GarmentSpec& spec, std::string buckleType = "pin",
         double widthCm = 3.5, int holeCount = 5);

    const std::string& buckleType() const;
    double widthCm() const;
    int holeCount() const;

    bool isReversible() const;
    bool fitsWaist(double waistCm) const;
    double buckleShine() const;

private:
    std::string buckleType_;
    double widthCm_;
    int holeCount_;
};

/** @brief Gloves derived from Accessory. */
class Gloves : public Accessory {
public:
    Gloves() = default;
    Gloves(const GarmentSpec& spec, bool fingerless = false,
           bool touchscreenCompatible = true, std::string lining = "fleece");

    bool fingerless() const;
    bool touchscreenCompatible() const;
    const std::string& lining() const;

    bool isWinterReady() const;
    double touchAccuracy() const;
    double dexterityScore() const;

private:
    bool fingerless_;
    bool touchscreenCompatible_;
    std::string lining_;
};

/** @brief Socks derived from Accessory. */
class Socks : public Accessory {
public:
    Socks() = default;
    Socks(const GarmentSpec& spec, double crewHeight = 10.0,
          int cushioning = 2, bool moistureControl = true);

    double crewHeight() const;
    int cushioning() const;
    bool moistureControl() const;

    bool isAthletic() const;
    double warmthRating() const;
    double blisterRisk() const;

private:
    double crewHeight_;
    int cushioning_;
    bool moistureControl_;
};

/** @brief Bag derived from Accessory. */
class Bag : public Accessory {
public:
    Bag() = default;
    Bag(const GarmentSpec& spec, double capacityLiters = 20.0,
        std::string strapType = "shoulder", int compartments = 3);

    double capacityLiters() const;
    const std::string& strapType() const;
    int compartments() const;

    bool isCarryOn() const;
    std::string organize() const;
    double loadScore() const;

private:
    double capacityLiters_;
    std::string strapType_;
    int compartments_;
};

/** @brief Inventory with protected inheritance from Identifiable. */
class Inventory : protected Identifiable {
public:
    Inventory();
    Inventory(std::string location, int initialUnits);

    std::string location() const;
    void addStock(const std::string& sku, int quantity);
    void removeStock(const std::string& sku, int quantity);
    int stockOf(const std::string& sku) const;
    bool isLowStock(const std::string& sku, int threshold) const;
    int totalUnits() const;
    std::vector<std::string> listSkus() const;

protected:
    std::map<std::string, int> stock_;
    std::string location_;
};

/** @brief Warehouse extends Inventory, keeping protected inheritance. */
class Warehouse : public Inventory {
public:
    Warehouse();
    Warehouse(std::string location, int capacity, std::string address,
              bool temperatureControlled);

    int capacity() const;
    const std::string& address() const;
    bool temperatureControlled() const;

    bool canStore(const Garment& garment) const;
    double utilizationPercent() const;
    bool requiresHeating() const;

private:
    int capacity_;
    std::string address_;
    bool temperatureControlled_;
};

/** @brief Outfit groups a complete set of garments. */
class Outfit {
public:
    Outfit();
    Outfit(std::shared_ptr<Top> top, std::shared_ptr<Bottom> bottom,
           std::shared_ptr<Outerwear> outer,
           std::shared_ptr<Footwear> footwear,
           std::shared_ptr<Accessory> accessory);

    std::shared_ptr<Top> top() const;
    std::shared_ptr<Bottom> bottom() const;
    std::shared_ptr<Outerwear> outer() const;
    std::shared_ptr<Footwear> footwear() const;
    std::shared_ptr<Accessory> accessory() const;

    bool isComplete() const;
    double totalPrice() const;
    bool matchesPalette() const;
    double formalityScore() const;
    std::string describe() const;
    void addLayer(std::shared_ptr<Garment> garment);

private:
    std::shared_ptr<Top> top_;
    std::shared_ptr<Bottom> bottom_;
    std::shared_ptr<Outerwear> outer_;
    std::shared_ptr<Footwear> footwear_;
    std::shared_ptr<Accessory> accessory_;
};

/** @brief Wardrobe with private inheritance from Identifiable. */
class Wardrobe : private Identifiable {
public:
    Wardrobe();
    Wardrobe(std::string ownerName, int capacity);

    const std::string& ownerName() const;
    int capacity() const;

    void addGarment(std::shared_ptr<Garment> garment);
    void removeGarment(const std::string& id);
    std::vector<std::shared_ptr<Garment>> findByColor(const Color& color) const;
    double totalValue() const;
    int countBySeason(Season season) const;
    Outfit recommendOutfit() const;
    void emptyWardrobe();
    std::string wardrobeCode() const;

private:
    std::string ownerName_;
    std::vector<std::shared_ptr<Garment>> garments_;
    int capacity_;
    mutable std::shared_ptr<Outfit> lastOutfit_;
};

/** @brief Size conversion table between sizing systems. */
class SizeChart {
public:
    SizeChart();
    SizeChart(std::string region, std::map<std::string, std::string> mappings);

    const std::string& region() const;
    std::string convert(const std::string& label) const;
    bool knowsLabel(const std::string& label) const;
    void addMapping(const std::string& from, const std::string& to);
    std::string describe() const;

private:
    std::string region_;
    std::map<std::string, std::string> mappings_;
};

/** @brief Laundry service that processes garments. */
class LaundryService {
public:
    LaundryService();
    LaundryService(std::string detergent, int washTemperature,
                   bool dryCleaningAvailable, double capacityKg);

    const std::string& detergent() const;
    int washTemperature() const;
    bool dryCleaningAvailable() const;
    double capacityKg() const;

    bool accepts(const Garment& garment) const;
    std::string recommendedProgram(const Material& material) const;
    int estimateTime(const Garment& garment) const;
    double costFor(const std::vector<std::shared_ptr<Garment>>& garments) const;
    bool isEcoFriendly() const;

private:
    std::string detergent_;
    int washTemperature_;
    bool dryCleaningAvailable_;
    double capacityKg_;
    CareInstructions carePolicy_;
};

/** @brief Discount policy applied to garments or price tags. */
class DiscountPolicy {
public:
    DiscountPolicy();
    DiscountPolicy(double percent, double minPurchase, bool seasonalOnly,
                   std::string code, Season targetSeason);

    double percent() const;
    double minPurchase() const;
    bool seasonalOnly() const;
    const std::string& code() const;
    Season targetSeason() const;

    bool appliesTo(const Garment& garment) const;
    PriceTag apply(const PriceTag& tag) const;
    bool isActive(Season season) const;
    std::string describe() const;

private:
    double percent_;
    double minPurchase_;
    bool seasonalOnly_;
    std::string code_;
    Season targetSeason_;
};

/** @brief Store that sells garments and manages inventory. */
class ClothingStore {
public:
    ClothingStore();
    ClothingStore(std::string name, Inventory inventory,
                  DiscountPolicy policy, LaundryService laundry,
                  SizeChart sizeChart);

    const std::string& name() const;
    Inventory& inventory();
    const DiscountPolicy& policy() const;
    const LaundryService& laundry() const;
    const SizeChart& sizeChart() const;

    void addToCatalog(std::shared_ptr<Garment> garment);
    double sell(const std::shared_ptr<Garment>& garment, int quantity);
    void restockSku(const std::string& sku, int quantity);
    double projectedRevenue() const;
    std::vector<std::shared_ptr<Garment>> findAffordable(double budget) const;
    std::vector<std::shared_ptr<Garment>> seasonalClearance(Season season) const;
    std::string describe() const;

private:
    std::string name_;
    Inventory inventory_;
    DiscountPolicy policy_;
    LaundryService laundry_;
    SizeChart sizeChart_;
    std::vector<std::shared_ptr<Garment>> catalog_;
};

} // namespace clothing

#endif // CLOTHING_CLOTHING_H
