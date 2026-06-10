#ifndef MILTECH_INCLUDE_FABRIC_HPP
#define MILTECH_INCLUDE_FABRIC_HPP

#include <string>
#include "../interfaces/config_loader.hpp"
#include "../interfaces/target_provider.hpp"
#include "../interfaces/ballistic_solver.hpp"

namespace miltech04 {

enum class SolverType   { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType   { FILE };
 
IBallisticSolver* createSolver(const SolverType type, IConfigLoader* configLoader);
ITargetProvider*  createProvider(const ProviderType type, const std::string& targetsSource);
IConfigLoader*    createLoader(const LoaderType type, const std::string& configSource, const std::string& ammoSource);

} // namespace miltech04

#endif // MILTECH_INCLUDE_FABRIC_HPP