#include <cmath>
#include "basics/coord.hpp"
#include "basics/target.hpp"
#include "basics/drone_context.hpp"
#include "interfaces/ballistic_solver.hpp"
#include "solvers/ballistic_table_solver.hpp"

using namespace miltech04;

// Лінійна інтерполяція для Result (обидва поля паралельно)
Result lerp(const Result& a, const Result& b, double t) {
    return {
        a.t     + (b.t     - a.t)     * t,
        a.hDist + (b.hDist - a.hDist) * t
    };
}
 
// Індекс і коефіцієнт для одного виміру
struct Interp {
    int lo;      // нижній індекс в осі
    double frac;  // коефіцієнт [0..1]
};
 
Interp findInterp(double val, const std::vector<double>& axis) {
    if (val <= axis.front()) return {0, 0.0};
    if (val >= axis.back())
        return {(int)axis.size()-2, 1.0};
 
    auto it = std::lower_bound(
        axis.begin(), axis.end(), val);
    int i = (int)(it - axis.begin()) - 1;
    if (i < 0) i = 0;
 
    double frac = (val - axis[i])
               / (axis[i+1] - axis[i]);
    return {i, frac};
}

Result BallisticTable::lookup(double Z0, double V0, double m, double d,  double l) const {
    Interp iz = findInterp(Z0, axisZ0);
    Interp iv = findInterp(V0, axisV0);
    Interp im = findInterp(m,  axisM);
    Interp id = findInterp(d,  axisD);
    Interp il = findInterp(l,  axisL);
 
    // 2^5 = 32 вершини гіперкуба
    // Згортаємо: 32 → 16 → 8 → 4 → 2 → 1
 
    // l: 32 → 16
    Result v[16];
    for (int a = 0; a < 2; a++)
     for (int b = 0; b < 2; b++)
      for (int c = 0; c < 2; c++)
       for (int e = 0; e < 2; e++) {
           auto& lo = at(iz.lo+a, iv.lo+b,
                         im.lo+c, id.lo+e, il.lo);
           auto& hi = at(iz.lo+a, iv.lo+b,
                         im.lo+c, id.lo+e, il.lo+1);
           v[a*8+b*4+c*2+e] = lerp(lo, hi, il.frac);
       }
 
    // d: 16 → 8
    Result w[8];
    for (int a = 0; a < 2; a++)
     for (int b = 0; b < 2; b++)
      for (int c = 0; c < 2; c++)
       w[a*4+b*2+c] = lerp(v[a*8+b*4+c*2],
                            v[a*8+b*4+c*2+1],
                            id.frac);
 
    // m: 8 → 4
    Result u[4];
    for (int a = 0; a < 2; a++)
     for (int b = 0; b < 2; b++)
      u[a*2+b] = lerp(w[a*4+b*2],
                       w[a*4+b*2+1], im.frac);
 
    // V0: 4 → 2
    Result s[2];
    for (int a = 0; a < 2; a++)
        s[a] = lerp(u[a*2], u[a*2+1], iv.frac);
 
    // Z0: 2 → 1
    return lerp(s[0], s[1], iz.frac);
}

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
