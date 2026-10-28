# Clothing Domain Model

A C++17 object model of the "Clothing" subject area. The project implements a
large class hierarchy for materials, garments, outfits, wardrobes, a store, a
laundry service, inventory, discounts, and a dedicated exception hierarchy. It
demonstrates public, protected, and private inheritance, virtual and multiple
inheritance, virtual methods, and name resolution with a `using` directive.

## Requirements

- CMake 3.16 or newer
- A C++17 compatible compiler
- No third-party libraries are required

## Build

From this directory:

```sh
cmake -S . -B build
cmake --build build
```

This builds:

- `clothing_model` — the domain library
- `clothing_app` — a demonstration program
- `clothing_tests` — a self-contained test executable

## Run the demonstration

```sh
./build/clothing_app
```

## Run the tests

```sh
ctest --test-dir build --output-on-failure
```

## Requirement coverage

| Requirement | Required | Implemented |
|---|---:|---:|
| Classes | 50 | 61 domain classes + 15 exception classes = 76 |
| Fields | 150 | 189 |
| Unique domain behaviors | 100 | 267 |
| Associations | 30 | 44 |
| Custom exceptions | 12 | 15 |

The behavior count excludes constructors, trivial getters/setters, operators,
and the `errorCode()` overrides of exceptions. Every behavior listed below is a
domain operation with logic (validation, calculation, selection, conversion,
state transition, compatibility check, and so on).

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Public inheritance | `Fabric : public Material`, `CottonFabric : public WovenFabric`, `WoolFabric : public KnitFabric`, `Top : public virtual Garment` |
| Protected inheritance | `Inventory : protected Identifiable`, then `Warehouse : public Inventory` |
| Private inheritance | `Wardrobe : private Identifiable` |
| Virtual methods | `Material::recommendedWashTemperature`, `Material::suitableFor`, `Material::stretchPercent`, `Material::requiresDryCleaning`, `Material::describe`; `Garment::describe`, `suitabilityScore`, `isWaterproof`, `cleanMethod`, `foldable`; `Fabric::shrinkagePercent` |
| Multiple inheritance | `Dress : public Top, public Bottom`; `SportJacket : public Jacket, public Sportswear`; `SportSneaker : public Sneaker, public Sportswear` |
| Virtual inheritance | `Top`, `Bottom`, `Outerwear`, `Footwear`, `Headwear`, `Neckwear`, `Accessory` all inherit `public virtual Garment`; this resolves the diamond in `Dress` |
| `using` directive (name resolution) | `CareInstructions::recommendedCycle`, `CareInstructions::isSafeFor`, and `LaundryService::recommendedProgram` contain `using namespace garment_care;` and call `cycleName` / `recommendedTemperature` unqualified |
| Namespace-qualified names | `textile::Fiber` is used explicitly in every material |

## Namespaces

- `clothing` — all domain classes and exceptions
- `clothing::textile` — textile vocabulary (`Fiber`)
- `clothing::garment_care` — care vocabulary used by the `using` directive
  (`recommendedTemperature`, `cycleName`)

---

## Exception hierarchy

All exceptions derive from `clothing::ClothingException`, which derives from
`std::runtime_error`.

| Exception | `errorCode()` | Meaning |
|---|---|---|
| `ClothingException` | `CLOTHING-GENERAL` | Root of the hierarchy |
| `InvalidSizeException` | `CLOTHING-SIZE` | Invalid size label/system/table lookup |
| `InvalidPriceException` | `CLOTHING-PRICE` | Invalid amount, discount, tax, or quantity |
| `OutOfStockException` | `CLOTHING-STOCK` | Sale exceeds available stock |
| `IncompatibleMaterialException` | `CLOTHING-MATERIAL` | Materials cannot be combined |
| `CareInstructionException` | `CLOTHING-CARE` | Invalid or contradictory care label |
| `WardrobeFullException` | `CLOTHING-WARDROBE-FULL` | Wardrobe capacity reached |
| `GarmentNotFoundException` | `CLOTHING-NOT-FOUND` | Garment or outfit component missing |
| `InvalidColorException` | `CLOTHING-COLOR` | RGB component or name invalid |
| `SizeMismatchException` | `CLOTHING-SIZE-MISMATCH` | Sizes from different systems compared directly |
| `InvalidMeasurementException` | `CLOTHING-MEASUREMENT` | Measurement out of physical bounds or wrong type |
| `DuplicateGarmentException` | `CLOTHING-DUPLICATE` | Same garment added twice |
| `InsufficientStockException` | `CLOTHING-STOCK-LOW` | Inventory cannot remove requested units |
| `InvalidSeasonException` | `CLOTHING-SEASON` | Text cannot be parsed as a season |
| `UnsupportedOperationException` | `CLOTHING-UNSUPPORTED` | Operation undefined for this object/state |

