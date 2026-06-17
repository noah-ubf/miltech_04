#include "mission_processor.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "basics/const.hpp"
#include "basics/drone.hpp"

using namespace miltech04;

MissionProcessor::MissionProcessor(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets) {
    init(configSource, ballisticSolver, targets);
};

void MissionProcessor::init(IConfigLoader* configSource, IBallisticSolver* ballisticSolver, ITargetProvider* targets) {
    config = configSource;
    solver = ballisticSolver;
    targetProvider = targets;
    reset();
};

void MissionProcessor::setTargetProvider(ITargetProvider* provider) {
    targetProvider = provider;
}

void MissionProcessor::changeSolver(IBallisticSolver* ballisticSolver) {
    solver = ballisticSolver;
};

bool MissionProcessor::hasNext() {
    double hitDistance = (currentStep.aimPoint - currentStep.predictedTarget).length();
    bool res = stepNum < MAX_STEPS && !(
        stepNum > 0 && currentStep.state == MOVING &&
        hitDistance <= 0.5 * config->getConfig().hitRadius
    );
    return res;
};

Drone MissionProcessor::step() {
    double minTime = 1.e99;
    double simTime = config->getConfig().simTimeStep * stepNum;
    targetProvider->setSimTime(simTime);

    while (targetProvider->hasNextTarget()) {
        Target target = targetProvider->getNextTarget();
        Solution solution = solver->solve(currentStep, target);

        if (solution.timeToFire <= minTime) {
            minTime = solution.timeToFire;
            currentStep.targetIdx = target.idx;
            currentStep.dropPoint = solution.firePoint;
            currentStep.predictedTarget = solution.predictedTarget;
            currentStep.aimPoint = solution.aimPoint;
            currentStep.fireDistance = solution.fireDistance;
        }
    }

    ++stepNum;
    Drone prevStep = currentStep;
    currentStep = prevStep.move(config, prevStep.predictedTarget);
    return prevStep;
};

void MissionProcessor::reset() {
    stepNum = 0;
    currentStep.pos = config->getConfig().startPos;
    currentStep.speed = 0;
    currentStep.direction = config->getConfig().initialDir;
    currentStep.state = STOPPED;
    currentStep.targetIdx = -1;
    currentStep.dropPoint = config->getConfig().startPos;
    currentStep.aimPoint = config->getConfig().startPos;
    currentStep.predictedTarget = config->getConfig().startPos;
    targetProvider->setArrayTimeStep(config->getConfig().arrayTimeStep);
};
