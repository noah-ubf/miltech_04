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
    if (argc != 5 && argc != 6) {
        std::cout << "Usage: " << argv[0] << " <config_file> <ammo_file> <targets_file> <output_file> [solver_param]" << std::endl;  // NOLINT(*-pointer-arithmetic)
        std::cout << "If [solver_param] is provided, the table-based solver will be used." << std::endl;
        std::cout << "If [solver_param] is empty, the analytical solver will be used." << std::endl;
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

    std::unique_ptr<IBallisticSolver> solver = createSolver(argc == 6 ? SolverType::TABLE : SolverType::ANALYTICAL, configLoader.get(), argc == 6 ? argv[5] : "");
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