Each exception class inherits the constructors of `ClothingException` with a
`using` declaration and overrides only `errorCode()`.

---

## Core value and support classes

### Measurement
- Fields: `centimeters_`, `type_`
- Methods: `Measurement()`, `Measurement(double, MeasurementType)`, `centimeters()`, `setCentimeters()`, `type()`, `toInches()`, `add()`, `isWithinRange()`, `ratioTo()`, `validate()`
- Behavior: converts centimeters to inches, adds measurements of the same type, checks ranges, computes ratios, and validates the physical interval `(0, 300]` cm.

### Color
- Fields: `red_`, `green_`, `blue_`, `name_`
- Methods: `Color()`, `Color(int, int, int, std::string)`, `red()`, `green()`, `blue()`, `name()`, `toHex()`, `mix()`, `lighten()`, `darken()`, `contrastWith()`, `isNeutral()`, `validate()`, `operator==()`, `operator!=()`
- Behavior: RGB validation, hexadecimal formatting, channel mixing, lightening/darkening, relative-luminance contrast, and neutral-color detection.

### Size
- Fields: `label_`, `system_`, `chest_`, `waist_`, `hip_`
- Methods: `Size()`, `Size(std::string, SizeSystem, Measurement, Measurement, Measurement)`, `label()`, `system()`, `chest()`, `waist()`, `hip()`, `fits()`, `isCompatibleWith()`, `convertedTo()`, `numericLabel()`, `validate()`, `ensureSameSystem()`
- Behavior: checks body-chest fit with tolerance, compares sizes of the same system, converts numeric labels between sizing systems, parses alphanumeric labels, and validates chest/waist/hip ordering.

### Identifiable
- Fields: `id_`, `createdAt_`
- Methods: `Identifiable()`, `Identifiable(std::string, long)`, `id()`, `createdAt()`, `regenerateId()`, `isSame()`, `identitySummary()`
- Behavior: generates unique IDs, restarts identity, compares identities, and produces an identity summary. This class is inherited publicly, protectedly, and privately elsewhere.

### PriceTag
- Fields: `amount_`, `currency_`, `discountPercent_`, `taxPercent_`
- Methods: `PriceTag()`, `PriceTag(double, Currency, double, double)`, `amount()`, `currency()`, `discountPercent()`, `taxPercent()`, `finalPrice()`, `applyDiscount()`, `convertTo()`, `isAffordable()`, `taxAmount()`, `validate()`
- Behavior: computes the final price with discount and tax, adds a discount immutably, converts between currencies, checks a budget, computes the tax amount, and validates all percentages.

### CareInstructions
- Fields: `washTemperature_`, `canTumbleDry_`, `canBleach_`, `ironTemperature_`, `dryCleanOnly_`
- Methods: `CareInstructions()`, `CareInstructions(int, bool, bool, int, bool)`, `washTemperature()`, `canTumbleDry()`, `canBleach()`, `ironTemperature()`, `dryCleanOnly()`, `merge()`, `recommendedCycle()`, `isSafeFor()`, `summary()`, `isGentle()`, `validate()`
- Behavior: merges two labels by taking the stricter rules, resolves a cycle name through the `using` directive, checks safety for a material, summarizes the label, detects gentle care, and rejects contradictory labels.

### Manufacturer
- Fields: `name_`, `country_`, `contactEmail_`, `foundedYear_`, `ethicalRating_`
- Methods: `Manufacturer()`, `Manufacturer(std::string, std::string, std::string, int, double)`, `name()`, `country()`, `contactEmail()`, `foundedYear()`, `ethicalRating()`, `isEuropean()`, `reputationScore()`, `contactInfo()`, `yearsInBusiness()`
- Behavior: recognizes European countries, computes a reputation score from ethics and age, formats contact data, and computes years in business.

---

## Material hierarchy

### Material (abstract)
- Fields: `name_`, `fiber_`, `weightGsm_`, `breathability_`, `waterproofRating_`
- Methods: virtual destructor, `name()`, `fiber()`, `weightGsm()`, `breathability()`, `waterproofRating()`, pure virtual `recommendedWashTemperature()`, `suitableFor()`, `stretchPercent()`, virtual `requiresDryCleaning()`, virtual `describe()`, `warmthIndex()`, `isLightweight()`, `isWaterproof()`
- Behavior: warmth index, lightweight test, waterproof test, and polymorphic care/suitability/stretch behavior.

