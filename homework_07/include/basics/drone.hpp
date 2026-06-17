#ifndef MILTECH_INCLUDE_BASICS_DRONE_HPP
#define MILTECH_INCLUDE_BASICS_DRONE_HPP

#include "basics/coord.hpp"
#include "interfaces/config_loader.hpp"

namespace miltech04 {

enum DroneState {
    STOPPED,
    ACCELERATING,
    DECELERATING,
    TURNING,
    MOVING
};

struct Drone {
    Coord pos;
    double speed;
    float direction;
    double fireDistance;
    DroneState state;
    int   targetIdx;
    Coord dropPoint;
    Coord aimPoint;
    Coord predictedTarget;

    double getTimeToFirePointFromStopped(IConfigLoader* config, const Coord& target) const;
    double getTimeToFirePoint(IConfigLoader* config, const Coord& target) const;
    Drone move(IConfigLoader* config, const Coord& target) const;

};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_DRONE_HPP