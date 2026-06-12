#ifndef MILTECH_INCLUDE_FACTORY_HPP
#define MILTECH_INCLUDE_FACTORY_HPP

#include <string>
#include <memory>
#include "interfaces/config_loader.hpp"
#include "interfaces/target_provider.hpp"
#include "interfaces/ballistic_solver.hpp"

namespace miltech04 {

enum class SolverType   { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType   { FILE };
 
std::unique_ptr<IBallisticSolver> createSolver(const SolverType type, std::unique_ptr<IConfigLoader>& configLoader);
std::unique_ptr<ITargetProvider>  createProvider(const ProviderType type, const std::string& targetsSource);
std::unique_ptr<IConfigLoader>    createLoader(const LoaderType type, const std::string& configSource, const std::string& ammoSource);

} // namespace miltech04

#endif // MILTECH_INCLUDE_FACTORY_HPP