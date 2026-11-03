#include "animalworld/Inhabitants.h"

#include <algorithm>
#include <memory>

namespace animalworld {

namespace {

template <typename Species>
void registerOne(SpeciesRegistry& registry) {
    const Species sample(Position(0, 0), Sex::Female);
    registry.registerSpecies(
        sample.buildProfile(), [](const Position& position, Sex sex) {
            return std::make_shared<Species>(position, sex);
        });
}

} // namespace

Rabbit::Rabbit(const Position& position, Sex sex)
    : Herbivore("Rabbit", position, sex, 1, 8, 2, 'r', 2, 4, 12, 40, 6),
      burrowDepth_(2),
      jumpHeight_(3) {}

int Rabbit::digBurrow(int depth) {
    burrowDepth_ = std::max(burrowDepth_, depth);
    return burrowDepth_ * 2;
}

int Rabbit::hopDistance() const { return jumpHeight_ + size_ * 2; }

std::shared_ptr<Animal> Rabbit::makeOffspring(const Position& position) const {
    return std::make_shared<Rabbit>(position, Sex::Female);
}

Hare::Hare(const Position& position, Sex sex)
    : Rabbit(position, sex), sprintSpeed_(4), earLength_(12) {
    speciesName_ = "Hare";
    maxAge_ = 7;
    maturityAge_ = 1;
    symbol_ = 'h';
    speed_ = 4;
    hungerTurnsLimit_ = 5;
    grazeRate_ = 14;
    fleeInstinct_ = 65;
    herdSize_ = 2;
}

int Hare::sprintAway() const { return sprintSpeed_ * 3 + earLength_ / 4; }

bool Hare::detectPredator(int predatorDistance) const {
    return predatorDistance * 2 <= earLength_ || predatorDistance <= 5;
}

std::shared_ptr<Animal> Hare::makeOffspring(const Position& position) const {
    return std::make_shared<Hare>(position, Sex::Female);
}

Deer::Deer(const Position& position, Sex sex)
    : Herbivore("Deer", position, sex, 4, 15, 3, 'd', 3, 6, 18, 45, 5),
      antlerPoints_(4),
      forestAffinity_(0.8) {}

int Deer::shedAntlers() {
    antlerPoints_ = std::max(0, antlerPoints_ - 1);
    return antlerPoints_;
}

double Deer::camouflageRating() const { return forestAffinity_ * 100.0; }

std::shared_ptr<Animal> Deer::makeOffspring(const Position& position) const {
    return std::make_shared<Deer>(position, Sex::Female);
}

Elk::Elk(const Position& position, Sex sex)
    : Deer(position, sex), bugleVolume_(8), antlerSpread_(20) {
    speciesName_ = "Elk";
    size_ = 6;
    maxAge_ = 20;
    maturityAge_ = 4;
    symbol_ = 'E';
    hungerTurnsLimit_ = 8;
    grazeRate_ = 22;
    fleeInstinct_ = 35;
    herdSize_ = 3;
}

int Elk::bugle() { return bugleVolume_ * 2 + antlerSpread_; }

double Elk::chargePower() const {
    return static_cast<double>(size_ * antlerSpread_) / 10.0;
}

std::shared_ptr<Animal> Elk::makeOffspring(const Position& position) const {
    return std::make_shared<Elk>(position, Sex::Female);
}

Gazelle::Gazelle(const Position& position, Sex sex)
    : Herbivore("Gazelle", position, sex, 2, 10, 2, 'g', 4, 5, 14, 70, 12),
      herdSizeSeen_(8),
      leapHeight_(150) {}

int Gazelle::leapAway() const { return leapHeight_ / 25 + speed_ * 2; }

bool Gazelle::herdAlert() const { return herdSizeSeen_ >= 5; }

std::shared_ptr<Animal> Gazelle::makeOffspring(const Position& position) const {
    return std::make_shared<Gazelle>(position, Sex::Female);
}

Zebra::Zebra(const Position& position, Sex sex)
    : Herbivore("Zebra", position, sex, 5, 20, 3, 'z', 4, 7, 20, 30, 10),
      stripeCount_(46),
      herdInstinct_(0.75) {}

double Zebra::confusePredator() const {
    return std::min(1.0, herdInstinct_ * 0.8 +
                             static_cast<double>(stripeCount_) / 200.0);
}

int Zebra::joinHerd() {
    ++herdSize_;
    return herdSize_;
}

std::shared_ptr<Animal> Zebra::makeOffspring(const Position& position) const {
    return std::make_shared<Zebra>(position, Sex::Female);
}

Elephant::Elephant(const Position& position, Sex sex)
    : Herbivore("Elephant", position, sex, 9, 60, 8, 'E', 2, 10, 30, 10, 8),
      trunkLength_(18),
      tuskLength_(24) {}

int Elephant::trumpet() { return trunkLength_ * 2 + tuskLength_; }

double Elephant::useTrunk() const {
    return static_cast<double>(trunkLength_) / 10.0 + 0.5;
}

std::shared_ptr<Animal> Elephant::makeOffspring(const Position& position) const {
    return std::make_shared<Elephant>(position, Sex::Female);
}

Giraffe::Giraffe(const Position& position, Sex sex)
    : Herbivore("Giraffe", position, sex, 8, 25, 4, 'G', 2, 9, 28, 15, 4),
      neckLength_(20),
      browseHeight_(500) {}

int Giraffe::browseCanopy() const { return browseHeight_ + neckLength_; }

double Giraffe::kickRange() const {
    return static_cast<double>(neckLength_) / 10.0 + 1.0;
}

std::shared_ptr<Animal> Giraffe::makeOffspring(const Position& position) const {
    return std::make_shared<Giraffe>(position, Sex::Female);
}

Camel::Camel(const Position& position, Sex sex)
    : Herbivore("Camel", position, sex, 6, 40, 4, 'C', 2, 8, 24, 20, 3),
      humpCount_(2),
      waterStorage_(80) {}

int Camel::storeWater() {
    waterStorage_ += humpCount_ * 2;
    return waterStorage_;
}

double Camel::desertEndurance() const {
    return static_cast<double>(waterStorage_) / 10.0 + humpCount_;
}

std::shared_ptr<Animal> Camel::makeOffspring(const Position& position) const {
    return std::make_shared<Camel>(position, Sex::Female);
}

Horse::Horse(const Position& position, Sex sex)
    : Herbivore("Horse", position, sex, 6, 25, 3, 'H', 4, 8, 24, 40, 6),
      gallopSpeed_(60),
      maneLength_(30) {}

int Horse::gallop() const { return gallopSpeed_ * 2; }

int Horse::kickBack() const { return gallopSpeed_ + maneLength_ / 5; }

std::shared_ptr<Animal> Horse::makeOffspring(const Position& position) const {
    return std::make_shared<Horse>(position, Sex::Female);
}

Goat::Goat(const Position& position, Sex sex)
    : Herbivore("Goat", position, sex, 3, 15, 2, 'o', 2, 5, 16, 35, 5),
      climbSkill_(70),
      beardLength_(15) {}

double Goat::climbRocks() const {
    return static_cast<double>(climbSkill_) / 100.0;
}

int Goat::headbutt() const { return climbSkill_ / 10 + beardLength_; }

std::shared_ptr<Animal> Goat::makeOffspring(const Position& position) const {
    return std::make_shared<Goat>(position, Sex::Female);
}

Boar::Boar(const Position& position, Sex sex)
    : Herbivore("Boar", position, sex, 4, 12, 2, 'B', 2, 6, 18, 25, 3),
      tuskSharpness_(7),
      mudAffinity_(0.9) {}

int Boar::chargeIntruder() const { return tuskSharpness_ * 2; }

double Boar::wallow() const {
    return mudAffinity_ * static_cast<double>(size_);
}

std::shared_ptr<Animal> Boar::makeOffspring(const Position& position) const {
    return std::make_shared<Boar>(position, Sex::Female);
}

Squirrel::Squirrel(const Position& position, Sex sex)
    : Herbivore("Squirrel", position, sex, 1, 6, 1, 's', 2, 3, 10, 55, 2),
      cacheSize_(30),
      tailBushiness_(0.8) {}

int Squirrel::cacheNuts() {
    cacheSize_ += 5;
    return cacheSize_;
}

bool Squirrel::balanceOnBranch() const { return tailBushiness_ > 0.5; }

std::shared_ptr<Animal> Squirrel::makeOffspring(const Position& position) const {
    return std::make_shared<Squirrel>(position, Sex::Female);
}

Kangaroo::Kangaroo(const Position& position, Sex sex)
    : Herbivore("Kangaroo", position, sex, 3, 12, 2, 'k', 4, 5, 16, 50, 4),
      pouchSize_(1),
      tailLength_(90) {}

int Kangaroo::hopAcross() const { return speed_ * 2 + tailLength_ / 10; }

int Kangaroo::boxKick() const { return pouchSize_ + speed_; }

std::shared_ptr<Animal> Kangaroo::makeOffspring(const Position& position) const {
    return std::make_shared<Kangaroo>(position, Sex::Female);
}

Bison::Bison(const Position& position, Sex sex)
    : Herbivore("Bison", position, sex, 8, 20, 4, 'b', 3, 9, 26, 20, 9),
      shoulderHump_(35),
      stampedePower_(80) {}

int Bison::stampede() const { return stampedePower_ * herdSize_; }

double Bison::weatherResistance() const {
    return static_cast<double>(shoulderHump_) / 10.0 + 0.5;
}

std::shared_ptr<Animal> Bison::makeOffspring(const Position& position) const {
    return std::make_shared<Bison>(position, Sex::Female);
}

Wolf::Wolf(const Position& position, Sex sex)
    : Predator("Wolf", position, sex, 3, 12, 2, 'W', 3, 7, 60, 5),
      packLeader_(true),
      howlRange_(8) {}

int Wolf::howl() const { return howlRange_ * 2; }

bool Wolf::coordinatePack() const { return packLeader_ && howlRange_ > 3; }

std::shared_ptr<Animal> Wolf::makeOffspring(const Position& position) const {
    return std::make_shared<Wolf>(position, Sex::Female);
}

Fox::Fox(const Position& position, Sex sex)
    : Predator("Fox", position, sex, 2, 8, 2, 'f', 3, 6, 45, 3),
      cunning_(0.85),
      denDepth_(4) {}

bool Fox::playDead() {
    cunning_ = std::max(0.0, cunning_ - 0.1);
    return cunning_ > 0.5;
}

int Fox::stealEgg() const {
    return denDepth_ * 2 + static_cast<int>(cunning_ * 10.0);
}

std::shared_ptr<Animal> Fox::makeOffspring(const Position& position) const {
    return std::make_shared<Fox>(position, Sex::Female);
}

Lion::Lion(const Position& position, Sex sex)
    : Predator("Lion", position, sex, 7, 18, 3, 'L', 3, 9, 75, 7),
      maneSize_(40),
      prideSize_(6) {}

int Lion::roar() const { return maneSize_ * 2 + prideSize_; }

bool Lion::leadPride() const { return prideSize_ >= 3; }

std::shared_ptr<Animal> Lion::makeOffspring(const Position& position) const {
    return std::make_shared<Lion>(position, Sex::Female);
}

Tiger::Tiger(const Position& position, Sex sex)
    : Predator("Tiger", position, sex, 7, 16, 3, 'T', 3, 10, 80, 8),
      stripeCount_(95),
      swimAbility_(0.9) {}

bool Tiger::swim() { return swimAbility_ > 0.5; }

double Tiger::ambushFromWater() const {
    return swimAbility_ * 2.0 +
           static_cast<double>(stripeCount_) / 100.0;
}

std::shared_ptr<Animal> Tiger::makeOffspring(const Position& position) const {
    return std::make_shared<Tiger>(position, Sex::Female);
}

Cheetah::Cheetah(const Position& position, Sex sex)
    : Predator("Cheetah", position, sex, 4, 12, 2, 'c', 5, 8, 70, 6),
      sprintSpeed_(110),
      acceleration_(0.95) {}

int Cheetah::sprint() const { return sprintSpeed_ * 3; }

double Cheetah::tripPrey() const { return acceleration_ * 10.0; }

std::shared_ptr<Animal> Cheetah::makeOffspring(const Position& position) const {
    return std::make_shared<Cheetah>(position, Sex::Female);
}

Leopard::Leopard(const Position& position, Sex sex)
    : Predator("Leopard", position, sex, 4, 14, 2, 'p', 4, 8, 68, 5),
      spotPattern_("rosette"),
      climbSkill_(0.9) {}

int Leopard::dragPreyToTree() const {
    return static_cast<int>(climbSkill_ * 10.0) +
           static_cast<int>(spotPattern_.size());
}

double Leopard::stalk() const { return climbSkill_ * 0.8; }

std::shared_ptr<Animal> Leopard::makeOffspring(const Position& position) const {
    return std::make_shared<Leopard>(position, Sex::Female);
}

Bear::Bear(const Position& position, Sex sex)
    : Predator("Bear", position, sex, 9, 25, 4, 'U', 2, 11, 65, 9),
      clawLength_(12),
      hibernationMonths_(5) {}

int Bear::hibernate() const { return clawLength_ * hibernationMonths_; }

int Bear::swipe() const { return clawLength_ * 3; }

std::shared_ptr<Animal> Bear::makeOffspring(const Position& position) const {
    return std::make_shared<Bear>(position, Sex::Female);
}

Lynx::Lynx(const Position& position, Sex sex)
    : Predator("Lynx", position, sex, 3, 12, 2, 'x', 3, 7, 58, 4),
      earTufts_(5),
      stealth_(0.85) {}

int Lynx::pounce() const {
    return static_cast<int>(stealth_ * 10.0) + earTufts_;
}

double Lynx::treadSoftly() const {
    return stealth_ * (earTufts_ > 0 ? 1.0 : 0.5);
}

std::shared_ptr<Animal> Lynx::makeOffspring(const Position& position) const {
    return std::make_shared<Lynx>(position, Sex::Female);
}

Hyena::Hyena(const Position& position, Sex sex)
    : Predator("Hyena", position, sex, 4, 14, 2, 'y', 3, 8, 55, 6),
      packSize_(6),
      jawForce_(70) {}

int Hyena::laugh() const { return packSize_ * 2 + jawForce_ / 10; }

bool Hyena::scavenge() const { return packSize_ >= 4; }

std::shared_ptr<Animal> Hyena::makeOffspring(const Position& position) const {
    return std::make_shared<Hyena>(position, Sex::Female);
}

Eagle::Eagle(const Position& position, Sex sex)
    : Predator("Eagle", position, sex, 2, 20, 3, 'A', 6, 6, 72, 10),
      wingspan_(220),
      diveSpeed_(160) {}

int Eagle::diveBomb() const { return diveSpeed_ * 2 + wingspan_ / 10; }

double Eagle::soar() const {
    return static_cast<double>(wingspan_) / 100.0 + 0.5;
}

std::shared_ptr<Animal> Eagle::makeOffspring(const Position& position) const {
    return std::make_shared<Eagle>(position, Sex::Female);
}

Owl::Owl(const Position& position, Sex sex)
    : Predator("Owl", position, sex, 1, 10, 2, 'O', 4, 5, 60, 4),
      nightVision_(true),
      wingSpan_(160) {}

int Owl::rotateHead() const { return nightVision_ ? 270 : 90; }

double Owl::silentFlight() const {
    return static_cast<double>(wingSpan_) / 100.0;
}

std::shared_ptr<Animal> Owl::makeOffspring(const Position& position) const {
    return std::make_shared<Owl>(position, Sex::Female);
}

Snake::Snake(const Position& position, Sex sex)
    : Predator("Snake", position, sex, 2, 15, 2, 'S', 1, 6, 50, 2),
      venomPotency_(8),
      bodyLength_(180) {}

int Snake::injectVenom() const { return venomPotency_ * 2; }

double Snake::slither() const {
    return static_cast<double>(bodyLength_) / 100.0 + 0.1;
}

std::shared_ptr<Animal> Snake::makeOffspring(const Position& position) const {
    return std::make_shared<Snake>(position, Sex::Female);
}

Crocodile::Crocodile(const Position& position, Sex sex)
    : Predator("Crocodile", position, sex, 6, 40, 5, 'K', 2, 9, 74, 3),
      jawForce_(90),
      ambushDepth_(5) {}

int Crocodile::deathRoll() const { return jawForce_ * 2; }

double Crocodile::aquaticAmbush() const {
    return static_cast<double>(ambushDepth_) / 10.0 + 0.5;
}

std::shared_ptr<Animal> Crocodile::makeOffspring(const Position& position) const {
    return std::make_shared<Crocodile>(position, Sex::Female);
}

Weasel::Weasel(const Position& position, Sex sex)
    : Predator("Weasel", position, sex, 1, 5, 1, 'w', 3, 4, 40, 2),
      agility_(9),
      burrowSkill_(6) {}

int Weasel::slipThrough() const { return agility_ * 2; }

int Weasel::raidNest() const { return burrowSkill_ + agility_; }

std::shared_ptr<Animal> Weasel::makeOffspring(const Position& position) const {
    return std::make_shared<Weasel>(position, Sex::Female);
}

void registerDefaultSpecies(SpeciesRegistry& registry) {
    registerOne<Rabbit>(registry);
    registerOne<Hare>(registry);
    registerOne<Deer>(registry);
    registerOne<Elk>(registry);
    registerOne<Gazelle>(registry);
    registerOne<Zebra>(registry);
    registerOne<Elephant>(registry);
    registerOne<Giraffe>(registry);
    registerOne<Camel>(registry);
    registerOne<Horse>(registry);
    registerOne<Goat>(registry);
    registerOne<Boar>(registry);
    registerOne<Squirrel>(registry);
    registerOne<Kangaroo>(registry);
    registerOne<Bison>(registry);
    registerOne<Wolf>(registry);
    registerOne<Fox>(registry);
    registerOne<Lion>(registry);
    registerOne<Tiger>(registry);
    registerOne<Cheetah>(registry);
    registerOne<Leopard>(registry);
    registerOne<Bear>(registry);
    registerOne<Lynx>(registry);
    registerOne<Hyena>(registry);
    registerOne<Eagle>(registry);
    registerOne<Owl>(registry);
    registerOne<Snake>(registry);
    registerOne<Crocodile>(registry);
    registerOne<Weasel>(registry);
}

} // namespace animalworld
