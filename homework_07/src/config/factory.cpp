#include "config/factory.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/target_provider.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "config/file_config_loader.hpp"
#include "providers/json_target_provider.hpp"
#include "solvers/analytical_solver.hpp"
#include "solvers/ballistic_table_solver.hpp"

using namespace miltech04;

namespace miltech04 {

std::unique_ptr<IConfigLoader> createLoader(const LoaderType type, const std::string& configSource, const std::string& ammoSource) {
    std::unique_ptr<IConfigLoader> loader = nullptr;
    switch (type) {
        case LoaderType::FILE:
            loader = std::unique_ptr<IConfigLoader>(new FileConfigLoader(configSource, ammoSource));
            break;
        default:
            return nullptr;
    }
 
    if (!loader->load()) {
        return nullptr;
    }
 
    return std::move(loader);
}

std::unique_ptr<ITargetProvider> createProvider(const ProviderType type, const std::string& targetsSource) {
    std::unique_ptr<ITargetProvider> provider = nullptr;
    switch (type) {
        case ProviderType::JSON:
            provider = std::unique_ptr<ITargetProvider>(new JsonTargetProvider(targetsSource));
            break;
        default:
            return nullptr;
    }

    if (!provider->isLoaded()) {
        return nullptr;
    }

    return std::move(provider);
}

std::unique_ptr<IBallisticSolver> createSolver(const SolverType type, IConfigLoader* configLoader, const std::string& param) {
    std::unique_ptr<IBallisticSolver> solver = nullptr;
    switch (type) {
        case SolverType::ANALYTICAL:
            solver = std::unique_ptr<IBallisticSolver>(new AnalyticalSolver(configLoader, param));
            break;
        case SolverType::TABLE:
            solver = std::unique_ptr<IBallisticSolver>(new BallisticTableSolver(configLoader, param));
            break;
        default:
            return nullptr;
    }

    if (!solver->isValid()) {
        return nullptr;
    }

    return std::move(solver);
};

} // namespace miltech04