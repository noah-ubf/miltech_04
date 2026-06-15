#ifndef MILTECH_INCLUDE_BASICS_DRONE_CONFIG_HPP
#define MILTECH_INCLUDE_BASICS_DRONE_CONFIG_HPP

#include "coord.hpp"

namespace miltech04 {

struct DroneConfig {
    Coord startPos;
    double altitude;
    double initialDir;
    double attackSpeed;
    double accelPath;
    float arrayTimeStep;
    float simTimeStep;
    float hitRadius;
    float angularSpeed;
    float turnThreshold;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_DRONE_CONFIG_HPP