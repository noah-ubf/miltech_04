#ifndef MILTECH_INCLUDE_CLASSES_MISSION_PROCESSOR_HPP
#define MILTECH_INCLUDE_CLASSES_MISSION_PROCESSOR_HPP

#include <memory>
#include "drone_states/drone_state_stopped.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "interfaces/target_provider.hpp"
#include "basics/drone_context.hpp"
#include "interfaces/drone_state.hpp"

namespace miltech04 {

class MissionProcessor {
    IBallisticSolver* solver;
    IConfigLoader* config;
    ITargetProvider* targetProvider;
    int stepNum = 0;
    DroneContext ctx;
    std::unique_ptr<IDroneState> state = std::make_unique<DroneStateStopped>();
public:
    MissionProcessor(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets);
    virtual ~MissionProcessor() = default;
    virtual void init(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets);
    virtual void setTargetProvider(ITargetProvider* provider);
    virtual void changeSolver(IBallisticSolver* ballisticSolver);
    virtual bool hasNext();
    virtual DroneContext step();
    virtual void reset();
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_MISSION_PROCESSOR_HPP