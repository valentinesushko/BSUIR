#include "clothing/Clothing.h"

#include <algorithm>
#include <cctype>
#include <cmath>
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

Inventory::Inventory() : location_("Unassigned") {}

Inventory::Inventory(std::string location, int initialUnits)
    : location_(std::move(location)) {
    if (initialUnits > 0) {
        stock_["DEFAULT"] = initialUnits;
    }
}

std::string Inventory::location() const {
    return location_;
}

void Inventory::addStock(const std::string& sku, int quantity) {
    if (quantity <= 0) {
        throw InsufficientStockException("Stock quantity must be positive");
    }
    stock_[sku] += quantity;
}

void Inventory::removeStock(const std::string& sku, int quantity) {
    if (quantity <= 0) {
        throw InsufficientStockException("Removal quantity must be positive");
    }
    const auto it = stock_.find(sku);
    if (it == stock_.end() || it->second < quantity) {
        throw InsufficientStockException("Not enough stock for SKU " + sku);
    }
    it->second -= quantity;
    if (it->second == 0) {
        stock_.erase(it);
    }
}

int Inventory::stockOf(const std::string& sku) const {
    const auto it = stock_.find(sku);
    return it == stock_.end() ? 0 : it->second;
}

bool Inventory::isLowStock(const std::string& sku, int threshold) const {
    return stockOf(sku) <= threshold;
}

int Inventory::totalUnits() const {
    int total = 0;
    for (const auto& entry : stock_) {
        total += entry.second;
    }
    return total;
}

std::vector<std::string> Inventory::listSkus() const {
    std::vector<std::string> skus;
    for (const auto& entry : stock_) {
        skus.push_back(entry.first);
    }
    return skus;
}

Warehouse::Warehouse()
    : Inventory("Main warehouse", 0),
      capacity_(1000),
      address_("Unknown address"),
      temperatureControlled_(false) {}

Warehouse::Warehouse(std::string location, int capacity, std::string address,
                     bool temperatureControlled)
    : Inventory(std::move(location), 0),
      capacity_(std::max(1, capacity)),
      address_(std::move(address)),
      temperatureControlled_(temperatureControlled) {}

int Warehouse::capacity() const { return capacity_; }
const std::string& Warehouse::address() const { return address_; }
bool Warehouse::temperatureControlled() const { return temperatureControlled_; }

bool Warehouse::canStore(const Garment& garment) const {
    if (totalUnits() >= capacity_) {
        return false;
    }
    if (garment.material() && garment.material()->requiresDryCleaning() &&
        !temperatureControlled_) {
        return false;
    }
    return true;
}

double Warehouse::utilizationPercent() const {
    return clampValue(100.0 * totalUnits() / capacity_, 0.0, 100.0);
}

bool Warehouse::requiresHeating() const {
    return temperatureControlled_;
}

Outfit::Outfit() = default;

Outfit::Outfit(std::shared_ptr<Top> top, std::shared_ptr<Bottom> bottom,
               std::shared_ptr<Outerwear> outer,
               std::shared_ptr<Footwear> footwear,
               std::shared_ptr<Accessory> accessory)
    : top_(std::move(top)),
      bottom_(std::move(bottom)),
      outer_(std::move(outer)),
      footwear_(std::move(footwear)),
      accessory_(std::move(accessory)) {}

std::shared_ptr<Top> Outfit::top() const { return top_; }
std::shared_ptr<Bottom> Outfit::bottom() const { return bottom_; }
std::shared_ptr<Outerwear> Outfit::outer() const { return outer_; }
std::shared_ptr<Footwear> Outfit::footwear() const { return footwear_; }
std::shared_ptr<Accessory> Outfit::accessory() const { return accessory_; }

bool Outfit::isComplete() const {
    return top_ && bottom_ && outer_ && footwear_ && accessory_;
}

double Outfit::totalPrice() const {
    double total = 0.0;
    if (top_) total += top_->totalPrice();
    if (bottom_) total += bottom_->totalPrice();
    if (outer_) total += outer_->totalPrice();
    if (footwear_) total += footwear_->totalPrice();
    if (accessory_) total += accessory_->totalPrice();
    return total;
}

bool Outfit::matchesPalette() const {
    std::vector<Color> colors;
    if (top_) colors.push_back(top_->color());
    if (bottom_) colors.push_back(bottom_->color());
    if (outer_) colors.push_back(outer_->color());
    if (footwear_) colors.push_back(footwear_->color());
    if (accessory_) colors.push_back(accessory_->color());

    for (std::size_t i = 0; i < colors.size(); ++i) {
        for (std::size_t j = i + 1; j < colors.size(); ++j) {
            if (colors[i] == colors[j] || colors[i].isNeutral() ||
                colors[j].isNeutral()) {
                continue;
            }
            if (colors[i].contrastWith(colors[j]) < 0.1) {
                return false;
            }
        }
    }
    return true;
}

