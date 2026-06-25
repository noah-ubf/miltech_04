#ifndef MILTECH_INCLUDE_BASICS_SIMULATION_HPP
#define MILTECH_INCLUDE_BASICS_SIMULATION_HPP

#include <string>
#include <vector>
#include "basics/coord.hpp"
#include "util.hpp"

namespace miltech04 {

enum DroneState {
    STOPPED,
    ACCELERATING,
    DECELERATING,
    TURNING,
    MOVING
};

struct SimStep {
    Coord pos;
    double speed;
    float direction;
    DroneState state;
    int   targetIdx;
    Coord dropPoint;
    Coord aimPoint;
    Coord predictedTarget;
};

class SimulationResults {
    int maxStepsCount;
    std::vector<SimStep> steps;
public:
    SimulationResults(int maxCount);
    bool push(const SimStep& step);
    bool save(const std::string& filename);
    ~SimulationResults() = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_SIMULATION_HPP