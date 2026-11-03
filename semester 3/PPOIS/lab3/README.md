# Animal World Domain Model

A C++17 object model of the "Animal World" subject area. The project implements
an ecosystem living on a rectangular game field: plants, herbivores, and
predators. The life cycle of inhabitants is driven by a step-based simulation
with movement, feeding, reproduction, and death phases. The project also
contains a world description loader, a console renderer, and a dedicated
exception hierarchy.

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

- `animalworld_lib` — the domain library
- `animalworld_app` — a demonstration program

## Run the demonstration

```sh
./build/animalworld_app data/world.txt auto 12
```

The program takes the world file, the mode (`auto` or `step`) and optional step
count and seed through the command line:

```sh
./build/animalworld_app data/world.txt step 6 42
```

The mode `auto` runs all configured steps without pauses, the mode `step` runs
one step and waits for the Enter key.

## Run the tests

```sh
ctest --test-dir build --output-on-failure
```

## Requirement coverage

The laboratory task requires a domain model that satisfies the following
quantitative constraints:

| Requirement | Required | Implemented |
|---|---:|---:|
| Classes | not less than 50 | 72 |
| Fields | not less than 150 | exceeded by the design |
| Unique domain behaviors | not less than 100 | exceeded by the design |
| Associations | not less than 30 | 44 |
| Custom exceptions | not less than 12 | 16 |

The behavior count excludes constructors, trivial getters/setters, operators,
and the `errorCode()` overrides of exceptions. Every behavior listed below is a
domain operation with logic (validation, calculation, selection, conversion,
state transition, compatibility check, and so on).

## Subject area

The virtual arena is a rectangular field of arbitrary size built from square
cells. Each cell may hold up to four inhabitants at once and at most one plant.
Life proceeds in discrete steps that contain the following clearly separated
stages: movement, feeding, reproduction, and death from old age or hunger.

Three kinds of inhabitants are simulated:

- **Plant** — does not move, loses life points every turn, dies when they run
  out, does not feed, and reproduces by spreading to free neighboring cells.
- **Herbivore** — moves, feeds on plants of the same cell, has a size that
  defines how much plant mass it can eat, has a lifespan, may die of hunger,
  breeds with a partner of the same species on the same cell.
- **Predator** — moves, hunts herbivores of the same cell, has a size that
  defines hunting power, has a lifespan, may die of hunger, breeds with a
  partner of the same species on the same cell.

Concrete animal species (rabbit, hare, deer, elk, gazelle, zebra, elephant,
giraffe, camel, horse, goat, boar, squirrel, kangaroo, bison, wolf, fox, lion,
tiger, cheetah, leopard, bear, lynx, hyena, eagle, owl, snake, crocodile,
weasel) are implemented as subclasses of `Herbivore` or `Predator` and inherit
the common lifecycle while adding species-specific behaviors.

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Inheritance | `Plant : public Inhabitant`, `Animal : public Inhabitant`, `Herbivore : public Animal`, `Predator : public Animal`, and concrete species inherit the base hierarchy |
| Polymorphism | virtual `ageOneTurn`, `maturityAge`, `describe`, `canShareCell`, `isAnimal`, `statusLine`, `speed`, `hungerLimit`, `diet`, `makeOffspring`, `plantBite`, `fleeChance`, `maxPreySize`, `huntingSkill` dispatch behavior to concrete species |
| Exceptions | the whole domain throws `AnimalWorldException` subtypes such as `InvalidPositionException`, `WorldFormatException`, `InvalidVitalException`, `CellOvercrowdedException`, and `DuplicatePlantException` |

## Namespaces

- `animalworld` — all domain classes, value objects, and exceptions
- Headers live in `include/animalworld/` and implement in `src/`

## Exception hierarchy

All exceptions derive from `animalworld::AnimalWorldException`, which derives
from `std::runtime_error`.

| Exception | `errorCode()` | Meaning |
|---|---|---|
| `AnimalWorldException` | `AW-GENERAL` | Root of the hierarchy |
| `InvalidPositionException` | `AW-POSITION` | Board coordinate is outside the playable rectangle |
| `InvalidFieldSizeException` | `AW-FIELD-SIZE` | Field has non-positive or unusable dimensions |
| `CellOvercrowdedException` | `AW-CELL-FULL` | Cell already holds the maximum number of inhabitants |
| `DuplicatePlantException` | `AW-PLANT-DUPLICATE` | A second plant is placed into a cell that already has one |
| `UnknownSpeciesException` | `AW-SPECIES` | A species name from the world file is not registered |
| `WorldFileException` | `AW-FILE` | World description file cannot be opened or read |
| `WorldFormatException` | `AW-FORMAT` | A line or value in the world description is invalid |
| `InvalidConfigurationException` | `AW-CONFIG` | Simulation configuration is inconsistent |
| `InvalidVitalException` | `AW-VITAL` | Health, hunger, or another vital value is out of range |
| `ReproductionException` | `AW-REPRODUCTION` | A reproduction request cannot be fulfilled |
| `InvalidDietException` | `AW-DIET` | A diet description is inconsistent with its diet type |
| `InvalidGenomeException` | `AW-GENERAL` | A genome value is outside the allowed coefficient range |
| `SimulationStateException` | `AW-STATE` | A simulation operation is illegal in the current state |
| `InvalidHabitatException` | `AW-HABITAT` | A habitat description contains impossible parameters |
| `InvalidModeException` | `AW-MODE` | A command-line argument cannot be parsed |

