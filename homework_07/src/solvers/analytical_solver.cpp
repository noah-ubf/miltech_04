#include <cmath>

#include <memory>
#include "solvers/analytical_solver.hpp"
#include "basics/util.hpp"
#include "basics/simulation.hpp"

using namespace miltech04;

AnalyticalSolver::AnalyticalSolver(IConfigLoader* configSource) {
    this->configSource = configSource;
    init();
};

void AnalyticalSolver::init() {
    flightTime = calcFlightTime();
    if (flightTime < 0) {
        LOG("Error: Incorrect flight time; no solution");
        return;
    }
    hDist = calcFireDistance();
};

bool AnalyticalSolver::isValid() {
    return flightTime > 0 && hDist > 0;
};

double AnalyticalSolver::getFireDistance() {
    return hDist;
};

double AnalyticalSolver::calcFlightTime() {
    const double attackSpeed = configSource->getConfig().attackSpeed;
    const double altitude = configSource->getConfig().altitude;
    const double mass = configSource->getAmmoParams().mass;
    const double drag = configSource->getAmmoParams().drag;
    const double lift = configSource->getAmmoParams().lift;


    double a = drag * g * mass - 2 * drag * drag * lift * attackSpeed;
    if (a == 0.f) { // a appears in denominator, so it cannot be zero
        LOG("Error: Incorrect a; no solution");
        return -1;
    }
    double b = -3 * g * mass * mass + 3 * drag * lift * mass * attackSpeed;
    double c = 6 * mass * mass * altitude;
    double p = -b * b / (3 * a * a);
    if (p >= 0) { // inversed p appears as sqrt parameter, so p cannot be positive; p appears in denominator so it cannot be zero
        LOG("Error: Incorrect p; no solution");
        return -1;
    }
    double q = 2 * b * b * b / (27 * a * a * a) + c / a;
    double sqrtP3 = sqrt(-p / 3);
    double temp = 3 * q / (2 * p * sqrtP3);
    if (temp > 1 || temp < -1) {
        LOG("Error: Argument of acos out of bounds [-1;1]");
        return -1;
    }
    double fi = acos(temp);
    return 2 * sqrtP3 * cos( (fi + 4 * M_PI) / 3 ) - b / (3 * a);
};

double AnalyticalSolver::calcFireDistance() {
    const double attackSpeed = configSource->getConfig().attackSpeed;
    const double mass = configSource->getAmmoParams().mass;
    const double drag = configSource->getAmmoParams().drag;
    const double lift = configSource->getAmmoParams().lift;
    // helper variables to reduce repeated multiplications
    // and make the final formula more concise:
    double m2 = mass * mass;
    double m3 = m2 * mass;
    double m4 = m3 * mass;
    double l2 = lift * lift;
    double l3 = l2 * lift;
    double l4 = l3 * lift;
    double d2 = drag * drag;
    double d3 = d2 * drag;
    double d4 = d3 * drag;
    double ft2 = flightTime * flightTime;
    double ft3 = ft2 * flightTime;
    double ft4 = ft3 * flightTime;
    double ft5 = ft4 * flightTime;
    double dglm = drag * lift * g * mass;

    double hDist = flightTime * attackSpeed
        - ft2 * drag * attackSpeed /(2 * mass)
        + ft3 * (6 * dglm - 6 * d2 * (l2 - 1) * attackSpeed) / (36 * m2)
        + ft4 * (
            -6 * drag * dglm * (1 + l2 + l4)
            + 3 * d3 * l2 * (1 + l2) * attackSpeed
            + 6 * d3 * l4 * (1 + l2) * attackSpeed
        )  / (36 * (1 + l2) * (1 + l2) * m3)
        + ft5 * (3 * d2 * dglm * l2 - 3 * d4 * l2 * (1 + l2) * attackSpeed) / (36 * (1 + l2) * m4);

    return hDist;
};