double Outfit::formalityScore() const {
    double total = 0.0;
    int count = 0;
    if (top_) {
        total += top_->isFormal() ? 8.0 : 5.0;
        ++count;
    }
    if (bottom_) {
        total += bottom_->isLong() ? 7.0 : 5.0;
        ++count;
    }
    if (outer_) {
        total += outer_->protectsFromRain() ? 8.0 : 7.0;
        ++count;
    }
    if (footwear_) {
        total += footwear_->isHeeled() ? 8.0 : 5.0;
        ++count;
    }
    if (accessory_) {
        total += accessory_->isDecorative() ? 7.0 : 5.0;
        ++count;
    }
    return count == 0 ? 0.0 : total / count;
}

std::string Outfit::describe() const {
    std::ostringstream out;
    out << "Outfit: ";
    out << (top_ ? top_->name() : "no top") << " + "
        << (bottom_ ? bottom_->name() : "no bottom") << " + "
        << (outer_ ? outer_->name() : "no outer layer") << " + "
        << (footwear_ ? footwear_->name() : "no footwear") << " + "
        << (accessory_ ? accessory_->name() : "no accessory");
    return out.str();
}

void Outfit::addLayer(std::shared_ptr<Garment> garment) {
    if (!garment) {
        throw GarmentNotFoundException("Cannot add a null garment to an outfit");
    }
    if (auto top = std::dynamic_pointer_cast<Top>(garment)) {
        top_ = top;
        return;
    }
    if (auto bottom = std::dynamic_pointer_cast<Bottom>(garment)) {
        bottom_ = bottom;
        return;
    }
    if (auto outer = std::dynamic_pointer_cast<Outerwear>(garment)) {
        outer_ = outer;
        return;
    }
    if (auto footwear = std::dynamic_pointer_cast<Footwear>(garment)) {
        footwear_ = footwear;
        return;
    }
    if (auto accessory = std::dynamic_pointer_cast<Accessory>(garment)) {
        accessory_ = accessory;
        return;
    }
    throw UnsupportedOperationException("Garment type cannot be used in an outfit");
}

Wardrobe::Wardrobe() : ownerName_("Unknown"), capacity_(20) {}

Wardrobe::Wardrobe(std::string ownerName, int capacity)
    : ownerName_(std::move(ownerName)), capacity_(std::max(1, capacity)) {}

const std::string& Wardrobe::ownerName() const { return ownerName_; }
int Wardrobe::capacity() const { return capacity_; }

void Wardrobe::addGarment(std::shared_ptr<Garment> garment) {
    if (!garment) {
        throw GarmentNotFoundException("Cannot add a null garment to a wardrobe");
    }
    for (const auto& existing : garments_) {
        if (existing->id() == garment->id()) {
            throw DuplicateGarmentException("Garment " + garment->id() +
                                            " is already in the wardrobe");
        }
    }
    if (static_cast<int>(garments_.size()) >= capacity_) {
        throw WardrobeFullException("Wardrobe capacity has been reached");
    }
    garments_.push_back(std::move(garment));
}

void Wardrobe::removeGarment(const std::string& id) {
    const auto it = std::find_if(garments_.begin(), garments_.end(),
                                 [&id](const std::shared_ptr<Garment>& garment) {
                                     return garment->id() == id;
                                 });
    if (it == garments_.end()) {
        throw GarmentNotFoundException("Garment " + id + " was not found");
    }
    garments_.erase(it);
}

std::vector<std::shared_ptr<Garment>> Wardrobe::findByColor(
    const Color& color) const {
    std::vector<std::shared_ptr<Garment>> result;
    for (const auto& garment : garments_) {
        if (garment->color() == color) {
            result.push_back(garment);
        }
    }
    return result;
}

double Wardrobe::totalValue() const {
    double total = 0.0;
    for (const auto& garment : garments_) {
        total += garment->totalPrice();
    }
    return total;
}

int Wardrobe::countBySeason(Season season) const {
    int count = 0;
    for (const auto& garment : garments_) {
        if (garment->isSameSeason(season)) {
            ++count;
        }
    }
    return count;
}