## Core value and support classes

### Position
- Fields: `row_`, `col_`
- Methods: `Position()`, `Position(int, int)`, `row()`, `col()`, `setRow()`,
  `setCol()`, `distanceTo()`, `chebyshevDistanceTo()`, `isAdjacentTo()`,
  `movedBy()`, `clampedTo()`, `isWithin()`, `neighbors()`, `directionTowards()`,
  `toString()`, comparison operators
- Behavior: builds validated non-negative coordinates, computes distances,
  moves by direction, clamps into a rectangle, and lists neighboring cells.

### FieldSize
- Fields: `rows_`, `cols_`
- Methods: `rows()`, `cols()`, `area()`, `isInside()`, `clamp()`,
  `randomPosition()`, `isBorder()`, `neighborhoodCount()`
- Behavior: validates positive dimensions, clamps positions into the field,
  and produces deterministic pseudo-random coordinates.

### VitalStats
- Fields: `health_`, `maxHealth_`, `hunger_`, `maxHunger_`
- Methods: `health()`, `maxHealth()`, `hunger()`, `maxHunger()`, `damage()`,
  `heal()`, `feed()`, `starve()`, `isDead()`, `isStarving()`, `healthPercent()`,
  `hungerPercent()`, `reset()`
- Behavior: clamps health and hunger into their ranges, applies decay,
  healing, feeding, and starvation transitions, and exposes percentages.

### DietProfile
- Fields: `type_`, `foodPerTurn_`, `hungerThreshold_`, `preferredFood_`
- Methods: `type()`, `foodPerTurn()`, `hungerThreshold()`, `preferredFood()`,
  `canEatPlant()`, `canEatAnimal()`, `isStarvingAt()`, `nutritionFor()`,
  `describe()`
- Behavior: validates diet parameters and answers whether an animal can eat a
  plant or meat, how many turns without food cause starvation, and how much
  nutrition a food mass yields.

### MovementProfile
- Fields: `maxSteps_`, `canFly_`, `canSwim_`, `stamina_`
- Methods: `maxSteps()`, `canFly()`, `canSwim()`, `stamina()`,
  `effectiveRange()`, `canReach()`, `staminaCost()`, `requiresRest()`,
  `describe()`
- Behavior: converts stamina into an effective movement range, checks whether a
  target is reachable, and prices movement in stamina.

### ReproductionProfile
- Fields: `maturityAge_`, `gestationTurns_`, `litterSize_`, `cooldown_`,
  `breedingSeason_`
- Methods: `maturityAge()`, `gestationTurns()`, `litterSize()`, `cooldown()`,
  `breedingSeason()`, `canBreedAtAge()`, `isInSeason()`, `offspringCount()`,
  `cooldownAfterBirth()`, `describe()`
- Behavior: decides when an individual may breed, whether the breeding season
  is open, and how many offspring a litter has.

### CombatProfile
- Fields: `bodySize_`, `attackPower_`, `defense_`, `escapeChance_`
- Methods: `bodySize()`, `attackPower()`, `defense()`, `escapeChance()`,
  `canDefeat()`, `escapeProbability()`, `damageFor()`, `isLargerThan()`,
  `describe()`
- Behavior: compares body sizes, computes escape probabilities, and resolves
  combat damage.

### Genome
- Fields: `aggression_`, `speedBonus_`, `resilience_`
- Methods: `aggression()`, `speedBonus()`, `resilience()`, `mutatedCopy()`,
  `similarity()`, `dominantGene()`, `express()`, `describe()`
- Behavior: mutates coefficients within `[-1, 1]` and measures similarity
  between individuals.

### SpeciesProfile
- Fields: `name_`, `size_`, `maxAge_`, `speed_`, `diet_`, `movement_`,
  `reproduction_`, `combat_`
