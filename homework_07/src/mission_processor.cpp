#include "mission_processor.hpp"
#include <memory>
#include "basics/drone_state_enum.hpp"
#include "drone_states/drone_state_stopped.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "basics/const.hpp"
#include "basics/drone_context.hpp"
#include "interfaces/drone_state.hpp"

using namespace miltech04;

const double MAX_POSSIBLE_TIME = 1.e99;

MissionProcessor::MissionProcessor(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets) {
    init(configSource, ballisticSolver, targets);
};

void MissionProcessor::init(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets) {
    config = configSource;
    solver = ballisticSolver;
    targetProvider = targets;
    ctx.config = config;
    ctx.state = state->getState();
    ctx.stateName = state->getStateName();
    reset();
};

void MissionProcessor::setTargetProvider(ITargetProvider* provider) {
    targetProvider = provider;
}

void MissionProcessor::changeSolver(IBallisticSolver* ballisticSolver) {
    solver = ballisticSolver;
};

bool MissionProcessor::hasNext() {
    double hitDistance = (ctx.aimPoint - ctx.predictedTarget).length();
    bool res = stepNum < MAX_STEPS && !(
        stepNum > 0 && ctx.state == EDroneState::MOVING &&
        hitDistance <= 0.25 * config->getConfig().hitRadius
    );
    return res;
};

DroneContext MissionProcessor::step() {
    double minTime = MAX_POSSIBLE_TIME;
    double simTime = config->getConfig().simTimeStep * stepNum;
    targetProvider->setSimTime(simTime);

    while (targetProvider->hasNextTarget()) {
        Target target = targetProvider->getNextTarget();
        Solution solution = solver->solve(ctx, target);

        if (solution.timeToFire <= minTime) {
            minTime = solution.timeToFire;
            ctx.targetIdx = target.idx;
            ctx.dropPoint = solution.firePoint;
            ctx.predictedTarget = solution.predictedTarget;
            ctx.aimPoint = solution.aimPoint;
            ctx.fireDistance = solution.fireDistance;
        }
    }

    ++stepNum;
    auto prevState = ctx;
    state = state->execute(ctx);
    ctx.state = state->getState();
    ctx.stateName = state->getStateName();
    ctx.isFinished = !hasNext();
    return prevState;
};

void MissionProcessor::reset() {
    stepNum = 0;
    state = std::make_unique<DroneStateStopped>();
    ctx.config = config;
    ctx.pos = config->getConfig().startPos;
    ctx.speed = 0;
    ctx.direction = config->getConfig().initialDir;
    ctx.targetIdx = -1;
    ctx.dropPoint = config->getConfig().startPos;
    ctx.aimPoint = config->getConfig().startPos;
    ctx.predictedTarget = config->getConfig().startPos;
    targetProvider->setArrayTimeStep(config->getConfig().arrayTimeStep);
};
