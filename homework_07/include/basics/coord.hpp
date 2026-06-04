#ifndef MILTECH_INCLUDE_BASICS_COORD_HPP
#define MILTECH_INCLUDE_BASICS_COORD_HPP

#include <cmath>
#include "../const.hpp"

struct Coord {
    double x;
    double y;

    Coord operator+(const Coord& other) const {
        Coord result;
        result.x = x + other.x;
        result.y = y + other.y;
        return result;
    }

    Coord operator-(const Coord& other) const {
        Coord result;
        result.x = x - other.x;
        result.y = y - other.y;
        return result;
    }

    Coord operator*(double s) const {
        Coord result;
        result.x = x * s;
        result.y = y * s;
        return result;
    }

    Coord operator/(double s) const {
        Coord result;
        result.x = x / s;
        result.y = y / s;
        return result;
    }

    bool operator==(Coord c) const {
        return fabs(c.x - x) < MIN && fabs(c.y - y) < MIN;
    }

    double length() const {
        return hypot(x, y);
    }

    Coord normalize() const {
        Coord result;
        double len = length();
        result.x = x / len;
        result.y = y / len;
        return result;
    }
};

#endif // MILTECH_INCLUDE_BASICS_COORD_HPP