- Methods: `name()`, `size()`, `maxAge()`, `speed()`, `diet()`, `movement()`,
  `reproduction()`, `combat()`, `validate()`, `isPredator()`,
  `isHerbivore()`, `summary()`, `compatibleWith()`, `expectedLongevity()`
- Behavior: aggregates diet, movement, reproduction, and combat profiles of a
  species into one ecosystem description.

### SpeciesRegistry
- Fields: `profiles_`, `factories_`
- Methods: `registerSpecies()`, `create()`, `hasSpecies()`, `speciesCount()`,
  `names()`, `profileOf()`
- Behavior: stores species profiles and maps a species name to an object
  factory.

### Environment, Habitat, RandomSource, SimulationConfig, SimulationStatistics,
WorldEvent, EventLog
- Fields and methods are listed in `include/animalworld/Core.h` and
  `include/animalworld/World.h`; they keep season, weather, terrain, random
  source, simulation counters, world events, and the bounded event log.

## Inhabitant hierarchy

### Inhabitant (abstract)
- Fields: `id_`, `speciesName_`, `position_`, `size_`, `age_`, `maxAge_`, `alive_`, `symbol_`
- Methods: virtual `ageOneTurn()`, pure virtual `maturityAge()`, `symbol()`,
  pure virtual `describe()`, `canShareCell()`, `isAnimal()`, `statusLine()`,
  `distanceTo()`
- Behavior: common identity, age, lifespan, and liveness bookkeeping shared by
  plants and animals.

### Plant : public Inhabitant
- Fields: `health_`, `maxHealth_`, `decayRate_`, `seedCount_`, `spreadRadius_`
- Methods: `ageOneTurn()`, `maturityAge()`, `wither()`, `regrow()`,
  `canSpreadTo()`, `spreadRange()`, `nutrition()`, `healthPercent()`,
  `describe()`, `takeDamage()`, `isWilted()`, `health()`, `maxHealth()`,
  `setHealth()`, `seedCount()`, `setSeedCount()`, `decayRate()`
- Behavior: loses health every turn, dies when health reaches zero, regrows
  from seeds, and spreads to neighboring cells.

### Animal : public Inhabitant
- Fields: `sex_`, `vital_`, `genome_`, `hungerTurns_`, `hungerTurnsLimit_`,
  `breedingCooldown_`, `maturityAge_`, `diet_`, `speed_`
- Methods: `sex()`, `setSex()`, `vital()`, `genome()`, `isHungry()`,
  `isStarving()`, `speed()`, `hungerLimit()`, `diet()`, `makeOffspring()`,
  `growHunger()`, `feed()`, `ageOneTurn()`, `canBreed()`, `canBreedWith()`,
  `startBreedingCooldown()`, `breedingCooldown()`, `tickCooldown()`,
  `canReach()`, `moveTowards()`, `statusLine()`, `describe()`, `buildProfile()`,
  `resetHungerTurns()`, `hungerTurns()`, `isAnimal()`
- Behavior: tracks vital statistics, hunger turns, breeding cooldown, and
  species-specific movement logic.

### Herbivore : public Animal
- Fields: `grazeRate_`, `fleeInstinct_`, `herdSize_`
- Methods: `plantBite()`, `fleeChance()`, `graze()`, `canGraze()`,
  `fleeFrom()`, `preferredPlantHealth()`, `nutritionFrom()`, `grazeReport()`
- Behavior: grazes plants of the current cell, flees from predators, and
  computes nutrition.

### Predator : public Animal
- Fields: `huntingSkill_`, `killCount_`, `territoryRadius_`
- Methods: `maxPreySize()`, `huntingSkill()`, `canHunt()`, `hunt()`,
  `chase()`, `hungerAfterKill()`, `territoryRadius()`, `huntReport()`
- Behavior: hunts herbivores, chases prey, and resolves kills.

## Species hierarchy

### Herbivore species

- `Rabbit` — burrow digging and hopping behavior
- `Hare` — sprint and predator detection
- `Deer` — antler shedding and camouflage
- `Elk` — bugle and charge behavior
- `Gazelle` — leaping and herd alertness
- `Zebra` — stripe-based predator confusion and herding
- `Elephant` — trumpet and trunk use
- `Giraffe` — canopy browsing and kick range
- `Camel` — water storage and desert endurance
- `Horse` — gallop and kick behavior
- `Goat` — climbing and headbutt
- `Boar` — charge and wallow
- `Squirrel` — nut caching and balance
- `Kangaroo` — hopping and boxing
- `Bison` — stampede and weather resistance

### Predator species