### Fabric : public Material
- Fields: `weave_`, `widthCm_`
- Methods: constructor, `weave()`, `widthCm()`, `areaWeightFor()`, `canBeDyed()`, virtual `shrinkagePercent()`, `isWashable()`, `recommendedWashTemperature()` (override), `suitableFor()` (override), `stretchPercent()` (override), `describe()` (override)
- Behavior: fabric weight per running meter, dye suitability, fiber-specific shrinkage, washability, season suitability by fabric weight.

### KnitFabric : public Fabric
- Fields: `stitchDensity_`, `elasticity_`
- Methods: constructor, `stretchPercent()` (override), `stretchesTo()`, `recoversAfterWash()`, `recommendNeedleSize()`
- Behavior: stretched length, elastic recovery, needle-size recommendation.

### WovenFabric : public Fabric
- Fields: `threadCount_`, `warpAligned_`
- Methods: constructor, `densityRating()`, `isFrayResistant()`, `recommendedIronTemperature()`, `shrinkagePercent()` (override)
- Behavior: density rating, fray resistance, fiber-specific ironing temperature, warp-aware shrinkage.

### CottonFabric : public WovenFabric
- Fields: `organic_`, `originRegion_`
- Methods: constructor, `isOrganic()`, `originRegion()`, `absorbencyRate()`, `isHypoallergenic()`, `dyeAffinity()`, `suitableFor()` (override), `recommendedWashTemperature()` (override)
- Behavior: absorbency, organic allergy safety, dye affinity, summer suitability, organic wash temperature.

### WoolFabric : public KnitFabric
- Fields: `micronCount_`, `merino_`
- Methods: constructor, `micronCount()`, `isMerino()`, `warmthRating()`, `feltsInWash()`, `allergenRisk()`, `recommendedWashTemperature()` (override), `requiresDryCleaning()` (override), `suitableFor()` (override)
- Behavior: warmth rating, felting risk, allergen risk by micron count, merino wash rules, dry-cleaning rule, autumn/winter suitability.

### SilkFabric : public WovenFabric
- Fields: `momme_`, `sheen_`
- Methods: constructor, `momme()`, `sheen()`, `lusterScore()`, `needsDelicateWash()`, `drapesWell()`, `recommendedWashTemperature()` (override), `requiresDryCleaning()` (override), `suitableFor()` (override)
- Behavior: luster score, delicate-wash detection, drape test, dry-clean rule, seasonal suitability.

### DenimFabric : public WovenFabric
- Fields: `ounceWeight_`, `stretchLimit_`
- Methods: constructor, `ounceWeight()`, `stretchLimit()`, `fadeAfterWashes()`, `isRigid()`, `recommendSoakHours()`, `stretchPercent()` (override), `recommendedWashTemperature()` (override), `suitableFor()` (override)
- Behavior: fade prediction, rigidity test, soak-time recommendation, stretch, wash temperature, seasonal suitability.

### Leather : public Material
- Fields: `animal_`, `thicknessMm_`, `tannery_`
- Methods: constructor, `animal()`, `thicknessMm()`, `tannery()`, `breakInHours()`, `needsConditioning()`, `waterResistanceScore()`, `recommendedWashTemperature()` (override), `suitableFor()` (override), `stretchPercent()` (override), `requiresDryCleaning()` (override), `describe()` (override)
- Behavior: break-in time, conditioning requirement, water resistance, dry-clean rule, seasonal suitability, descriptive text.

### SyntheticFabric : public Fabric
- Fields: `polymer_`, `recycledPercent_`
- Methods: constructor, `polymer()`, `recycledPercent()`, `meltRisk()`, `uvResistance()`, `microplasticShedding()`, `recommendedWashTemperature()` (override), `suitableFor()` (override)
- Behavior: melt risk by polymer, UV resistance, microplastic shedding, low-temperature care, seasonal suitability.

---

## Garment hierarchy

### Garment : public Identifiable
- Fields: `name_`, `brand_`, `size_`, `color_`, `material_`, `price_`, `care_`, `manufacturer_`, `season_`, `stockCount_`
- Methods: `Garment()`, virtual destructor, `name()`, `brand()`, `size()`, `color()`, `material()`, `price()`, `care()`, `manufacturer()`, `season()`, `inStock()`, `stockCount()`, protected `initialize()`, virtual `describe()`, `suitabilityScore()`, `isWaterproof()`, `cleanMethod()`, `foldable()`, `totalPrice()`, `isAvailable()`, `restock()`, `markSold()`, `applyDiscount()`, `isCompatibleWith()`, `careSummary()`, `isSameSeason()`, static `parseSeason()`
- Behavior: builds a garment from `GarmentSpec`, computes season suitability, checks waterproofing, chooses a cleaning method, decides foldability, computes final price, restocks, validates sales, applies a discount, checks compatibility with another garment, formats care information, parses season names from text.

