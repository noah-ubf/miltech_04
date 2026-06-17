#include "basics/util.hpp"
#include "external/json.hpp"
#include "interfaces/config_loader.hpp"
#include "basics/drone.hpp"

using json = nlohmann::json;
using namespace miltech04;

double Drone::getTimeToFirePointFromStopped(IConfigLoader* config, const Coord& target) const {
    const double attackSpeed = config->getConfig().attackSpeed;
    const double accelPath = config->getConfig().accelPath;
    const double angularSpeed = config->getConfig().angularSpeed;

    double targetDirection = atan2(target.y - pos.y, target.x - pos.x);
    double dDir = addAngles(targetDirection, -direction);
    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    double accelTime = attackSpeed / acceleration;
    double distance = (target - pos).length();

    if (distance >= fireDistance + accelPath) { // turn and go
        double turnTime = fabs(dDir) / angularSpeed;
        return turnTime + accelTime + (distance - fireDistance - accelPath) / attackSpeed;
    } else { // need to move away, turn back and go
        double turnAwayTime = (M_PI - fabs(dDir)) / angularSpeed;
        double turnBackTime = M_PI / angularSpeed;
        return turnAwayTime + 2 * accelTime + turnBackTime + (accelPath + fireDistance - distance) / attackSpeed;
    }
}

double Drone::getTimeToFirePoint(IConfigLoader* config, const Coord& target) const {
    const double attackSpeed = config->getConfig().attackSpeed;
    const double accelPath = config->getConfig().accelPath;
    const double angularSpeed = config->getConfig().angularSpeed;
    const double turnThreshold = config->getConfig().turnThreshold;

    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    Drone step = {
        .pos = pos,
        .speed = speed,
        .direction = direction,
        .fireDistance = fireDistance,
        .state = state,
        .targetIdx = targetIdx,
        .dropPoint = dropPoint,
        .aimPoint = aimPoint,
        .predictedTarget = predictedTarget
    };
    Coord dir = Coord{ cos(direction), sin(direction) };

    switch (state) {
        case ACCELERATING: { // accelerate, decelerate, go from stopped state
            double accelPath = 0.5 * (attackSpeed * attackSpeed - speed * speed) / acceleration;
            double accelDecelTime = (attackSpeed - speed) / acceleration + attackSpeed / acceleration;
            step.pos = pos + dir * (accelPath + accelPath);
            return accelDecelTime + step.getTimeToFirePointFromStopped(config, target);
            break;
        }
        case MOVING: {
            double distance = (target - pos).length();
            double targetDirection = atan2(target.y - pos.y, target.x - pos.x);
            double dDir = addAngles(targetDirection, -direction);
            if (fabs(dDir) <= turnThreshold) {
                if (distance >= fireDistance) { // if the drone is already moving in the right direction, just calculate time to target
                    return (distance - fireDistance) / attackSpeed;
                } else { // need to move to the target + flight distance, decelerate, go from stopped state
                    step.pos = pos + dir * (distance + fireDistance + accelPath);
                    double moveTime = (distance + fireDistance) / attackSpeed;
                    double decelTime = attackSpeed / acceleration;
                    return moveTime + decelTime + step.getTimeToFirePointFromStopped(config, target);
                }
            } else { // decelerate, go from stopped state
                step.pos = pos + dir * accelPath;
                double decelTime = attackSpeed / acceleration;
                return decelTime + step.getTimeToFirePointFromStopped(config, target);
            } 
            break;
        }
        case DECELERATING: { // decelerate, turn, accelerate, move
            double decelPath = 0.5 * speed * speed / acceleration;
            double decelTime = speed / acceleration;
            step.pos = pos + dir * decelPath;
            return decelTime + step.getTimeToFirePointFromStopped(config, target);
            break;
        }
        case STOPPED:
        case TURNING:
        default: {
            return step.getTimeToFirePointFromStopped(config, target);
            break;
        }
    }
}

Drone Drone::move(IConfigLoader* config, const Coord& target) const {
    Drone res;
    res.targetIdx = targetIdx;
    res.dropPoint = dropPoint;
    res.aimPoint = aimPoint;
    res.predictedTarget = predictedTarget;
    res.pos = pos;
    res.state = state;
    res.speed = speed;
    res.direction = direction;
    Coord dir = { cos(direction), sin(direction)};
    double targetDirection = atan2(target.y - pos.y, target.x - pos.x);
    double acceleration = config->getConfig().attackSpeed * config->getConfig().attackSpeed / (2 * config->getConfig().accelPath);
    double distance = (target - pos).length();
    double dDir = addAngles(targetDirection, -direction);

    // move the drone according to it's state
    switch(state) {
        case STOPPED:
            // if direction ok, -> ACCELERATING, else -> TURNING
            if (fabs(dDir) <= config->getConfig().turnThreshold) {
                res.direction = targetDirection;
                res.state = ACCELERATING;
            } else {
                res.state = TURNING;
            }
            break;
        case ACCELERATING:
            res.pos = res.pos + dir * speed * config->getConfig().simTimeStep;
            res.speed += acceleration * config->getConfig().simTimeStep;
            if (res.speed >= config->getConfig().attackSpeed) {
                res.speed = config->getConfig().attackSpeed;
                res.state = MOVING;
            }
            break;
        case DECELERATING:
            res.pos = res.pos + dir * speed * config->getConfig().simTimeStep;
            res.speed -= acceleration * config->getConfig().simTimeStep;
            if (res.speed <= 0) {
                res.speed = 0;
                res.state = STOPPED;
            }
            break;
        case TURNING:
            if (dDir >= 0) {
                res.direction = addAngles(res.direction, config->getConfig().angularSpeed * config->getConfig().simTimeStep);
            } else {
                res.direction = addAngles(res.direction, -config->getConfig().angularSpeed * config->getConfig().simTimeStep);
            }
            dDir = addAngles(targetDirection, -res.direction);
            if (fabs(dDir) < config->getConfig().turnThreshold) {
                res.direction = targetDirection;
                res.state = STOPPED;
            }
            break;
        case MOVING:
            res.pos = res.pos + dir * res.speed * config->getConfig().simTimeStep;
            if (fabs(dDir) > config->getConfig().turnThreshold) {
                res.state = DECELERATING;
            }
            break;
    }
    return res;
}