#include "mission_processor.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "basics/const.hpp"
#include "basics/util.hpp"
#include "basics/simulation.hpp"

#include <memory>
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

SimStep MissionProcessor::step() {
    double minTime = 1.e99;
    double simTime = config->getConfig().simTimeStep * stepNum;
    Coord dir = { cos(currentStep.direction), sin(currentStep.direction)};
    currentStep.aimPoint = currentStep.pos + dir * solver->getFireDistance();
    targetProvider->setSimTime(simTime);

    while (targetProvider->hasNextTarget()) {
        Target target = targetProvider->getNextTarget();
        Solution solution = solver->solve(currentStep, target);

        if (solution.timeToFire <= minTime) {
            minTime = solution.timeToFire;
            currentStep.targetIdx = target.idx;
            currentStep.dropPoint = solution.firePoint;
            currentStep.predictedTarget = solution.predictedTarget;
        }
    }

    ++stepNum;
    SimStep prevStep = currentStep;
    currentStep = moveDrone();
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

SimStep MissionProcessor::moveDrone() {
    SimStep res;
    res.targetIdx = currentStep.targetIdx;
    res.dropPoint = currentStep.dropPoint;
    res.aimPoint = currentStep.aimPoint;
    res.predictedTarget = currentStep.predictedTarget;
    Coord pos = currentStep.pos;
    DroneState state = currentStep.state;
    double speed = currentStep.speed;
    double direction = currentStep.direction;
    Coord& target = currentStep.predictedTarget;
    Coord dir = { cos(direction), sin(direction)};
    double targetDirection = atan2(target.y - pos.y, target.x - pos.x);
    double acceleration = config->getConfig().attackSpeed * config->getConfig().attackSpeed / (2 * config->getConfig().accelPath);
    double distance = (target - pos).length();
    if (distance < distance - solver->getFireDistance() - config->getConfig().accelPath) {
        // if it's too close, must move away first
        targetDirection = addAngles(targetDirection, M_PI);
    }
    double dDir = addAngles(targetDirection, -direction);

    // move the drone according to it's state
    switch(state) {
        case STOPPED:
            // if direction ok, -> ACCELERATING, else -> TURNING
            if (fabs(dDir) <= config->getConfig().turnThreshold) {
                direction = targetDirection;
                state = ACCELERATING;
            } else {
                state = TURNING;
            }
            break;
        case ACCELERATING:
            pos = pos + dir * speed * config->getConfig().simTimeStep;
            speed = speed + acceleration * config->getConfig().simTimeStep;
            if (speed >= config->getConfig().attackSpeed) {
                speed = config->getConfig().attackSpeed;
                state = MOVING;
            }
            break;
        case DECELERATING:
            pos = pos + dir * speed * config->getConfig().simTimeStep;
            speed = speed - acceleration * config->getConfig().simTimeStep;
            if (speed <= 0) {
                speed = 0;
                state = STOPPED;
            }
            break;
        case TURNING:
            if (dDir >= 0) {
                direction = addAngles(direction, config->getConfig().angularSpeed * config->getConfig().simTimeStep);
            } else {
                direction = addAngles(direction, -config->getConfig().angularSpeed * config->getConfig().simTimeStep);
            }
            dDir = addAngles(targetDirection, -direction);
            if (fabs(dDir) < config->getConfig().turnThreshold) {
                direction = targetDirection;
                state = STOPPED;
            }
            break;
        case MOVING:
            pos = pos + dir * speed * config->getConfig().simTimeStep;
            if (fabs(dDir) > config->getConfig().turnThreshold) {
                state = DECELERATING;
            }
            break;
    }
    res.state = state;
    res.pos = pos;
    res.direction = direction;
    res.speed = speed;
    return res;
}
