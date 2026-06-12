#ifndef MILTECH_INCLUDE_CLASSES_ANALYTICAL_SOLVER_HPP
#define MILTECH_INCLUDE_CLASSES_ANALYTICAL_SOLVER_HPP

#include <memory>
#include "interfaces/ballistic_solver.hpp"
#include "interfaces/config_loader.hpp"
#include "basics/simulation.hpp"

struct Coord;
struct Target;

namespace miltech04 {

class AnalyticalSolver : public IBallisticSolver {
    IConfigLoader* configSource;
    double flightTime;
    double hDist;
    void init();
    double calcFlightTime();
    double calcFireDistance();
    Coord calcPredictedTarget(const Coord& targetPos, const Coord& targetV) const ;
    double getTimeToFirePointFromStopped(const SimStep& drone, const Coord& target) const;
    double getTimeToFirePoint(const SimStep& drone, const Coord target) const;

public:
    AnalyticalSolver(IConfigLoader* configSource);
    virtual bool isValid() override;
    virtual double getFireDistance() override;
    virtual Solution solve(const SimStep& drone, const Target& target) const override;
    virtual ~AnalyticalSolver() override = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_ANALYTICAL_SOLVER_HPP