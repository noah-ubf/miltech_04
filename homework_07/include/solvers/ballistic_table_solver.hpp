#ifndef MILTECH_INCLUDE_CLASSES_BALLISTIC_TABLE_SOLVER_HPP
#define MILTECH_INCLUDE_CLASSES_BALLISTIC_TABLE_SOLVER_HPP

#include "interfaces/ballistic_solver.hpp"
#include "basics/coord.hpp"
#include "config/ballistic_table.hpp"

namespace miltech04 {

struct DroneContext;
struct Target;
class IConfigLoader;

class BallisticTableSolver : public IBallisticSolver {
    bool isLoaded;
    IConfigLoader* configSource;
    BallisticTable table;
    void init(std::string param);
    Coord calcPredictedTarget(const Coord& targetPos, const Coord& targetV) const ;
    Result lookup(double Z0, double V0) const;
public:
    BallisticTableSolver(IConfigLoader* configSource, std::string param = "");
    virtual bool isValid() override;
    virtual Solution solve(const DroneContext& drone, const Target& target) const override;
    virtual ~BallisticTableSolver() override = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_BALLISTIC_TABLE_SOLVER_HPP