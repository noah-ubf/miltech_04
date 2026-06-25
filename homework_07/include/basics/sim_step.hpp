#ifndef MILTECH_INCLUDE_BASICS_SIM_STEP_HPP
#define MILTECH_INCLUDE_BASICS_SIM_STEP_HPP

#include <string>
#include "basics/coord.hpp"
#include "basics/drone_state_enum.hpp"

namespace miltech04 {

struct SimStep {
    Coord pos;
    float direction;
    EDroneState state;
    std::string stateName;
    int   targetIdx;
    Coord dropPoint;
    Coord aimPoint;
    Coord predictedTarget;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_SIM_STEP_HPP