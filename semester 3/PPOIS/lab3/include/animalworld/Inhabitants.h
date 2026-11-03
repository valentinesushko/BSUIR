#ifndef ANIMALWORLD_INHABITANTS_H
#define ANIMALWORLD_INHABITANTS_H

#include "animalworld/Core.h"

#include <memory>
#include <string>

/**
 * @file Inhabitants.h
 * @brief Living inhabitants of the world: plants, animals and concrete
 *        herbivore and predator species.
 */

namespace animalworld {

class SpeciesRegistry;
class Predator;

/** @brief Common base for every plant and animal living on the field. */
class Inhabitant {
public:
    Inhabitant(std::string species, const Position& position, int size,
               int maxAge, char symbol);
    virtual ~Inhabitant() = default;

    const std::string& id() const;
    const std::string& species() const;
    const Position& position() const;
    void setPosition(const Position& position);
    int size() const;
    int age() const;
    int maxAge() const;
    bool alive() const;
    void kill();

    virtual void ageOneTurn();
    bool isMature() const;
    virtual int maturityAge() const = 0;
    virtual char symbol() const;
    virtual std::string describe() const = 0;
    virtual bool canShareCell() const;
    virtual bool isAnimal() const;
    virtual std::string statusLine() const;

    int distanceTo(const Inhabitant& other) const;

protected:
    std::string id_;
    std::string speciesName_;
    Position position_;
    int size_;
    int age_;
    int maxAge_;
    bool alive_;
    char symbol_;

    static long nextId_;
};

/** @brief The single plant species of the model. */
class Plant : public Inhabitant {
public:
    Plant();
    explicit Plant(const Position& position);
    Plant(const Position& position, int health);

    void ageOneTurn() override;
    int maturityAge() const override;
    void wither(int amount);
    void regrow();
    bool canSpreadTo(const Position& target, const FieldSize& size) const;
    int spreadRange() const;
    int nutrition() const;
    int healthPercent() const;
    std::string describe() const override;
    void takeDamage(int amount);
    bool isWilted() const;

    int health() const;
    int maxHealth() const;
    void setHealth(int health);
    int seedCount() const;
    void setSeedCount(int seeds);
    int decayRate() const;

private:
    int health_;
    int maxHealth_;
    int decayRate_;
    int seedCount_;
    int spreadRadius_;
};

/** @brief Base class for all moving creatures with hunger and sex. */
class Animal : public Inhabitant {
public:
    Animal(std::string species, const Position& position, Sex sex, int size,
           int maxAge, int maturityAge, char symbol, DietType diet,
           int hungerLimit, int speed);

    Sex sex() const;
    void setSex(Sex sex);
    VitalStats& vital();
    const VitalStats& vital() const;
    Genome& genome();
    const Genome& genome() const;

    bool isHungry() const;
    bool isStarving() const;
    virtual int speed() const;
    virtual int hungerLimit() const;
    virtual DietType diet() const;
    virtual int maturityAge() const override;
    virtual std::shared_ptr<Animal> makeOffspring(
        const Position& position) const = 0;

    void growHunger();
    void feed(int nutrition);
    void ageOneTurn() override;
    virtual bool canBreed() const;
    bool canBreedWith(const Animal& other) const;
    void startBreedingCooldown();
    int breedingCooldown() const;
    void tickCooldown();
    bool canReach(const Position& target) const;
    bool moveTowards(const Position& target, const FieldSize& fieldSize);
    std::string statusLine() const override;
    std::string describe() const override;
    SpeciesProfile buildProfile() const;
    void resetHungerTurns();
    int hungerTurns() const;
    bool isAnimal() const override;

protected:
    Sex sex_;
    VitalStats vital_;
    Genome genome_;
    int hungerTurns_;
    int hungerTurnsLimit_;
    int breedingCooldown_;
    int maturityAge_;
    DietType diet_;
    int speed_;
};

/** @brief Plant-eating animal with grazing and fleeing behaviour. */
class Herbivore : public Animal {
public:
    Herbivore(std::string species, const Position& position, Sex sex, int size,
              int maxAge, int maturityAge, char symbol, int speed,
              int hungerLimit, int grazeRate, int fleeInstinct, int herdSize);

    virtual int plantBite() const;
    virtual int fleeChance() const;
    int graze(Plant& plant);
    bool canGraze(const Plant& plant) const;
    bool fleeFrom(const Predator& predator) const;
    virtual int preferredPlantHealth() const;
    int nutritionFrom(const Plant& plant) const;
    std::string grazeReport() const;

protected:
    int grazeRate_;
    int fleeInstinct_;
    int herdSize_;
};

/** @brief Carnivore with hunting and territory behaviour. */
class Predator : public Animal {
public:
    Predator(std::string species, const Position& position, Sex sex, int size,
             int maxAge, int maturityAge, char symbol, int speed,
             int hungerLimit, int huntingSkill, int territoryRadius);

