#ifndef MILTECH_INCLUDE_CLASSES_ANALYTICAL_SOLVER_HPP
#define MILTECH_INCLUDE_CLASSES_ANALYTICAL_SOLVER_HPP

#include <string>
#include "interfaces/ballistic_solver.hpp"
#include "interfaces/config_loader.hpp"
#include "basics/drone.hpp"
#include "basics/target.hpp"

struct Coord;
struct Target;

namespace miltech04 {

class AnalyticalSolver : public IBallisticSolver {
    IConfigLoader* config;
    double flightTime;
    double hDist;
    void init();
    double calcFlightTime();
    double calcFireDistance();
    Coord calcPredictedTarget(const Coord& targetPos, const Coord& targetV) const ;

public:
    AnalyticalSolver(IConfigLoader* configSource, std::string param);
    virtual bool isValid() override;
    virtual Solution solve(const Drone& drone, const Target& target) const override;
    virtual ~AnalyticalSolver() override = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_ANALYTICAL_SOLVER_HPP