### Top : public virtual Garment
- Fields: `sleeveLength_`, `neckline_`, `fit_`
- Methods: `Top()`, `Top(const GarmentSpec&, SleeveLength, Neckline, FitType)`, `sleeveLength()`, `neckline()`, `fit()`, `coversArms()`, `isFormal()`, `layerCount()`, `matchesBottom()`, `describe()` (override)
- Behavior: arm coverage, formality test, layering count, bottom compatibility, description.

### Bottom : public virtual Garment
- Fields: `waistRise_`, `legShape_`, `lengthCm_`
- Methods: `Bottom()`, `Bottom(const GarmentSpec&, WaistRise, LegShape, double)`, `waistRise()`, `legShape()`, `lengthCm()`, `isLong()`, `suitsFootwear()`, `waistToHipRatio()`, `describe()` (override)
- Behavior: long/short classification, footwear compatibility, waist-to-hip ratio, description.

### Dress : public Top, public Bottom
- Fields: `hasTrain_`, `dressStyle_`
- Methods: `Dress()`, `Dress(const GarmentSpec&, bool, std::string)`, `hasTrain()`, `dressStyle()`, `isEveningWear()`, `silhouette()`, `requiresSpecialCare()`, `describe()` (override), `cleanMethod()` (override)
- Behavior: evening-wear detection, silhouette selection from fit/leg shape, special-care check, combined description and cleaning rule. The diamond is resolved because `Top` and `Bottom` inherit `Garment` virtually.

### Shirt : public Top
- Fields: `collar_`, `buttonCount_`, `cuff_`
- Methods: `Shirt()`, `Shirt(const GarmentSpec&, std::string, int, std::string)`, `collar()`, `buttonCount()`, `cuff()`, `isButtonedUp()`, `rollSleeves()`, `formalRating()`, `describe()` (override)
- Behavior: button test, sleeve-roll instruction, formality score, description.

### TShirt : public Shirt
- Fields: `printText_`, `graphicTheme_`
- Methods: `TShirt()`, `TShirt(const GarmentSpec&, std::string, std::string)`, `printText()`, `graphicTheme()`, `isGraphic()`, `sloganLength()`, `casualRating()`, `describe()` (override)
- Behavior: graphic detection, slogan length, casualness score, description.

### Blouse : public Top
- Fields: `sheen_`, `tieOption_`
- Methods: `Blouse()`, `Blouse(const GarmentSpec&, double, bool)`, `sheen()`, `hasTie()`, `isElegant()`, `canTuck()`, `drapeScore()`
- Behavior: elegance test, tuckability, drape score.

### Sweater : public Top
- Fields: `knitGauge_`, `collar_`, `woolContent_`
- Methods: `Sweater()`, `Sweater(const GarmentSpec&, int, std::string, int)`, `knitGauge()`, `collar()`, `woolContent()`, `warmthLevel()`, `isHandWashOnly()`, `pillRisk()`
- Behavior: warmth level, hand-wash rule, pilling risk.

### Hoodie : public Sweater
- Fields: `hasKangarooPocket_`, `drawstringLength_`
- Methods: `Hoodie()`, `Hoodie(const GarmentSpec&, bool, double)`, `hasKangarooPocket()`, `drawstringLength()`, `pocketCapacity()`, `toggleDrawstring()`, `streetStyleScore()`
- Behavior: pocket capacity, drawstring toggle, street-style score.

### Pants : public Bottom
- Fields: `closure_`, `pocketCount_`, `pleated_`
- Methods: `Pants()`, `Pants(const GarmentSpec&, ClosureType, int, bool)`, `closure()`, `pocketCount()`, `isPleated()`, `hasPockets()`, `canRollCuffs()`, `formalityScore()`
- Behavior: pocket test, cuff-roll rule, formality score.

### Jeans : public Pants
- Fields: `washType_`, `rivetCount_`, `selvedge_`
- Methods: `Jeans()`, `Jeans(const GarmentSpec&, std::string, int, bool)`, `washType()`, `rivetCount()`, `isSelvedge()`, `fadeLevel()`, `recommendWashInterval()`
- Behavior: fade level by wash type, wash interval recommendation.

### Shorts : public Bottom
- Fields: `inseamCm_`, `cuffed_`
- Methods: `Shorts()`, `Shorts(const GarmentSpec&, double, bool)`, `inseamCm()`, `isCuffed()`, `isSporty()`, `canConvertToLong()`, `summerSuitability()`
- Behavior: sporty classification, conversion support, summer suitability.

