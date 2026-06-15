#ifndef MILTECH_INCLUDE_BASICS_TARGET_HPP
#define MILTECH_INCLUDE_BASICS_TARGET_HPP

#include "basics/coord.hpp"

namespace miltech04 {

struct Target {
    int idx;
    Coord pos;
    Coord velocity;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_TARGET_HPP