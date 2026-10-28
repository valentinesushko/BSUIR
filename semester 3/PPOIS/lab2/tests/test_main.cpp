#include "clothing/Clothing.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace clothing;

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAILED: " << message << "\n";
    }
}

void expectNear(double actual, double expected, double tolerance,
                const std::string& message) {
    expect(std::abs(actual - expected) <= tolerance,
           message + " (actual " + std::to_string(actual) + ", expected " +
               std::to_string(expected) + ")");
}

GarmentSpec makeSpec(std::shared_ptr<Material> material = nullptr,
                     Season season = Season::AllSeason, int stock = 5) {
    GarmentSpec spec;
    spec.name = "Test garment";
    spec.brand = "Test brand";
    spec.color = Color(20, 20, 20, "Black");
    spec.material = std::move(material);
    spec.price = PriceTag(100.0, Currency::BYN, 0.0, 20.0);
    spec.care = CareInstructions(30, false, false, 110, false);
    spec.manufacturer =
        Manufacturer("Test brand", "Belarus", "test@example.com", 2000, 8.0);
    spec.season = season;
    spec.stockCount = stock;
    return spec;
}

void testCoreValues() {
    Measurement chest(96.0, MeasurementType::Chest);
    expectNear(chest.toInches(), 37.795, 0.001, "measurement converts to inches");

    bool threw = false;
    try {
        Measurement invalid(-1.0, MeasurementType::Waist);
        (void)invalid;
    } catch (const InvalidMeasurementException&) {
        threw = true;
    }
    expect(threw, "negative measurement throws");

    Color red(255, 0, 0, "Red");
    expect(red.toHex() == "#FF0000", "color hex is uppercase");
    expect(red.mix(Color(0, 0, 255, "Blue"), 0.5) == Color(128, 0, 128, "Purple"),
           "color mixing blends channels");

    Size size("M", SizeSystem::EU, Measurement(96.0, MeasurementType::Chest),
              Measurement(80.0, MeasurementType::Waist),
              Measurement(100.0, MeasurementType::Hip));
    expect(size.fits(Measurement(98.0, MeasurementType::Chest)),
           "size fits close chest measurement");
    expect(size.convertedTo(SizeSystem::US).system() == SizeSystem::US,
           "size converts to US system");
}

void testMaterialsAndCare() {
    auto cotton = std::make_shared<CottonFabric>(
        "Cotton", 160.0, 0.8, 2.0, WeaveType::Plain, 150.0, 140, true, true,
        "Belarus");
    auto wool = std::make_shared<WoolFabric>(
        "Wool", 240.0, 0.6, 1.0, WeaveType::Rib, 160.0, 40, 15.0, 18, true);
    auto leather = std::make_shared<Leather>("Leather", LeatherType::Cow, 1.2, 0.2,
                                             6.0, "Minsk");

    expect(cotton->isOrganic(), "cotton remembers organic flag");
    expect(cotton->isHypoallergenic(), "organic cotton is hypoallergenic");
    expect(wool->isMerino(), "wool remembers merino flag");
    expect(wool->suitableFor(Season::Winter), "wool is suitable for winter");
    expect(leather->requiresDryCleaning(), "leather requires dry cleaning");

    CareInstructions care(30, false, false, 110, false);
    expect(care.isSafeFor(*cotton), "care label is safe for cotton");
    expect(!care.isSafeFor(*leather), "care label is not safe for leather");
    expect(care.recommendedCycle() == "delicate cycle",
           "using-directive resolves cycle name");

    std::vector<std::shared_ptr<Material>> materials{cotton, wool, leather};
    expect(materials.size() == 3, "materials can be stored polymorphically");
    expect(materials[2]->describe().find("leather") != std::string::npos,
           "virtual describe is dispatched through base pointer");
}

