#include "../include/config/fabric.hpp"
#include "../include/interfaces/config_loader.hpp"
#include "../include/interfaces/target_provider.hpp"
#include "../include/interfaces/ballistic_solver.hpp"
#include "../include/config/file_config_loader.hpp"
#include "../include/providers/json_target_provider.hpp"
#include "../include/solvers/analytical_solver.hpp"

using namespace miltech04;

namespace miltech04 {

IConfigLoader* createLoader(const LoaderType type, const std::string& configSource, const std::string& ammoSource) {
    switch (type) {
        case LoaderType::FILE:
            return new FileConfigLoader(configSource, ammoSource);
        default:
            return nullptr;
    }
}

ITargetProvider* createProvider(const ProviderType type, const std::string& targetsSource) {
    switch (type) {
        case ProviderType::JSON:
            return new JsonTargetProvider(targetsSource);
        default:
            return nullptr;
    }
}

IBallisticSolver* createSolver(const SolverType type, IConfigLoader* configLoader) {
    switch (type) {
        case SolverType::ANALYTICAL:
            return new AnalyticalSolver(configLoader);
        default:
            return nullptr;
    }
};

} // namespace miltech04