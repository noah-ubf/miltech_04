#ifndef MILTECH_INCLUDE_FACTORY_HPP
#define MILTECH_INCLUDE_FACTORY_HPP

#include <string>
#include <memory>

namespace miltech04 {

class IConfigLoader;
class ITargetProvider;
class IBallisticSolver;

enum class SolverType   { ANALYTICAL, TABLE };
enum class ProviderType { JSON };
enum class LoaderType   { FILE };
 
std::unique_ptr<IBallisticSolver> createSolver(const SolverType type, IConfigLoader* configLoader, const std::string& param);
std::unique_ptr<ITargetProvider>  createProvider(const ProviderType type, const std::string& targetsSource);
std::unique_ptr<IConfigLoader>    createLoader(const LoaderType type, const std::string& configSource, const std::string& ammoSource);

} // namespace miltech04

#endif // MILTECH_INCLUDE_FACTORY_HPP