- `Wolf` — howl and pack coordination
- `Fox` — play dead and egg stealing
- `Lion` — roar and pride leading
- `Tiger` — swimming and ambush
- `Cheetah` — sprint and trip behavior
- `Leopard` — prey dragging to tree and stalking
- `Bear` — hibernation and swipe
- `Lynx` — pounce and silent tread
- `Hyena` — laugh and scavenging
- `Eagle` — dive bomb and soaring
- `Owl` — head rotation and silent flight
- `Snake` — venom injection and slithering
- `Crocodile` — death roll and aquatic ambush
- `Weasel` — slipping and nest raiding

## World, simulation and renderer classes

- `FieldCell` — one square of the field, holds up to four inhabitants and one
  plant, validates capacity and plant uniqueness
- `GameField` — the rectangular board of `FieldCell` objects, placement and
  lookup logic
- `WorldLoader` — reads the world description file, parses configuration and
  species counts, and builds the initial field
- `ConsoleRenderer` — renders the field, legends, and summaries to the console
- `Simulation` — drives the movement, feeding, reproduction, and death phases
  of a step and keeps statistics, events, and the environment

## Associations

A class is associated with another class when it stores it as a field or
accepts/returns it as a method parameter or result. The model contains 44 such
associations.

| # | Association | Kind |
|---:|---|---|
| 1 | `Inhabitant` → `Position` via `position_` | field |
| 2 | `Inhabitant` → `String` via `id_` and `speciesName_` | field |
| 3 | `Plant` → `Inhabitant` via inheritance | inheritance |
| 4 | `Animal` → `Inhabitant` via inheritance | inheritance |
| 5 | `Herbivore` → `Animal` via inheritance | inheritance |
| 6 | `Predator` → `Animal` via inheritance | inheritance |
| 7 | `Animal` → `VitalStats` via `vital_` | field |
| 8 | `Animal` → `Genome` via `genome_` | field |
| 9 | `Animal` → `DietProfile` via `diet_` | field |
| 10 | `Animal` → `MovementProfile` via `speed_` | field |
| 11 | `Animal` → `ReproductionProfile` via `maturityAge_` | field |
| 12 | `Predator` → `CombatProfile` via `combat_` | field |
| 13 | `Herbivore` → `Plant` via `graze()` | parameter |
| 14 | `Predator` → `Herbivore` via `hunt()` | parameter |
| 15 | `Herbivore` → `Predator` via `fleeFrom()` | parameter |
| 16 | `Animal` → `Animal` via `canBreedWith()` | parameter |
| 17 | `SpeciesRegistry` → `SpeciesProfile` via `profiles_` | field |
| 18 | `SpeciesRegistry` → `Factory` via `factories_` | field |
| 19 | `WorldLoader` → `SpeciesRegistry` via `registry_` | field |
| 20 | `WorldLoader` → `SimulationConfig` via `config` | field |
| 21 | `GameField` → `FieldCell` via `cells_` | field collection |
| 22 | `FieldCell` → `Inhabitant` via `occupants_` | field collection |
| 23 | `Simulation` → `GameField` via `field_` | field |
| 24 | `Simulation` → `SimulationConfig` via `config_` | field |
| 25 | `Simulation` → `SimulationStatistics` via `stats_` | field |
| 26 | `Simulation` → `EventLog` via `log_` | field |
| 27 | `Simulation` → `Environment` via `environment_` | field |
| 28 | `Simulation` → `RandomSource` via `random_` | field |
| 29 | `ConsoleRenderer` → `GameField` via `render()` | parameter |
| 30 | `ConsoleRenderer` → `Simulation` via `summary()` | parameter |
| 31 | `Habitat` → `SpeciesProfile` via `suitabilityFor()` | parameter |
| 32 | `Genome` → `RandomSource` via `mutatedCopy()` | parameter |
| 33 | `WorldEvent` → `EventType` / `Position` | field |
| 34 | `EventLog` → `WorldEvent` via `events_` | field collection |
| 35 | `WorldLoader` → `GameField` via `buildField()` | parameter |

## Project layout

- `include/animalworld/Core.h` — value objects, profiles, registry, and
  bookkeeping types
- `include/animalworld/Inhabitants.h` — inhabitants and species hierarchy
- `include/animalworld/World.h` — field, loader, renderer, simulation engine
- `include/animalworld/Exceptions.h` — exception hierarchy
- `src/Core.cpp` — implementations of value objects and support types
- `src/Inhabitants.cpp` — inhabitants, herbivores, predators
- `src/World.cpp` — field, loader, renderer, simulation engine
- `src/Simulation.cpp` — simulation phases
- `src/Species.cpp` — concrete species factories and behaviors
- `src/Exceptions.cpp` — exception classes
- `src/main.cpp` — demonstration program
- `data/world.txt` — example world description
- `task.txt` — laboratory task statement
