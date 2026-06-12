#ifndef MILTECH_INCLUDE_CLASSES_MISSION_PROCESSOR_HPP
#define MILTECH_INCLUDE_CLASSES_MISSION_PROCESSOR_HPP

#include <memory>
#include "interfaces/config_loader.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "interfaces/target_provider.hpp"
#include "basics/simulation.hpp"

namespace miltech04 {

class MissionProcessor {
    IBallisticSolver* solver;
    IConfigLoader* config;
    ITargetProvider* targetProvider;
    int stepNum = 0;
    SimStep currentStep;
    SimStep moveDrone();
public:
    MissionProcessor(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets);
    virtual ~MissionProcessor() = default;
    virtual void init(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets);
    virtual void setTargetProvider(ITargetProvider* provider);
    virtual void changeSolver(IBallisticSolver* ballisticSolver);
    virtual bool hasNext();
    virtual SimStep step();
    virtual void reset();
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_MISSION_PROCESSOR_HPP