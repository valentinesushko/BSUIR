#include "animalworld/Exceptions.h"
#include "animalworld/Inhabitants.h"
#include "animalworld/World.h"

#include <iostream>
#include <string>

using namespace animalworld;

namespace {

void printUsage(const char* program) {
    std::cerr << "Usage: " << program
              << " <world-file> <auto|step> [steps] [seed]\n";
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        const std::string filename = argv[1];
        const std::string mode = argv[2];
        if (mode != "auto" && mode != "step") {
            throw InvalidModeException("Mode must be 'auto' or 'step'");
        }

        int stepsOverride = -1;
        if (argc >= 4) {
            try {
                stepsOverride = std::stoi(argv[3]);
            } catch (const std::exception&) {
                throw InvalidModeException("Step count must be an integer");
            }
            if (stepsOverride < 0) {
                throw InvalidModeException("Step count must not be negative");
            }
        }

        unsigned long seed = 12345UL;
        if (argc >= 5) {
            try {
                seed = static_cast<unsigned long>(std::stoul(argv[4]));
            } catch (const std::exception&) {
                throw InvalidModeException("Seed must be an unsigned integer");
            }
        }

        SpeciesRegistry registry;
        registerDefaultSpecies(registry);

        RandomSource random(seed);
        WorldLoader loader(filename, registry);
        SimulationConfig config;
        GameField field = loader.load(config, random);

        config.setAutoMode(mode == "auto");
        if (stepsOverride >= 0) {
            config.setSteps(stepsOverride);
        }
        config.validate();

        Simulation simulation(std::move(field), config, seed);

        ConsoleRenderer renderer;
        std::cout << "Animal world simulation\n";
        std::cout << config.describe() << "\n";
        std::cout << "Known species: " << registry.speciesCount() << "\n\n";
        std::cout << "Initial state:\n";
        std::cout << renderer.render(simulation.field());

        simulation.run(config.steps(), !config.isAutoMode());

        std::cout << "\nFinal state:\n";
        std::cout << renderer.render(simulation.field());
        std::cout << "\nSummary:\n" << simulation.summary();
    } catch (const AnimalWorldException& error) {
        std::cerr << "Animal world error [" << error.errorCode()
                  << "]: " << error.what() << "\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Unexpected error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
