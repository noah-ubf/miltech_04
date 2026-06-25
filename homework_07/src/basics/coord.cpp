#include "basics/const.hpp"
#include "basics/coord.hpp"

using namespace miltech04;

Coord Coord::operator+(const Coord& other) const {
    Coord result;
    result.x = x + other.x;
    result.y = y + other.y;
    return result;
}

Coord Coord::operator-(const Coord& other) const {
    Coord result;
    result.x = x - other.x;
    result.y = y - other.y;
    return result;
}

Coord Coord::operator*(double s) const {
    Coord result;
    result.x = x * s;
    result.y = y * s;
    return result;
}

Coord Coord::operator/(double s) const {
    Coord result;
    result.x = x / s;
    result.y = y / s;
    return result;
}

bool Coord::operator==(Coord c) const {
    return fabs(c.x - x) < MIN && fabs(c.y - y) < MIN;
}

double Coord::length() const {
    return hypot(x, y);
}

Coord Coord::normalize() const {
    Coord result;
    double len = length();
    result.x = x / len;
    result.y = y / len;
    return result;
}