void testGarments() {
    auto cotton = std::make_shared<CottonFabric>(
        "Cotton", 160.0, 0.8, 2.0, WeaveType::Plain, 150.0, 140, true, true,
        "Belarus");
    GarmentSpec spec = makeSpec(cotton, Season::Summer, 5);

    TShirt tshirt(spec, "Hello", "text");
    expect(tshirt.isGraphic(), "t-shirt with print is graphic");
    expect(tshirt.isAvailable(), "t-shirt is available");
    expectNear(tshirt.totalPrice(), 120.0, 0.001, "tax is included in total price");

    tshirt.markSold(2);
    expect(tshirt.stockCount() == 3, "sold units reduce stock");

    bool threw = false;
    try {
        tshirt.markSold(100);
    } catch (const OutOfStockException&) {
        threw = true;
    }
    expect(threw, "selling more than stock throws");

    Jeans jeans(spec, "raw", 6, true);
    expect(jeans.isSelvedge(), "jeans remember selvedge flag");
    expect(jeans.fadeLevel() == 0.0, "raw denim has no fade");

    Jacket jacket(spec, "biker", 3, false);
    expect(jacket.isBikerStyle(), "jacket detects biker lapel");
    expect(jacket.layerOver(tshirt), "jacket can layer over summer top");

    Dress dress(spec, true, "evening");
    expect(dress.isEveningWear(), "dress with train is evening wear");
    Garment* throughTop = dynamic_cast<Top*>(&dress);
    Garment* throughBottom = dynamic_cast<Bottom*>(&dress);
    Garment* direct = static_cast<Garment*>(&dress);
    expect(throughTop == direct && throughBottom == direct,
           "virtual inheritance keeps one Garment subobject");

    SportJacket sportJacket(spec, ActivityType::Running, true, 25.0, 3, true);
    Jacket* asJacket = &sportJacket;
    Sportswear* asSportswear = &sportJacket;
    expect(asJacket != nullptr && asSportswear != nullptr,
           "sport jacket exposes both parent interfaces");
    expect(sportJacket.isHighImpact(), "sport jacket inherits impact logic");
    expect(sportJacket.visibilityScore() == 9.0, "reflective strips add visibility");

    SportSneaker sportSneaker(spec, ActivityType::Hiking, true, "trail");
    expect(sportSneaker.raceReadiness() > 5.0, "sport sneaker has race readiness");
}

void testRetail() {
    auto cotton = std::make_shared<CottonFabric>(
        "Cotton", 160.0, 0.8, 2.0, WeaveType::Plain, 150.0, 140, true, true,
        "Belarus");
    GarmentSpec spec = makeSpec(cotton, Season::Winter, 10);

    auto tshirt = std::make_shared<TShirt>(spec, "", "none");
    auto jeans = std::make_shared<Jeans>(spec, "dark", 6, false);
    auto jacket = std::make_shared<Jacket>(spec, "notch", 2, false);
    auto sneakers = std::make_shared<Sneaker>(spec);
    auto belt = std::make_shared<Belt>(spec);

    Wardrobe wardrobe("Tester", 5);
    wardrobe.addGarment(tshirt);
    wardrobe.addGarment(jeans);
    wardrobe.addGarment(jacket);
    wardrobe.addGarment(sneakers);
    wardrobe.addGarment(belt);

    expect(wardrobe.totalValue() > 0.0, "wardrobe total value is positive");
    expect(wardrobe.countBySeason(Season::Winter) == 5,
           "all-season garments match winter");

    Outfit outfit = wardrobe.recommendOutfit();
    expect(outfit.isComplete(), "recommended outfit is complete");
    expect(outfit.formalityScore() > 0.0, "outfit has a formality score");

    bool duplicateThrew = false;
    try {
        wardrobe.addGarment(tshirt);
    } catch (const DuplicateGarmentException&) {
        duplicateThrew = true;
    }
    expect(duplicateThrew, "duplicate garment is rejected");

    bool fullThrew = false;
    try {
        wardrobe.addGarment(std::make_shared<Scarf>(spec));
    } catch (const WardrobeFullException&) {
        fullThrew = true;
    }
    expect(fullThrew, "full wardrobe is rejected");

    Inventory inventory("Minsk", 0);
    inventory.addStock("SKU-1", 5);
    expect(inventory.stockOf("SKU-1") == 5, "inventory adds stock");
    inventory.removeStock("SKU-1", 2);
    expect(inventory.totalUnits() == 3, "inventory removes stock");

    bool stockThrew = false;
    try {
        inventory.removeStock("SKU-1", 100);
    } catch (const InsufficientStockException&) {
        stockThrew = true;
    }
    expect(stockThrew, "insufficient stock is rejected");

    Warehouse warehouse("Central", 50, "Main street", true);
    expect(warehouse.canStore(*jacket), "warehouse can store jacket");

    SizeChart chart;
    expect(chart.convert("M") == "46", "size chart converts M");
    bool sizeThrew = false;
    try {
        chart.convert("unknown");
    } catch (const InvalidSizeException&) {
        sizeThrew = true;
    }
    expect(sizeThrew, "unknown size label throws");

    LaundryService laundry("eco wash", 30, true, 8.0);
    expect(laundry.isEcoFriendly(), "eco detergent is eco friendly");
    expect(laundry.accepts(*tshirt), "laundry accepts cotton t-shirt");
    expect(laundry.recommendedProgram(*cotton) == "normal cycle",
           "laundry recommends normal cycle");
    expect(laundry.estimateTime(*tshirt) > 0, "laundry estimates positive time");

    DiscountPolicy policy(10.0, 50.0, true, "WINTER", Season::Winter);
    expect(policy.isActive(Season::Winter), "winter policy is active in winter");
    expect(!policy.isActive(Season::Summer), "winter policy is inactive in summer");
    expect(policy.appliesTo(*jacket), "policy applies to winter jacket");

    Inventory storeInventory("Store", 0);
    storeInventory.addStock(tshirt->id(), 10);
    ClothingStore store("Store", storeInventory, policy,
                        LaundryService("eco", 30, true, 10.0), chart);
    store.addToCatalog(tshirt);
    store.addToCatalog(jeans);
    expect(store.findAffordable(1000.0).size() == 2,
           "store finds affordable catalog items");
    expect(store.seasonalClearance(Season::Winter).size() == 2,
           "store finds winter clearance items");
    expect(store.sell(tshirt, 2) > 0.0, "store sells garment");
}

