#include "clothing/Clothing.h"

#include <iostream>
#include <memory>

using namespace clothing;

namespace {

GarmentSpec makeSpec(const std::string& name, std::shared_ptr<Material> material,
                     const Color& color, Season season, int stockCount,
                     double price) {
    GarmentSpec spec;
    spec.name = name;
    spec.brand = "BSUIR Wear";
    spec.color = color;
    spec.material = std::move(material);
    spec.price = PriceTag(price, Currency::BYN, 10.0, 20.0);
    spec.care = CareInstructions(30, false, false, 110, false);
    spec.manufacturer =
        Manufacturer("BSUIR Wear", "Belarus", "shop@bsuir.by", 1995, 8.5);
    spec.season = season;
    spec.stockCount = stockCount;
    return spec;
}

} // namespace

int main() {
    try {
        auto cotton = std::make_shared<CottonFabric>(
            "Cotton poplin", 160.0, 0.8, 2.0, WeaveType::Plain, 150.0, 140, true,
            true, "Belarus");
        auto wool = std::make_shared<WoolFabric>(
            "Merino jersey", 220.0, 0.6, 1.0, WeaveType::Rib, 160.0, 40, 15.0, 18,
            true);
        auto leather = std::make_shared<Leather>(
            "Full grain leather", LeatherType::Cow, 1.2, 0.2, 6.0, "Minsk Tannery");

        const Color navy(20, 30, 70, "Navy");
        const Color white(245, 245, 245, "White");
        const Color black(10, 10, 10, "Black");

        auto tshirt = std::make_shared<TShirt>(
            makeSpec("Graphic tee", cotton, white, Season::Summer, 10, 65.0),
            "BSUIR", "university");
        auto jeans = std::make_shared<Jeans>(
            makeSpec("Slim jeans", cotton, navy, Season::Autumn, 8, 140.0),
            "dark", 6, true);
        auto jacket = std::make_shared<Jacket>(
            makeSpec("City jacket", leather, black, Season::Spring, 4, 320.0),
            "biker", 3, false);
        auto sneakers = std::make_shared<Sneaker>(
            makeSpec("Runner", cotton, white, Season::AllSeason, 6, 210.0),
            "foam", "lugged", 8.0);
        auto scarf = std::make_shared<Scarf>(
            makeSpec("Plaid scarf", wool, navy, Season::Winter, 5, 80.0), 6.0,
            "plaid");
        auto belt = std::make_shared<Belt>(
            makeSpec("Leather belt", leather, black, Season::AllSeason, 7, 55.0),
            "pin", 3.5, 5);

        Wardrobe wardrobe("Valentin", 10);
        wardrobe.addGarment(tshirt);
        wardrobe.addGarment(jeans);
        wardrobe.addGarment(jacket);
        wardrobe.addGarment(sneakers);
        wardrobe.addGarment(scarf);
        wardrobe.addGarment(belt);

        std::cout << "Wardrobe code: " << wardrobe.wardrobeCode() << "\n";
        std::cout << "Wardrobe value: " << wardrobe.totalValue() << " BYN\n";
        std::cout << "Winter items: " << wardrobe.countBySeason(Season::Winter)
                  << "\n";

        Outfit outfit = wardrobe.recommendOutfit();
        std::cout << outfit.describe() << "\n";
        std::cout << "Outfit price: " << outfit.totalPrice() << " BYN, palette "
                  << (outfit.matchesPalette() ? "matches" : "clashes") << "\n";

        Inventory inventory("Minsk store", 0);
        inventory.addStock(tshirt->id(), 10);
        inventory.addStock(jeans->id(), 8);
        inventory.addStock(jacket->id(), 4);

        ClothingStore store(
            "BSUIR Store", inventory,
            DiscountPolicy(15.0, 100.0, false, "WELCOME", Season::AllSeason),
            LaundryService("eco wash", 30, true, 12.0), SizeChart());
        store.addToCatalog(tshirt);
        store.addToCatalog(jeans);
        store.addToCatalog(jacket);
        store.addToCatalog(sneakers);

        std::cout << store.describe() << "\n";
        std::cout << "Sold for " << store.sell(tshirt, 2) << " BYN\n";
        std::cout << "Projected revenue: " << store.projectedRevenue() << " BYN\n";

        const auto affordable = store.findAffordable(150.0);
        std::cout << "Affordable items: " << affordable.size() << "\n";

        std::cout << "Clean method for jacket: " << jacket->cleanMethod() << "\n";
        std::cout << "Laundry program: " << store.laundry().recommendedProgram(*cotton)
                  << "\n";

        Warehouse warehouse("Central", 100, "Nezavisimosti 1", true);
        std::cout << "Warehouse can store jacket: "
                  << (warehouse.canStore(*jacket) ? "yes" : "no") << "\n";

        std::cout << "Parsed season: " << toString(Garment::parseSeason("Winter"))
                  << "\n";

        try {
            const Color invalid(300, 0, 0, "Broken");
            (void)invalid;
        } catch (const InvalidColorException& error) {
            std::cout << "Caught " << error.errorCode() << ": " << error.what()
                      << "\n";
        }

        try {
            wardrobe.addGarment(tshirt);
        } catch (const DuplicateGarmentException& error) {
            std::cout << "Caught " << error.errorCode() << ": " << error.what()
                      << "\n";
        }
    } catch (const ClothingException& error) {
        std::cerr << "Clothing error " << error.errorCode() << ": " << error.what()
                  << "\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Unexpected error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
