#ifndef MILTECH_INCLUDE_INTERFACES_BALLISTIC_SOLVER_HPP
#define MILTECH_INCLUDE_INTERFACES_BALLISTIC_SOLVER_HPP

#include "basics/drone_context.hpp"
#include "basics/target.hpp"

namespace miltech04 {

struct Solution {
    Coord aimPoint;
    Coord predictedTarget;
    Coord firePoint;
    double fireDistance;
    double timeToFire;
};

class IBallisticSolver {
public:
    virtual ~IBallisticSolver() = default;
    virtual bool isValid() = 0;
    virtual Solution solve(const DroneContext& drone, const Target& target) const = 0;

};

} // namespace miltech04

#endif // MILTECH_INCLUDE_INTERFACES_BALLISTIC_SOLVER_HPP