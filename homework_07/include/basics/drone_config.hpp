#ifndef MILTECH_INCLUDE_BASICS_DRONE_CONFIG_HPP
#define MILTECH_INCLUDE_BASICS_DRONE_CONFIG_HPP

#include "coord.hpp"

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

#endif // MILTECH_INCLUDE_BASICS_DRONE_CONFIG_HPP