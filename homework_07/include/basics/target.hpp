#ifndef MILTECH_INCLUDE_BASICS_TARGET_HPP
#define MILTECH_INCLUDE_BASICS_TARGET_HPP

#include "coord.hpp"

struct Target {
    int idx;
    Coord pos;
    Coord velocity;
};

#endif // MILTECH_INCLUDE_BASICS_TARGET_HPP