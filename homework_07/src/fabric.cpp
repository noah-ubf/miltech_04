#include "../include/fabric.hpp"
#include "../include/interfaces/config_loader.hpp"
#include "../include/interfaces/target_provider.hpp"
#include "../include/interfaces/ballistic_solver.hpp"
#include "../include/core/file_config_loader.hpp"
#include "../include/core/json_target_provider.hpp"
#include "../include/core/analytical_solver.hpp"

using namespace miltech04;

IConfigLoader* createLoader(LoaderType type, const char* configSource, const char* ammoSource) {
    switch (type) {
        case LoaderType::FILE:
            return new FileConfigLoader(configSource, ammoSource);
        default:
            return nullptr;
    }
}

ITargetProvider* createProvider(ProviderType type, const char* targetsSource) {
    switch (type) {
        case ProviderType::JSON:
            return new JsonTargetProvider(targetsSource);
        default:
            return nullptr;
    }
}

IBallisticSolver* createSolver(SolverType type, IConfigLoader* configLoader) {
    switch (type) {
        case SolverType::ANALYTICAL:
            return new AnalyticalSolver(configLoader);
        default:
            return nullptr;
    }
};
