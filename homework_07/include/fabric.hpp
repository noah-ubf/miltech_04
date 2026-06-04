#ifndef MILTECH_INCLUDE_FABRIC_HPP
#define MILTECH_INCLUDE_FABRIC_HPP

#include "interfaces/config_loader.hpp"
#include "interfaces/target_provider.hpp"
#include "interfaces/ballistic_solver.hpp"

enum class SolverType   { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType   { FILE };
 
IBallisticSolver* createSolver(SolverType type, IConfigLoader* configLoader);
ITargetProvider*  createProvider(ProviderType type, const char* targetsSource);
IConfigLoader*    createLoader(LoaderType type, const char* configSource, const char* ammoSource);

#endif // MILTECH_INCLUDE_FABRIC_HPP