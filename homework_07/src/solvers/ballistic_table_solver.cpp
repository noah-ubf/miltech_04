#include <cmath>
#include "basics/coord.hpp"
#include "basics/target.hpp"
#include "basics/drone_context.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "solvers/ballistic_table_solver.hpp"

using namespace miltech04;

BallisticTableSolver::BallisticTableSolver(IConfigLoader* configSource, std::string param) {
    this->configSource = configSource;
    init(param);
};

void BallisticTableSolver::init(std::string param) {
    isLoaded = table.load(param);
};

bool BallisticTableSolver::isValid() {
    return isLoaded;
};

Result BallisticTableSolver::lookup(double Z0, double V0) const {
    const double m  = configSource->getAmmoParams().mass;
    const double d  = configSource->getAmmoParams().drag;
    const double l  = configSource->getAmmoParams().lift;
    return table.lookup(Z0, V0, m, d, l);
}
Solution BallisticTableSolver::solve(const DroneContext& ctx, const Target& target) const {
    const double Z0 = configSource->getConfig().altitude;
    const double V0 = configSource->getConfig().attackSpeed;

    Result res = lookup(Z0, V0);

    Solution solution;
    Coord dir = { cos(ctx.direction), sin(ctx.direction)};
    solution.aimPoint = ctx.pos + dir * res.hDist;
    const double totalTime0 = ctx.getTimeToFirePoint(configSource, target.pos);
    Coord predicted = target.pos + target.velocity * (totalTime0 + res.t);
    solution.timeToFire = ctx.getTimeToFirePoint(configSource, predicted);
    Coord delta = predicted - ctx.pos;
    solution.firePoint = predicted - delta.normalize() * res.hDist;
    solution.predictedTarget = target.pos + target.velocity * res.t;
    solution.fireDistance = res.hDist;
    return solution;
}
