#include <cmath>
#include "basics/util.hpp"

namespace miltech04 {

double addAngles(double angle, double increment) {
    // Ensure that the angle lays between M_PI and -M_PI
    const double M_2PI = M_PI + M_PI;
    angle += increment;
    while(angle >= M_PI) angle -= M_2PI;
    while(angle < -M_PI) angle += M_2PI;
    return angle;
}

} // namespace miltech04