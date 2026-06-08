#ifndef MILTECH_INCLUDE_BASICS_SIMULATION_HPP
#define MILTECH_INCLUDE_BASICS_SIMULATION_HPP

#include <string>
#include "./basics/coord.hpp"
#include "../include/util.hpp"

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
    int stepCount;
    SimStep* steps;
public:
    SimulationResults(int maxCount);

    bool push(const SimStep& step) {
        if (stepCount >= maxStepsCount) {
            LOG("Error: Simulation results exceeded maximum count");
            return false;
        }
        steps[stepCount++] = step;
        return true;
    }

    bool save(const std::string& filename);

    ~SimulationResults();
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_SIMULATION_HPP