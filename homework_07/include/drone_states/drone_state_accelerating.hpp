#ifndef MILTECH04_DRONESTATE_ACCELERATING_HPP
#define MILTECH04_DRONESTATE_ACCELERATING_HPP

#include <memory>
#include "interfaces/drone_state.hpp"

namespace miltech04 {
struct DroneContext;

class DroneStateAccelerating : public IDroneState {
public:
    virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
    virtual EDroneState getState() const override {
        return EDroneState::ACCELERATING;
    };
    virtual const char* getStateName() const override {
        return "Accelerating";
    };
};

} // namespace miltech04

#endif // MILTECH04_DRONESTATE_ACCELERATING_HPP
