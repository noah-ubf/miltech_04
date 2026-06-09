#include <iostream>
#include <cstring>
#include <cmath>
#include "../include/util.hpp"
#include "../include/fabric.hpp"
#include "../include/interfaces/config_loader.hpp"
#include "../include/interfaces/target_provider.hpp"
#include "../include/interfaces/ballistic_solver.hpp"
#include "../include/classes/mission_processor.hpp"
#include "../include/basics/simulation.hpp"
#include "../include/fabric.hpp"

using namespace std;
using namespace miltech04;

int main(int argc, char** argv)
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <config_file> <ammo_file> <targets_file> <output_file>" << std::endl;  // NOLINT(*-pointer-arithmetic)
        return 1;
    }

    IConfigLoader* configLoader = createLoader(LoaderType::FILE, argv[1], argv[2]);
    if (configLoader == nullptr) {
        LOG("Error: Failed to create config loader");
        return 1;
    }

    if (!configLoader->load()) {
        delete configLoader;
        return 1;
    }

    ITargetProvider* targetProvider = createProvider(ProviderType::JSON, argv[3]);
    if (targetProvider == nullptr) {
        LOG("Error: Failed to create target provider");
        delete configLoader;
        return 1;
    }

    if (!targetProvider->isLoaded()) {
        delete configLoader;
        delete targetProvider;
        return 1;
    }

    IBallisticSolver* solver = createSolver(SolverType::ANALYTICAL, configLoader);
    if (solver == nullptr) {
        LOG("Error: Failed to create ballistic solver");
        delete configLoader;
        delete targetProvider;
        return 1;
    }

    if (!solver->isValid()) {
        LOG("Error: Ballistic solver is not valid");
        delete configLoader;
        delete targetProvider;
        delete solver;
        return 1;
    }

    SimulationResults simulation(MAX_STEPS);
    MissionProcessor processor(configLoader, solver, targetProvider);

    while (processor.hasNext()) {
        if (!simulation.push(processor.step())) break;
    }

    LOG("Simulation completed successfully");

    if (!simulation.save(argv[4])) {
        LOG("Failed to save simulation steps");
        delete configLoader;
        delete targetProvider;
        delete solver;
        return 1;
    }

    delete configLoader;
    delete targetProvider;
    delete solver;
    return 0;
}
