#ifndef MILTECH04_DRONE_STATE_ENUM_HPP
#define MILTECH04_DRONE_STATE_ENUM_HPP

namespace miltech04 {

enum EDroneState {
    STOPPED,
    ACCELERATING,
    DECELERATING,
    TURNING,
    MOVING
};

} // namespace miltech04

#endif // MILTECH04_DRONE_STATE_ENUM_HPP