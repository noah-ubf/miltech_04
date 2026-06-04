#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include "../include/const.hpp"
#include "../include/util.hpp"
#include "../include/external/json.hpp"
#include "../include/basics/coord.hpp"
#include "../include/basics/drone_config.hpp"
#include "../include/basics/simulation.hpp"
#include "../include/const.hpp"
#include "../include/fabric.hpp"

using json = nlohmann::json;
using namespace std;

void initStep(SimStep& s, const DroneConfig& droneConfig) {
    s.pos = droneConfig.startPos;
    s.speed = 0;
    s.direction = droneConfig.initialDir;
    s.state = STOPPED;
    s.targetIdx = -1;
    s.dropPoint = s.pos;
    s.aimPoint = s.pos;
    s.predictedTarget = s.pos;
}

SimStep moveDrone(const DroneConfig& droneConfig, SimStep step, double hDist) {
    Coord& target = step.predictedTarget;
    Coord dir = { cos(step.direction), sin(step.direction)};
    double targetDirection = atan2(target.y - step.pos.y, target.x - step.pos.x);
    double acceleration = droneConfig.attackSpeed * droneConfig.attackSpeed / (2 * droneConfig.accelPath);
    double distance = (target - step.pos).length();
    if (distance < distance - hDist - droneConfig.accelPath) {
        // if it's too close, must move away first
        targetDirection = addAngles(targetDirection, M_PI);
    }
    double dDir = addAngles(targetDirection, -step.direction);

    // move the drone according to it's state
    switch(step.state) {
        case STOPPED:
            // if direction ok, -> ACCELERATING, else -> TURNING
            if (abs(dDir) <= droneConfig.turnThreshold) {
                step.direction = targetDirection;
                step.state = ACCELERATING;
            } else {
                step.state = TURNING;
            }
            break;
        case ACCELERATING:
            step.pos = step.pos + dir * step.speed * droneConfig.simTimeStep;
            step.speed = step.speed + acceleration * droneConfig.simTimeStep;
            if (step.speed >= droneConfig.attackSpeed) {
                step.speed = droneConfig.attackSpeed;
                step.state = MOVING;
            }
            break;
        case DECELERATING:
            step.pos = step.pos + dir * step.speed * droneConfig.simTimeStep;
            step.speed -= acceleration * droneConfig.simTimeStep;
            if (step.speed <= 0) {
                step.speed = 0;
                step.state = STOPPED;
            }
            break;
        case TURNING:
            if (dDir >= 0) {
                step.direction = addAngles(step.direction, droneConfig.angularSpeed * droneConfig.simTimeStep);
            } else {
                step.direction = addAngles(step.direction, -droneConfig.angularSpeed * droneConfig.simTimeStep);
            }
            dDir = addAngles(targetDirection, -step.direction);
            if (abs(dDir) < droneConfig.turnThreshold) {
                step.direction = targetDirection;
                step.state = STOPPED;
            }
            break;
        case MOVING:
            step.pos = step.pos + dir * step.speed * droneConfig.simTimeStep;
            if (abs(dDir) > droneConfig.turnThreshold) {
                step.state = DECELERATING;
            }
            break;
    }
    return step;
}

int runSimulation(IConfigLoader* configLoader, ITargetProvider* targetProvider, IBallisticSolver* solver, SimSteps& simulationSteps) {
    int stepNum = 0;
    SimStep currentStep;
    initStep(currentStep, configLoader->getConfig());
    do {
        double minTime = 1.e99;
        double simTime = configLoader->getConfig().simTimeStep * stepNum;
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
        simulationSteps.stepCount = stepNum;
        simulationSteps.steps[stepNum] = currentStep;

        if (currentStep.state == MOVING &&
            (currentStep.aimPoint - currentStep.predictedTarget).length() <= 0.5 * configLoader->getConfig().hitRadius) {
            break;
        }

        currentStep = moveDrone(configLoader->getConfig(), currentStep, solver->getFireDistance());

        ++stepNum;
    } while (simulationSteps.stepCount < MAX_STEPS);
    return 0;
}

int saveSimSteps(const SimSteps& simulationSteps) {
    json out;
    out["totalSteps"] = simulationSteps.stepCount;
    out["steps"] = json::array();
    for (int i = 0; i < simulationSteps.stepCount; i++) {
        SimStep s = simulationSteps.steps[i];
        json step;
        step["position"]        = {{"x", s.pos.x}, {"y", s.pos.y}};
        step["direction"]       = s.direction;
        step["state"]           = s.state;
        #ifdef ENABLE_DEBUG
            step["stateName"]       = STATE_NAMES[s.state];
        #endif
        step["targetIndex"]     = s.targetIdx;
        step["dropPoint"]       = {{"x", s.dropPoint.x}, {"y", s.dropPoint.y}};
        step["aimPoint"]        = {{"x", s.aimPoint.x}, {"y", s.aimPoint.y}};
        step["predictedTarget"] = {{"x", s.predictedTarget.x}, {"y", s.predictedTarget.y}};
        out["steps"].push_back(step);
    }
    std::ofstream fout("simulation.json");
    if (!fout.is_open()) {
        LOG("Error: Unable to open simulation.json file");
        return 1;
    }
    fout << out.dump(2);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <config_file> <ammo_file> <targets_file>" << std::endl;  // NOLINT(*-pointer-arithmetic)
        return 1;
    }

    SimSteps simulationSteps;

    IConfigLoader* configLoader = createLoader(LoaderType::FILE, argv[1], argv[2]);
    if (configLoader == nullptr) {
        LOG("Error: Failed to create config loader");
        return 1;
    }

    if (!configLoader->load()) {
        delete configLoader;
        return 1;
    }

    ITargetProvider* targetProvider = createProvider(ProviderType::JSON, argv[3]);
    if (targetProvider == nullptr) {
        LOG("Error: Failed to create target provider");
        delete configLoader;
        return 1;
    }

    if (!targetProvider->isLoaded()) {
        delete configLoader;
        delete targetProvider;
        return 1;
    }

    targetProvider->setArrayTimeStep(configLoader->getConfig().arrayTimeStep);

    IBallisticSolver* solver = createSolver(SolverType::ANALYTICAL, configLoader);
    if (solver == nullptr) {
        LOG("Error: Failed to create ballistic solver");
        delete configLoader;
        delete targetProvider;
        return 1;
    }

    if (!solver->isValid()) {
        LOG("Error: Ballistic solver is not valid");
        delete configLoader;
        delete targetProvider;
        delete solver;
        return 1;
    }

    simulationSteps.steps = new SimStep[MAX_STEPS];

    if (runSimulation(configLoader, targetProvider, solver, simulationSteps) == 1) {
        LOG("Simulation failed");
        delete[] simulationSteps.steps;
        delete configLoader;
        delete targetProvider;
        delete solver;
        return 1;
    }

    LOG("Simulation completed successfully");

    if (saveSimSteps(simulationSteps) == 1) {
        LOG("Failed to save simulation steps");
        delete[] simulationSteps.steps;
        delete configLoader;
        delete targetProvider;
        delete solver;
        return 1;
    }

    delete[] simulationSteps.steps;
    delete configLoader;
    delete targetProvider;
    delete solver;
    return 0;
}