Outfit Wardrobe::recommendOutfit() const {
    std::shared_ptr<Top> top;
    std::shared_ptr<Bottom> bottom;
    std::shared_ptr<Outerwear> outer;
    std::shared_ptr<Footwear> footwear;
    std::shared_ptr<Accessory> accessory;

    for (const auto& garment : garments_) {
        if (!top) top = std::dynamic_pointer_cast<Top>(garment);
        if (!bottom) bottom = std::dynamic_pointer_cast<Bottom>(garment);
        if (!outer) outer = std::dynamic_pointer_cast<Outerwear>(garment);
        if (!footwear) footwear = std::dynamic_pointer_cast<Footwear>(garment);
        if (!accessory) accessory = std::dynamic_pointer_cast<Accessory>(garment);
    }

    if (!top || !bottom) {
        throw GarmentNotFoundException(
            "A wardrobe needs at least a top and a bottom to build an outfit");
    }

    top->size().ensureSameSystem(bottom->size());
    if (!top->isCompatibleWith(*bottom)) {
        throw IncompatibleMaterialException(
            "The selected top and bottom cannot be combined");
    }

    lastOutfit_ = std::make_shared<Outfit>(top, bottom, outer, footwear, accessory);
    return *lastOutfit_;
}

void Wardrobe::emptyWardrobe() {
    garments_.clear();
    lastOutfit_.reset();
}

std::string Wardrobe::wardrobeCode() const {
    return identitySummary();
}

SizeChart::SizeChart() : region_("EU") {
    mappings_["XS"] = "42";
    mappings_["S"] = "44";
    mappings_["M"] = "46";
    mappings_["L"] = "48";
    mappings_["XL"] = "50";
    mappings_["XXL"] = "52";
}

SizeChart::SizeChart(std::string region,
                     std::map<std::string, std::string> mappings)
    : region_(std::move(region)), mappings_(std::move(mappings)) {}

const std::string& SizeChart::region() const { return region_; }

std::string SizeChart::convert(const std::string& label) const {
    const auto it = mappings_.find(label);
    if (it == mappings_.end()) {
        throw InvalidSizeException("SizeChart does not know label " + label);
    }
    return it->second;
}

bool SizeChart::knowsLabel(const std::string& label) const {
    return mappings_.find(label) != mappings_.end();
}

void SizeChart::addMapping(const std::string& from, const std::string& to) {
    if (from.empty() || to.empty()) {
        throw InvalidSizeException("Size mapping labels must not be empty");
    }
    mappings_[from] = to;
}

std::string SizeChart::describe() const {
    return "Size chart for region " + region_ + " with " +
           std::to_string(mappings_.size()) + " mappings";
}

LaundryService::LaundryService()
    : detergent_("eco wash"),
      washTemperature_(30),
      dryCleaningAvailable_(true),
      capacityKg_(10.0),
      carePolicy_(30, false, false, 110, false) {}

LaundryService::LaundryService(std::string detergent, int washTemperature,
                               bool dryCleaningAvailable, double capacityKg)
    : detergent_(std::move(detergent)),
      washTemperature_(washTemperature),
      dryCleaningAvailable_(dryCleaningAvailable),
      capacityKg_(capacityKg),
      carePolicy_(washTemperature, false, false, 110, false) {}

const std::string& LaundryService::detergent() const { return detergent_; }
int LaundryService::washTemperature() const { return washTemperature_; }
bool LaundryService::dryCleaningAvailable() const { return dryCleaningAvailable_; }
double LaundryService::capacityKg() const { return capacityKg_; }

bool LaundryService::accepts(const Garment& garment) const {
    if (!garment.material()) {
        return true;
    }
    if (garment.material()->requiresDryCleaning()) {
        return dryCleaningAvailable_;
    }
    return carePolicy_.isSafeFor(*garment.material());
}

std::string LaundryService::recommendedProgram(const Material& material) const {
    using namespace garment_care;
    if (material.requiresDryCleaning()) {
        if (!dryCleaningAvailable_) {
            throw UnsupportedOperationException(
                "This laundry cannot process dry-clean-only materials");
        }
        return "professional dry cleaning";
    }
    return cycleName(static_cast<int>(material.recommendedWashTemperature()));
}

int LaundryService::estimateTime(const Garment& garment) const {
    const double weight = garment.material() ? garment.material()->weightGsm() : 150.0;
    int minutes = static_cast<int>(30 + weight / 10.0);
    if (garment.material() && garment.material()->requiresDryCleaning()) {
        minutes += 60;
    }
    return minutes;
}

double LaundryService::costFor(
    const std::vector<std::shared_ptr<Garment>>& garments) const {
    double cost = 0.0;
    for (const auto& garment : garments) {
        if (!garment) {
            continue;
        }
        const double weight = garment->material() ? garment->material()->weightGsm() : 150.0;
        cost += 2.0 + weight * 0.01;
    }
    return cost;
}

bool LaundryService::isEcoFriendly() const {
    return washTemperature_ <= 30 || toLowerCopy(detergent_).find("eco") != std::string::npos;
}

