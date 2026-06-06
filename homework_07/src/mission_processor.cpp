#include "../include/core/mission_processor.hpp"
#include "../include/interfaces/config_loader.hpp"
#include "../include/interfaces/ballistic_solver.hpp"
#include "../include/const.hpp"
#include "../include/util.hpp"
#include "basics/simulation.hpp"

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
            break;
        }
    }

    ++stepNum;
    SimStep prevStep = currentStep;
    moveDrone();
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

void MissionProcessor::moveDrone() {
    Coord& target = currentStep.predictedTarget;
    Coord dir = { cos(currentStep.direction), sin(currentStep.direction)};
    double targetDirection = atan2(target.y - currentStep.pos.y, target.x - currentStep.pos.x);
    double acceleration = config->getConfig().attackSpeed * config->getConfig().attackSpeed / (2 * config->getConfig().accelPath);
    double distance = (target - currentStep.pos).length();
    if (distance < distance - solver->getFireDistance() - config->getConfig().accelPath) {
        // if it's too close, must move away first
        targetDirection = addAngles(targetDirection, M_PI);
    }
    double dDir = addAngles(targetDirection, -currentStep.direction);

    // move the drone according to it's state
    switch(currentStep.state) {
        case STOPPED:
            // if direction ok, -> ACCELERATING, else -> TURNING
            if (fabs(dDir) <= config->getConfig().turnThreshold) {
                currentStep.direction = targetDirection;
                currentStep.state = ACCELERATING;
            } else {
                currentStep.state = TURNING;
            }
            break;
        case ACCELERATING:
            currentStep.pos = currentStep.pos + dir * currentStep.speed * config->getConfig().simTimeStep;
            currentStep.speed = currentStep.speed + acceleration * config->getConfig().simTimeStep;
            if (currentStep.speed >= config->getConfig().attackSpeed) {
                currentStep.speed = config->getConfig().attackSpeed;
                currentStep.state = MOVING;
            }
            break;
        case DECELERATING:
            currentStep.pos = currentStep.pos + dir * currentStep.speed * config->getConfig().simTimeStep;
            currentStep.speed -= acceleration * config->getConfig().simTimeStep;
            if (currentStep.speed <= 0) {
                currentStep.speed = 0;
                currentStep.state = STOPPED;
            }
            break;
        case TURNING:
            if (dDir >= 0) {
                currentStep.direction = addAngles(currentStep.direction, config->getConfig().angularSpeed * config->getConfig().simTimeStep);
            } else {
                currentStep.direction = addAngles(currentStep.direction, -config->getConfig().angularSpeed * config->getConfig().simTimeStep);
            }
            dDir = addAngles(targetDirection, -currentStep.direction);
            if (fabs(dDir) < config->getConfig().turnThreshold) {
                currentStep.direction = targetDirection;
                currentStep.state = STOPPED;
            }
            break;
        case MOVING:
            currentStep.pos = currentStep.pos + dir * currentStep.speed * config->getConfig().simTimeStep;
            if (fabs(dDir) > config->getConfig().turnThreshold) {
                currentStep.state = DECELERATING;
            }
            break;
    }
}