### Skirt : public Bottom
- Fields: `flare_`, `slitHeight_`, `waistband_`
- Methods: `Skirt()`, `Skirt(const GarmentSpec&, double, double, std::string)`, `flare()`, `slitHeight()`, `waistband()`, `twirlRadius()`, `isPencil()`, `occasionRating()`
- Behavior: twirl radius, pencil-skirt test, occasion rating.

### Outerwear : public virtual Garment
- Fields: `lining_`, `hood_`, `closure_`
- Methods: `Outerwear()`, `Outerwear(const GarmentSpec&, std::string, bool, ClosureType)`, `lining()`, `hasHood()`, `closure()`, `isLayered()`, `warmthIndex()`, `protectsFromRain()`
- Behavior: layered test, warmth index, rain protection.

### Jacket : public Outerwear
- Fields: `lapel_`, `pocketCount_`, `quilted_`
- Methods: `Jacket()`, `Jacket(const GarmentSpec&, std::string, int, bool)`, `lapel()`, `pocketCount()`, `isQuilted()`, `isBikerStyle()`, `zipUp()`, `layerOver()`, `describe()` (override)
- Behavior: biker-style detection, zipper check, layering compatibility, description.

### Coat : public Outerwear
- Fields: `lengthCm_`, `beltIncluded_`, `trenchStyle_`
- Methods: `Coat()`, `Coat(const GarmentSpec&, double, bool, bool)`, `lengthCm()`, `beltIncluded()`, `isTrench()`, `coversKnees()`, `formalityRating()`
- Behavior: knee coverage, formality rating.

### Raincoat : public Coat
- Fields: `waterproofRating_`, `sealedSeams_`
- Methods: `Raincoat()`, `Raincoat(const GarmentSpec&, int, bool)`, `waterproofRating()`, `sealedSeams()`, `rainProtectionScore()`, `isSeamSealed()`, `packability()`
- Behavior: rain protection score, seam-seal check, packability.

### Sportswear (mixin)
- Fields: `activity_`, `moistureWicking_`, `stretchFactor_`
- Methods: `Sportswear()`, `Sportswear(ActivityType, bool, double)`, `activity()`, `moistureWicking()`, `stretchFactor()`, `isHighImpact()`, `sweatManagementScore()`, `flexibilityRating()`, `describeSport()`
- Behavior: high-impact classification, sweat-management score, flexibility rating, sport description.

### SportJacket : public Jacket, public Sportswear
- Fields: `reflectiveStrips_`, `packable_`
- Methods: `SportJacket()`, `SportJacket(const GarmentSpec&, ActivityType, bool, double, int, bool)`, `reflectiveStrips()`, `isPackable()`, `visibilityScore()`, `activitySuitability()`, `describe()` (override)
- Behavior: visibility score, packability, activity suitability, combined description.

### Footwear : public virtual Garment
- Fields: `heelHeightCm_`, `soleMaterial_`, `closure_`
- Methods: `Footwear()`, `Footwear(const GarmentSpec&, double, std::string, ClosureType)`, `heelHeightCm()`, `soleMaterial()`, `closure()`, `isHeeled()`, `cushioningScore()`, `breakInDays()`
- Behavior: heel test, cushioning score by sole, break-in days.

### Shoe : public Footwear
- Fields: `shoeWidth_`, `lining_`, `laceType_`
- Methods: `Shoe()`, `Shoe(const GarmentSpec&, double, std::string, std::string)`, `shoeWidth()`, `lining()`, `laceType()`, `isLaceUp()`, `widthCategory()`, `polish()`, `describe()` (override)
- Behavior: lace-up test, width category, polishing instruction, description.

### Boot : public Shoe
- Fields: `shaftHeightCm_`, `weatherproof_`, `treadDepth_`
- Methods: `Boot()`, `Boot(const GarmentSpec&, double, bool, int)`, `shaftHeightCm()`, `weatherproof()`, `treadDepth()`, `ankleSupport()`, `isWinterReady()`, `waterproofScore()`
- Behavior: ankle support, winter readiness, waterproof score.

### Sneaker : public Shoe
- Fields: `cushioningUnit_`, `outsolePattern_`, `dropMm_`
- Methods: `Sneaker()`, `Sneaker(const GarmentSpec&, std::string, std::string, double)`, `cushioningUnit()`, `outsolePattern()`, `dropMm()`, `runningScore()`, `isTrailReady()`, `impactAbsorption()`
- Behavior: running score, trail readiness, impact absorption.

