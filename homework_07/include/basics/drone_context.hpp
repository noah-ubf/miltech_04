#ifndef MILTECH_INCLUDE_BASICS_DRONE_HPP
#define MILTECH_INCLUDE_BASICS_DRONE_HPP

#include <string>
#include "basics/coord.hpp"
#include "basics/drone_state_enum.hpp"

namespace miltech04 {

class IConfigLoader;

struct DroneContext {
    bool isFinished = false;
    IConfigLoader* config;
    Coord pos;
    double speed;
    float direction;
    double fireDistance;
    EDroneState state;
    std::string stateName;
    int   targetIdx;
    Coord dropPoint;
    Coord aimPoint;
    Coord predictedTarget;

    double getTimeToFirePointFromStopped(IConfigLoader* config, const Coord& target) const;
    double getTimeToFirePoint(IConfigLoader* config, const Coord& target) const;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_DRONE_HPP