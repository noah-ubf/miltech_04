#include "../include/config/fabric.hpp"
#include "../include/interfaces/config_loader.hpp"
#include "../include/interfaces/target_provider.hpp"
#include "../include/interfaces/ballistic_solver.hpp"
#include "../include/config/file_config_loader.hpp"
#include "../include/providers/json_target_provider.hpp"
#include "../include/solvers/analytical_solver.hpp"

using namespace miltech04;

namespace miltech04 {

std::unique_ptr<IConfigLoader> createLoader(const LoaderType type, const std::string& configSource, const std::string& ammoSource) {
    switch (type) {
        case LoaderType::FILE:
            return std::make_unique<FileConfigLoader>(configSource, ammoSource);
        default:
            return nullptr;
    }
}

std::unique_ptr<ITargetProvider> createProvider(const ProviderType type, const std::string& targetsSource) {
    switch (type) {
        case ProviderType::JSON:
            return std::make_unique<JsonTargetProvider>(targetsSource);
        default:
            return nullptr;
    }
}

std::unique_ptr<IBallisticSolver> createSolver(const SolverType type, std::unique_ptr<IConfigLoader>& configLoader) {
    switch (type) {
        case SolverType::ANALYTICAL:
            return std::make_unique<AnalyticalSolver>(configLoader.get());
        default:
            return nullptr;
    }
};

} // namespace miltech04