DiscountPolicy::DiscountPolicy()
    : percent_(0.0),
      minPurchase_(0.0),
      seasonalOnly_(false),
      code_("NONE"),
      targetSeason_(Season::AllSeason) {}

DiscountPolicy::DiscountPolicy(double percent, double minPurchase,
                               bool seasonalOnly, std::string code,
                               Season targetSeason)
    : percent_(percent),
      minPurchase_(minPurchase),
      seasonalOnly_(seasonalOnly),
      code_(std::move(code)),
      targetSeason_(targetSeason) {
    if (percent < 0.0 || percent > 100.0) {
        throw InvalidPriceException("Discount percent must be in [0, 100]");
    }
}

double DiscountPolicy::percent() const { return percent_; }
double DiscountPolicy::minPurchase() const { return minPurchase_; }
bool DiscountPolicy::seasonalOnly() const { return seasonalOnly_; }
const std::string& DiscountPolicy::code() const { return code_; }
Season DiscountPolicy::targetSeason() const { return targetSeason_; }

bool DiscountPolicy::appliesTo(const Garment& garment) const {
    if (seasonalOnly_ && !isActive(garment.season())) {
        return false;
    }
    return garment.totalPrice() >= minPurchase_;
}

PriceTag DiscountPolicy::apply(const PriceTag& tag) const {
    if (tag.amount() < minPurchase_) {
        return tag;
    }
    return tag.applyDiscount(percent_);
}

bool DiscountPolicy::isActive(Season season) const {
    if (!seasonalOnly_) {
        return true;
    }
    return targetSeason_ == Season::AllSeason || targetSeason_ == season;
}

std::string DiscountPolicy::describe() const {
    std::ostringstream out;
    out << "Policy " << code_ << ": " << percent_ << "% off, minimum "
        << minPurchase_;
    if (seasonalOnly_) {
        out << ", active in " << toString(targetSeason_);
    }
    return out.str();
}

ClothingStore::ClothingStore()
    : name_("Clothing store"), inventory_(), policy_(), laundry_(), sizeChart_() {}

ClothingStore::ClothingStore(std::string name, Inventory inventory,
                             DiscountPolicy policy, LaundryService laundry,
                             SizeChart sizeChart)
    : name_(std::move(name)),
      inventory_(std::move(inventory)),
      policy_(std::move(policy)),
      laundry_(std::move(laundry)),
      sizeChart_(std::move(sizeChart)) {}

const std::string& ClothingStore::name() const { return name_; }
Inventory& ClothingStore::inventory() { return inventory_; }
const DiscountPolicy& ClothingStore::policy() const { return policy_; }
const LaundryService& ClothingStore::laundry() const { return laundry_; }
const SizeChart& ClothingStore::sizeChart() const { return sizeChart_; }

void ClothingStore::addToCatalog(std::shared_ptr<Garment> garment) {
    if (!garment) {
        throw GarmentNotFoundException("Cannot add a null garment to the catalog");
    }
    catalog_.push_back(std::move(garment));
}

double ClothingStore::sell(const std::shared_ptr<Garment>& garment, int quantity) {
    if (!garment) {
        throw GarmentNotFoundException("Cannot sell a null garment");
    }
    if (!garment->isAvailable()) {
        throw OutOfStockException("Garment " + garment->name() + " is out of stock");
    }
    inventory_.removeStock(garment->id(), quantity);
    garment->markSold(quantity);
    return policy_.apply(garment->price()).finalPrice() * quantity;
}

void ClothingStore::restockSku(const std::string& sku, int quantity) {
    inventory_.addStock(sku, quantity);
}

double ClothingStore::projectedRevenue() const {
    double total = 0.0;
    for (const auto& garment : catalog_) {
        total += garment->totalPrice() * garment->stockCount();
    }
    return total;
}

std::vector<std::shared_ptr<Garment>> ClothingStore::findAffordable(
    double budget) const {
    std::vector<std::shared_ptr<Garment>> result;
    for (const auto& garment : catalog_) {
        if (policy_.apply(garment->price()).isAffordable(budget)) {
            result.push_back(garment);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Garment>> ClothingStore::seasonalClearance(
    Season season) const {
    std::vector<std::shared_ptr<Garment>> result;
    for (const auto& garment : catalog_) {
        if (garment->isSameSeason(season)) {
            result.push_back(garment);
        }
    }
    return result;
}

std::string ClothingStore::describe() const {
    std::ostringstream out;
    out << name_ << " (" << inventory_.location() << "), " << catalog_.size()
        << " catalog items, " << policy_.describe();
    return out.str();
}

} // namespace clothing
