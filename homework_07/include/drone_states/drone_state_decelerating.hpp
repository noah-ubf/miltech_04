#ifndef MILTECH04_DRONESTATE_DECELERATING_HPP
#define MILTECH04_DRONESTATE_DECELERATING_HPP

#include <memory>
#include "basics/drone_context.hpp"
#include "interfaces/drone_state.hpp"

namespace miltech04 {

class DroneStateDecelerating : public IDroneState {
public:
    virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
    virtual EDroneState getState() const override {
        return EDroneState::DECELERATING;
    };
    const char* getStateName() const override {
        return "Decelerating";
    };
};

} // namespace miltech04

#endif // MILTECH04_DRONESTATE_DECELERATING_HPP
