#include <cmath>

#include "interfaces/config_loader.hpp"
#include "solvers/analytical_solver.hpp"
#include "basics/const.hpp"
#include "basics/util.hpp"
#include "basics/drone_context.hpp"

using namespace miltech04;

AnalyticalSolver::AnalyticalSolver(IConfigLoader* configSource, std::string param) {
    config = configSource;
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

double AnalyticalSolver::calcFlightTime() {
    const double attackSpeed = config->getConfig().attackSpeed;
    const double altitude = config->getConfig().altitude;
    const double mass = config->getAmmoParams().mass;
    const double drag = config->getAmmoParams().drag;
    const double lift = config->getAmmoParams().lift;

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
    const double attackSpeed = config->getConfig().attackSpeed;
    const double mass = config->getAmmoParams().mass;
    const double drag = config->getAmmoParams().drag;
    const double lift = config->getAmmoParams().lift;
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

Solution AnalyticalSolver::solve(const DroneContext& ctx, const Target& target) const {
    Solution result;
    Coord dir = { cos(ctx.direction), sin(ctx.direction)};
    result.aimPoint = ctx.pos + dir * hDist;
    const double totalTime0 = ctx.getTimeToFirePoint(config, target.pos);
    Coord predicted = target.pos + target.velocity * (totalTime0 + flightTime);
    result.timeToFire = ctx.getTimeToFirePoint(config, predicted);
    Coord delta = predicted - ctx.pos;
    result.firePoint = predicted - delta.normalize() * hDist;
    result.predictedTarget = target.pos + target.velocity * flightTime;
    result.fireDistance = hDist;
    return result;
};