### Sandal : public Shoe
- Fields: `strapCount_`, `openToe_`, `archSupport_`
- Methods: `Sandal()`, `Sandal(const GarmentSpec&, int, bool, bool)`, `strapCount()`, `openToe()`, `archSupport()`, `isBreathable()`, `summerFriendly()`, `adjustmentCount()`
- Behavior: breathability, summer suitability, adjustment count.

### SportSneaker : public Sneaker, public Sportswear
- Fields: `carbonPlate_`, `terrainType_`
- Methods: `SportSneaker()`, `SportSneaker(const GarmentSpec&, ActivityType, bool, std::string)`, `carbonPlate()`, `terrainType()`, `raceReadiness()`, `sportProfile()`, `energyReturn()`, `describe()` (override)
- Behavior: race readiness, sport profile, energy return, combined description.

### Headwear : public virtual Garment
- Fields: `circumferenceCm_`, `brimWidthCm_`, `adjustable_`
- Methods: `Headwear()`, `Headwear(const GarmentSpec&, double, double, bool)`, `circumferenceCm()`, `brimWidthCm()`, `adjustable()`, `fitsHead()`, `isAdjustable()`, `sunProtectionFactor()`
- Behavior: head-fit check, adjustability, sun protection factor.

### Hat : public Headwear
- Fields: `crownHeightCm_`, `materialType_`, `brimStyle_`
- Methods: `Hat()`, `Hat(const GarmentSpec&, double, std::string, std::string)`, `crownHeightCm()`, `materialType()`, `brimStyle()`, `isFedora()`, `shapeRating()`, `packability()`
- Behavior: fedora detection, shape rating, packability.

### Cap : public Hat
- Fields: `visorStyle_`, `snapback_`, `teamLogo_`
- Methods: `Cap()`, `Cap(const GarmentSpec&, std::string, bool, std::string)`, `visorStyle()`, `snapback()`, `teamLogo()`, `isSnapback()`, `hasTeamLogo()`, `casualScore()`
- Behavior: snapback check, logo check, casual score.

### Beanie : public Hat
- Fields: `foldHeightCm_`, `pomPom_`, `knitPattern_`
- Methods: `Beanie()`, `Beanie(const GarmentSpec&, double, bool, std::string)`, `foldHeightCm()`, `hasPomPom()`, `knitPattern()`, `isSlouchy()`, `warmthRating()`, `canFold()`
- Behavior: slouch detection, warmth rating, foldability.

### Neckwear : public virtual Garment
- Fields: `lengthCm_`, `widthCm_`, `style_`
- Methods: `Neckwear()`, `Neckwear(const GarmentSpec&, double, double, std::string)`, `lengthCm()`, `widthCm()`, `style()`, `wrapCount()`, `warmthRating()`, `isInfinity()`
- Behavior: wrap count, warmth rating, infinity-scarf detection.

### Scarf : public Neckwear
- Fields: `fringeLength_`, `pattern_`
- Methods: `Scarf()`, `Scarf(const GarmentSpec&, double, std::string)`, `fringeLength()`, `pattern()`, `fringeDensity()`, `knotStyle()`, `windProtection()`
- Behavior: fringe density, knot style, wind protection.

### Accessory : public virtual Garment
- Fields: `purpose_`, `adjustable_`, `metallicFinish_`
- Methods: `Accessory()`, `Accessory(const GarmentSpec&, std::string, bool, std::string)`, `purpose()`, `adjustable()`, `metallicFinish()`, `isDecorative()`, `adjustabilityScore()`, `matchesColor()`
- Behavior: decorative check, adjustability score, color matching.

### Belt : public Accessory
- Fields: `buckleType_`, `widthCm_`, `holeCount_`
- Methods: `Belt()`, `Belt(const GarmentSpec&, std::string, double, int)`, `buckleType()`, `widthCm()`, `holeCount()`, `isReversible()`, `fitsWaist()`, `buckleShine()`
- Behavior: reversibility, waist-fit calculation, buckle shine.

### Gloves : public Accessory
- Fields: `fingerless_`, `touchscreenCompatible_`, `lining_`
- Methods: `Gloves()`, `Gloves(const GarmentSpec&, bool, bool, std::string)`, `fingerless()`, `touchscreenCompatible()`, `lining()`, `isWinterReady()`, `touchAccuracy()`, `dexterityScore()`
- Behavior: winter readiness, touch accuracy, dexterity score.

### Socks : public Accessory
- Fields: `crewHeight_`, `cushioning_`, `moistureControl_`
- Methods: `Socks()`, `Socks(const GarmentSpec&, double, int, bool)`, `crewHeight()`, `cushioning()`, `moistureControl()`, `isAthletic()`, `warmthRating()`, `blisterRisk()`
- Behavior: athletic classification, warmth rating, blister risk.

