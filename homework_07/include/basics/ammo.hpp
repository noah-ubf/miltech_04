#ifndef MILTECH_INCLUDE_BASICS_AMMO_HPP
#define MILTECH_INCLUDE_BASICS_AMMO_HPP

#include <string>

namespace miltech04 {

struct AmmoParams {
    std::string name;
    float mass;
    float drag;
    float lift;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_AMMO_HPP