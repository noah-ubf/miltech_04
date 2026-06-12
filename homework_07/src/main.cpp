#include <iostream>
#include <cstring>
#include <cmath>
#include "basics/util.hpp"
#include "config/factory.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/target_provider.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "mission_processor.hpp"
#include "basics/simulation.hpp"

using namespace std;
using namespace miltech04;

int main(int argc, char** argv)
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <config_file> <ammo_file> <targets_file> <output_file>" << std::endl;  // NOLINT(*-pointer-arithmetic)
        return 1;
    }

    std::unique_ptr<IConfigLoader> configLoader = createLoader(LoaderType::FILE, argv[1], argv[2]);
    if (configLoader == nullptr) {
        LOG("Error: Failed to create config loader");
        return 1;
    }

    std::unique_ptr<ITargetProvider> targetProvider = createProvider(ProviderType::JSON, argv[3]);
    if (targetProvider == nullptr) {
        LOG("Error: Failed to create target provider");
        return 1;
    }

    std::unique_ptr<IBallisticSolver> solver = createSolver(SolverType::ANALYTICAL, configLoader);
    if (solver == nullptr) {
        LOG("Error: Failed to create ballistic solver");
        return 1;
    }

    SimulationResults simulation(MAX_STEPS);
    MissionProcessor processor(configLoader.get(), solver.get(), targetProvider.get());

    while (processor.hasNext()) {
        if (!simulation.push(processor.step())) break;
    }

    LOG("Simulation completed successfully");

    if (!simulation.save(argv[4])) {
        LOG("Failed to save simulation steps");
        return 1;
    }

    return 0;
}