### Bag : public Accessory
- Fields: `capacityLiters_`, `strapType_`, `compartments_`
- Methods: `Bag()`, `Bag(const GarmentSpec&, double, std::string, int)`, `capacityLiters()`, `strapType()`, `compartments()`, `isCarryOn()`, `organize()`, `loadScore()`
- Behavior: carry-on test, organization summary, load score.

---

## Retail and organization classes

### Inventory : protected Identifiable
- Fields: `stock_`, `location_`
- Methods: `Inventory()`, `Inventory(std::string, int)`, `location()`, `addStock()`, `removeStock()`, `stockOf()`, `isLowStock()`, `totalUnits()`, `listSkus()`
- Behavior: stock accumulation, checked stock removal, SKU lookup, low-stock detection, total count, SKU listing. Protected inheritance keeps `Identifiable` hidden from external callers but available to `Warehouse`.

### Warehouse : public Inventory
- Fields: `capacity_`, `address_`, `temperatureControlled_`
- Methods: `Warehouse()`, `Warehouse(std::string, int, std::string, bool)`, `capacity()`, `address()`, `temperatureControlled()`, `canStore()`, `utilizationPercent()`, `requiresHeating()`
- Behavior: storage eligibility (capacity and dry-clean temperature), utilization percentage, heating requirement.

### Outfit
- Fields: `top_`, `bottom_`, `outer_`, `footwear_`, `accessory_`
- Methods: `Outfit()`, `Outfit(shared_ptr<Top>, shared_ptr<Bottom>, shared_ptr<Outerwear>, shared_ptr<Footwear>, shared_ptr<Accessory>)`, all five accessors, `isComplete()`, `totalPrice()`, `matchesPalette()`, `formalityScore()`, `describe()`, `addLayer()`
- Behavior: completeness test, total price, palette compatibility, formality score, description, dynamic layer assignment.

### Wardrobe : private Identifiable
- Fields: `ownerName_`, `garments_`, `capacity_`, `lastOutfit_`
- Methods: `Wardrobe()`, `Wardrobe(std::string, int)`, `ownerName()`, `capacity()`, `addGarment()`, `removeGarment()`, `findByColor()`, `totalValue()`, `countBySeason()`, `recommendOutfit()`, `emptyWardrobe()`, `wardrobeCode()`
- Behavior: duplicate detection, capacity enforcement, removal by ID, color search, total value, season count, automatic outfit recommendation, emptying, identity exposure through `wardrobeCode()`. Private inheritance hides the `Identifiable` interface from callers.

### SizeChart
- Fields: `region_`, `mappings_`
- Methods: `SizeChart()`, `SizeChart(std::string, std::map<std::string, std::string>)`, `region()`, `convert()`, `knowsLabel()`, `addMapping()`, `describe()`
- Behavior: label conversion, label lookup, validated mapping insertion, description.

### LaundryService
- Fields: `detergent_`, `washTemperature_`, `dryCleaningAvailable_`, `capacityKg_`, `carePolicy_`
- Methods: `LaundryService()`, `LaundryService(std::string, int, bool, double)`, `detergent()`, `washTemperature()`, `dryCleaningAvailable()`, `capacityKg()`, `accepts()`, `recommendedProgram()`, `estimateTime()`, `costFor()`, `isEcoFriendly()`
- Behavior: garment acceptance, program selection with the `using` directive, processing-time estimation, batch cost, eco-friendliness.

### DiscountPolicy
- Fields: `percent_`, `minPurchase_`, `seasonalOnly_`, `code_`, `targetSeason_`
- Methods: `DiscountPolicy()`, `DiscountPolicy(double, double, bool, std::string, Season)`, `percent()`, `minPurchase()`, `seasonalOnly()`, `code()`, `targetSeason()`, `appliesTo()`, `apply()`, `isActive()`, `describe()`
- Behavior: garment eligibility, price-tag discount application, season activation, description.

### ClothingStore
- Fields: `name_`, `inventory_`, `policy_`, `laundry_`, `sizeChart_`, `catalog_`
- Methods: `ClothingStore()`, `ClothingStore(std::string, Inventory, DiscountPolicy, LaundryService, SizeChart)`, `name()`, `inventory()`, `policy()`, `laundry()`, `sizeChart()`, `addToCatalog()`, `sell()`, `restockSku()`, `projectedRevenue()`, `findAffordable()`, `seasonalClearance()`, `describe()`
- Behavior: catalog management, validated sales via inventory and garment stock, restocking, projected revenue, budget search, seasonal clearance, description.

