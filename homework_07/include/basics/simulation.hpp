#ifndef MILTECH_INCLUDE_BASICS_SIMULATION_HPP
#define MILTECH_INCLUDE_BASICS_SIMULATION_HPP

#include "coord.hpp"

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

struct SimSteps {
    int stepCount;
    SimStep* steps;
};

#endif // MILTECH_INCLUDE_BASICS_SIMULATION_HPP