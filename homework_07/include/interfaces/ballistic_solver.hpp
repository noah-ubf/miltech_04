#ifndef MILTECH_INCLUDE_INTERFACES_BALLISTIC_SOLVER_HPP
#define MILTECH_INCLUDE_INTERFACES_BALLISTIC_SOLVER_HPP

#include "basics/simulation.hpp"
#include "basics/target.hpp"

namespace miltech04 {

struct Solution {
  Coord predictedTarget;
  Coord firePoint;
  double timeToFire;
};

class IBallisticSolver {
public:
    virtual ~IBallisticSolver() = default;
    virtual bool isValid() = 0;
    virtual double getFireDistance() = 0;
    virtual Solution solve(const SimStep& drone, const Target& target) const = 0;

};

} // namespace miltech04

#endif // MILTECH_INCLUDE_INTERFACES_BALLISTIC_SOLVER_HPP