### GarmentSpec (data transfer struct)
- Fields: `name`, `brand`, `size`, `color`, `material`, `price`, `care`, `manufacturer`, `season`, `stockCount`
- Purpose: carries all base-garment data into constructors of the hierarchy, including the virtual base `Garment` in diamond and multiple-inheritance cases.

---

## Associations

A class is associated with another class when it stores it as a field or accepts/returns it as a method parameter/result. The model contains 44 such associations.

| # | Association | Kind |
|---:|---|---|
| 1 | `Size` → `Measurement` via `chest_` | field |
| 2 | `Size` → `Measurement` via `waist_` | field |
| 3 | `Size` → `Measurement` via `hip_` | field |
| 4 | `Garment` → `Size` via `size_` | field |
| 5 | `Garment` → `Color` via `color_` | field |
| 6 | `Garment` → `Material` via `material_` | field |
| 7 | `Garment` → `PriceTag` via `price_` | field |
| 8 | `Garment` → `CareInstructions` via `care_` | field |
| 9 | `Garment` → `Manufacturer` via `manufacturer_` | field |
| 10 | `Outfit` → `Top` via `top_` | field |
| 11 | `Outfit` → `Bottom` via `bottom_` | field |
| 12 | `Outfit` → `Outerwear` via `outer_` | field |
| 13 | `Outfit` → `Footwear` via `footwear_` | field |
| 14 | `Outfit` → `Accessory` via `accessory_` | field |
| 15 | `Wardrobe` → `Garment` via `garments_` | field collection |
| 16 | `Wardrobe` → `Outfit` via `lastOutfit_` | field |
| 17 | `ClothingStore` → `Garment` via `catalog_` | field collection |
| 18 | `ClothingStore` → `Inventory` via `inventory_` | field |
| 19 | `ClothingStore` → `DiscountPolicy` via `policy_` | field |
| 20 | `ClothingStore` → `LaundryService` via `laundry_` | field |
| 21 | `ClothingStore` → `SizeChart` via `sizeChart_` | field |
| 22 | `LaundryService` → `CareInstructions` via `carePolicy_` | field |
| 23 | `Accessory::matchesColor(const Color&)` | parameter |
| 24 | `Top::matchesBottom(const Bottom&)` | parameter |
| 25 | `Bottom::suitsFootwear(const Footwear&)` | parameter |
| 26 | `Jacket::layerOver(const Garment&)` | parameter |
| 27 | `Garment::isCompatibleWith(const Garment&)` | parameter |
| 28 | `Warehouse::canStore(const Garment&)` | parameter |
| 29 | `LaundryService::accepts(const Garment&)` | parameter |
| 30 | `LaundryService::recommendedProgram(const Material&)` | parameter |
| 31 | `LaundryService::estimateTime(const Garment&)` | parameter |
| 32 | `LaundryService::costFor(const std::vector<shared_ptr<Garment>>&)` | parameter |
| 33 | `DiscountPolicy::appliesTo(const Garment&)` | parameter |
| 34 | `DiscountPolicy::apply(const PriceTag&)` | parameter |
| 35 | `CareInstructions::isSafeFor(const Material&)` | parameter |
| 36 | `Wardrobe::addGarment(shared_ptr<Garment>)` | parameter |
| 37 | `Wardrobe::findByColor(const Color&)` / returns `vector<shared_ptr<Garment>>` | parameter/result |
| 38 | `Wardrobe::recommendOutfit()` returns `Outfit` | result |
| 39 | `Outfit::addLayer(shared_ptr<Garment>)` | parameter |
| 40 | `ClothingStore::addToCatalog(shared_ptr<Garment>)` | parameter |
| 41 | `ClothingStore::sell(shared_ptr<Garment>, int)` | parameter |
| 42 | `ClothingStore::findAffordable(double)` returns `vector<shared_ptr<Garment>>` | result |
| 43 | `ClothingStore::seasonalClearance(Season)` returns `vector<shared_ptr<Garment>>` | result |
| 44 | `ClothingStore` → `SizeChart` through `SizeChart()` constructor parameter | construction |

---

## Project layout

- `include/clothing/Clothing.h` — all domain class declarations
- `include/clothing/Exceptions.h` — exception hierarchy
- `src/Core.cpp` — value types, care, manufacturer, price, identity
- `src/Materials.cpp` — material and fabric hierarchy
- `src/Garments.cpp` — garment hierarchy (including `Dress`)
- `src/Sportswear.cpp` — sport mixin and multiple-inheritance classes
- `src/Retail.cpp` — inventory, wardrobe, outfit, store, laundry, discounts
- `src/Exceptions.cpp` — exception classes
- `src/main.cpp` — demonstration program
- `tests/test_main.cpp` — self-contained tests
- `CMakeLists.txt` — build and test configuration