    virtual int maxPreySize() const;
    virtual double huntingSkill() const;
    bool canHunt(const Herbivore& prey) const;
    int hunt(Herbivore& prey);
    bool chase(const Herbivore& prey);
    int hungerAfterKill() const;
    int territoryRadius() const;
    std::string huntReport() const;

protected:
    int huntingSkill_;
    int killCount_;
    int territoryRadius_;
};

// ---------------------------------------------------------------------------
// Herbivore species
// ---------------------------------------------------------------------------

class Rabbit : public Herbivore {
public:
    Rabbit(const Position& position, Sex sex);
    int digBurrow(int depth);
    int hopDistance() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int burrowDepth_;
    int jumpHeight_;
};

class Hare : public Rabbit {
public:
    Hare(const Position& position, Sex sex);
    int sprintAway() const;
    bool detectPredator(int predatorDistance) const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int sprintSpeed_;
    int earLength_;
};

class Deer : public Herbivore {
public:
    Deer(const Position& position, Sex sex);
    int shedAntlers();
    double camouflageRating() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int antlerPoints_;
    double forestAffinity_;
};

class Elk : public Deer {
public:
    Elk(const Position& position, Sex sex);
    int bugle();
    double chargePower() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int bugleVolume_;
    int antlerSpread_;
};

class Gazelle : public Herbivore {
public:
    Gazelle(const Position& position, Sex sex);
    int leapAway() const;
    bool herdAlert() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int herdSizeSeen_;
    int leapHeight_;
};

class Zebra : public Herbivore {
public:
    Zebra(const Position& position, Sex sex);
    double confusePredator() const;
    int joinHerd();
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int stripeCount_;
    double herdInstinct_;
};

class Elephant : public Herbivore {
public:
    Elephant(const Position& position, Sex sex);
    int trumpet();
    double useTrunk() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int trunkLength_;
    int tuskLength_;
};

class Giraffe : public Herbivore {
public:
    Giraffe(const Position& position, Sex sex);
    int browseCanopy() const;
    double kickRange() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int neckLength_;
    int browseHeight_;
};

class Camel : public Herbivore {
public:
    Camel(const Position& position, Sex sex);
    int storeWater();
    double desertEndurance() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int humpCount_;
    int waterStorage_;
};

class Horse : public Herbivore {
public:
    Horse(const Position& position, Sex sex);
    int gallop() const;
    int kickBack() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int gallopSpeed_;
    int maneLength_;
};

class Goat : public Herbivore {
public:
    Goat(const Position& position, Sex sex);
    double climbRocks() const;
    int headbutt() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int climbSkill_;
    int beardLength_;
};

class Boar : public Herbivore {
public:
    Boar(const Position& position, Sex sex);
    int chargeIntruder() const;
    double wallow() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int tuskSharpness_;
    double mudAffinity_;
};

class Squirrel : public Herbivore {
public:
    Squirrel(const Position& position, Sex sex);
    int cacheNuts();
    bool balanceOnBranch() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int cacheSize_;
    double tailBushiness_;
};

class Kangaroo : public Herbivore {
public:
    Kangaroo(const Position& position, Sex sex);
    int hopAcross() const;
    int boxKick() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int pouchSize_;
    int tailLength_;
};

class Bison : public Herbivore {
public:
    Bison(const Position& position, Sex sex);
    int stampede() const;
    double weatherResistance() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int shoulderHump_;
    int stampedePower_;
};

// ---------------------------------------------------------------------------
// Predator species
// ---------------------------------------------------------------------------

class Wolf : public Predator {
public:
    Wolf(const Position& position, Sex sex);
    int howl() const;
    bool coordinatePack() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    bool packLeader_;
    int howlRange_;
};

class Fox : public Predator {
public:
    Fox(const Position& position, Sex sex);
    bool playDead();
    int stealEgg() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    double cunning_;
    int denDepth_;
};

class Lion : public Predator {
public:
    Lion(const Position& position, Sex sex);
    int roar() const;
    bool leadPride() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int maneSize_;
    int prideSize_;
};

class Tiger : public Predator {
public:
    Tiger(const Position& position, Sex sex);
    bool swim();
    double ambushFromWater() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int stripeCount_;
    double swimAbility_;
};

class Cheetah : public Predator {
public:
    Cheetah(const Position& position, Sex sex);
    int sprint() const;
    double tripPrey() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int sprintSpeed_;
    double acceleration_;
};

class Leopard : public Predator {
public:
    Leopard(const Position& position, Sex sex);
    int dragPreyToTree() const;
    double stalk() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    std::string spotPattern_;
    double climbSkill_;
};

class Bear : public Predator {
public:
    Bear(const Position& position, Sex sex);
    int hibernate() const;
    int swipe() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int clawLength_;
    int hibernationMonths_;
};

class Lynx : public Predator {
public:
    Lynx(const Position& position, Sex sex);
    int pounce() const;
    double treadSoftly() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int earTufts_;
    double stealth_;
};

class Hyena : public Predator {
public:
    Hyena(const Position& position, Sex sex);
    int laugh() const;
    bool scavenge() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int packSize_;
    int jawForce_;
};

class Eagle : public Predator {
public:
    Eagle(const Position& position, Sex sex);
    int diveBomb() const;
    double soar() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int wingspan_;
    int diveSpeed_;
};

class Owl : public Predator {
public:
    Owl(const Position& position, Sex sex);
    int rotateHead() const;
    double silentFlight() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    bool nightVision_;
    int wingSpan_;
};

class Snake : public Predator {
public:
    Snake(const Position& position, Sex sex);
    int injectVenom() const;
    double slither() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int venomPotency_;
    int bodyLength_;
};

class Crocodile : public Predator {
public:
    Crocodile(const Position& position, Sex sex);
    int deathRoll() const;
    double aquaticAmbush() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int jawForce_;
    int ambushDepth_;
};

class Weasel : public Predator {
public:
    Weasel(const Position& position, Sex sex);
    int slipThrough() const;
    int raidNest() const;
    std::shared_ptr<Animal> makeOffspring(const Position& position) const override;

private:
    int agility_;
    int burrowSkill_;
};

/** @brief Register every concrete species and its factory. */
void registerDefaultSpecies(SpeciesRegistry& registry);

} // namespace animalworld

#endif // ANIMALWORLD_INHABITANTS_H
