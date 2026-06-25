#ifndef MILTECH04_DRONESTATE_MOVING_HPP
#define MILTECH04_DRONESTATE_MOVING_HPP

#include <memory>
#include "interfaces/drone_state.hpp"

namespace miltech04 {

struct DroneContext;

class DroneStateMoving : public IDroneState {
public:
    virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
    virtual EDroneState getState() const override {
        return EDroneState::MOVING;
    };
    const char* getStateName() const override {
        return "Moving";
    };
};

} // namespace miltech04

#endif // MILTECH04_DRONESTATE_MOVING_HPP
