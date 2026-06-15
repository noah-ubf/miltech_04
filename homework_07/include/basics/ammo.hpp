#ifndef MILTECH_INCLUDE_BASICS_AMMO_HPP
#define MILTECH_INCLUDE_BASICS_AMMO_HPP

namespace miltech04 {

struct AmmoParams {
    char name[32];
    float mass;
    float drag;
    float lift;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_AMMO_HPP