Solution AnalyticalSolver::solve(const SimStep& drone, const Target& target) const {
    Solution result;
    const double totalTime0 = getTimeToFirePoint(drone, target.pos);
    Coord predicted = target.pos + target.velocity * (totalTime0 + flightTime);
    result.timeToFire = getTimeToFirePoint(drone, predicted);
    Coord delta = predicted - drone.pos;
    result.firePoint = predicted - delta.normalize() * hDist;
    result.predictedTarget = target.pos + target.velocity * flightTime;
    return result;
};

double AnalyticalSolver::getTimeToFirePointFromStopped(const SimStep& drone, const Coord& target) const {
    const double attackSpeed = configSource->getConfig().attackSpeed;
    const double accelPath = configSource->getConfig().accelPath;
    const double angularSpeed = configSource->getConfig().angularSpeed;

    double targetDirection = atan2(target.y - drone.pos.y, target.x - drone.pos.x);
    double dDir = addAngles(targetDirection, -drone.direction);
    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    double accelTime = attackSpeed / acceleration;
    double distance = (target - drone.pos).length();

    if (distance >= hDist + accelPath) { // turn and go
        double turnTime = fabs(dDir) / angularSpeed;
        return turnTime + accelTime + (distance - hDist - accelPath) / attackSpeed;
    } else { // need to move away, turn back and go
        double turnAwayTime = (M_PI - fabs(dDir)) / angularSpeed;
        double turnBackTime = M_PI / angularSpeed;
        return turnAwayTime + 2 * accelTime + turnBackTime + (accelPath + hDist - distance) / attackSpeed;
    }
}

double AnalyticalSolver::getTimeToFirePoint(const SimStep& drone, const Coord target) const {
    const double attackSpeed = configSource->getConfig().attackSpeed;
    const double accelPath = configSource->getConfig().accelPath;
    const double angularSpeed = configSource->getConfig().angularSpeed;
    const double turnThreshold = configSource->getConfig().turnThreshold;

    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    SimStep step = drone;
    Coord pos = drone.pos;
    Coord dir = Coord{ cos(drone.direction), sin(drone.direction) };

    switch (drone.state) {
        case ACCELERATING: { // accelerate, decelerate, go from stopped state
            double accelPath = 0.5 * (attackSpeed * attackSpeed - drone.speed * drone.speed) / acceleration;
            double accelDecelTime = (attackSpeed - drone.speed) / acceleration + attackSpeed / acceleration;
            step.pos = pos + dir * (accelPath + accelPath);
            return accelDecelTime + getTimeToFirePointFromStopped(step, target);
            break;
        }
        case MOVING: {
            double distance = (target - pos).length();
            double targetDirection = atan2(target.y - drone.pos.y, target.x - drone.pos.x);
            double dDir = addAngles(targetDirection, -drone.direction);
            if (fabs(dDir) <= turnThreshold) {
                if (distance >= hDist) { // if the drone is already moving in the right direction, just calculate time to target
                    return (distance - hDist) / attackSpeed;
                } else { // need to move to the target + flight distance, decelerate, go from stopped state
                    step.pos = pos + dir * (distance + hDist + accelPath);
                    double moveTime = (distance + hDist) / attackSpeed;
                    double decelTime = attackSpeed / acceleration;
                    return moveTime + decelTime + getTimeToFirePointFromStopped(step, target);
                }
            } else { // decelerate, go from stopped state
                step.pos = pos + dir * accelPath;
                double decelTime = attackSpeed / acceleration;
                return decelTime + getTimeToFirePointFromStopped(step, target);
            } 
            break;
        }
        case DECELERATING: { // decelerate, turn, accelerate, move
            double decelPath = 0.5 * drone.speed * drone.speed / acceleration;
            double decelTime = drone.speed / acceleration;
            step.pos = pos + dir * decelPath;
            return decelTime + getTimeToFirePointFromStopped(step, target);
            break;
        }
        case STOPPED:
        case TURNING: {
            return getTimeToFirePointFromStopped(step, target);
            break;
        }
    }
}
