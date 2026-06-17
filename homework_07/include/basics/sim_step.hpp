#ifndef MILTECH_INCLUDE_BASICS_SIM_STEP_HPP
#define MILTECH_INCLUDE_BASICS_SIM_STEP_HPP

#include "basics/coord.hpp"
#include "basics/drone.hpp"

namespace miltech04 {

struct SimStep {
    Coord pos;
    float direction;
    DroneState state;
    int   targetIdx;
    Coord dropPoint;
    Coord aimPoint;
    Coord predictedTarget;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_SIM_STEP_HPP