void testExceptions() {
    expect(InvalidSizeException("x").errorCode() == "CLOTHING-SIZE",
           "invalid size code");
    expect(InvalidPriceException("x").errorCode() == "CLOTHING-PRICE",
           "invalid price code");
    expect(OutOfStockException("x").errorCode() == "CLOTHING-STOCK",
           "out of stock code");
    expect(IncompatibleMaterialException("x").errorCode() == "CLOTHING-MATERIAL",
           "incompatible material code");
    expect(CareInstructionException("x").errorCode() == "CLOTHING-CARE",
           "care instruction code");
    expect(WardrobeFullException("x").errorCode() == "CLOTHING-WARDROBE-FULL",
           "wardrobe full code");
    expect(GarmentNotFoundException("x").errorCode() == "CLOTHING-NOT-FOUND",
           "not found code");
    expect(InvalidColorException("x").errorCode() == "CLOTHING-COLOR",
           "invalid color code");
    expect(SizeMismatchException("x").errorCode() == "CLOTHING-SIZE-MISMATCH",
           "size mismatch code");
    expect(InvalidMeasurementException("x").errorCode() == "CLOTHING-MEASUREMENT",
           "invalid measurement code");
    expect(DuplicateGarmentException("x").errorCode() == "CLOTHING-DUPLICATE",
           "duplicate code");
    expect(InsufficientStockException("x").errorCode() == "CLOTHING-STOCK-LOW",
           "insufficient stock code");
    expect(InvalidSeasonException("x").errorCode() == "CLOTHING-SEASON",
           "invalid season code");
    expect(UnsupportedOperationException("x").errorCode() == "CLOTHING-UNSUPPORTED",
           "unsupported operation code");

    bool parseThrew = false;
    try {
        Garment::parseSeason("monsoon");
    } catch (const InvalidSeasonException&) {
        parseThrew = true;
    }
    expect(parseThrew, "unknown season throws");
    expect(Garment::parseSeason("fall") == Season::Autumn, "fall maps to autumn");
}

} // namespace

int main() {
    testCoreValues();
    testMaterialsAndCare();
    testGarments();
    testRetail();
    testExceptions();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "All clothing model tests passed\n";
    return 0;
}
