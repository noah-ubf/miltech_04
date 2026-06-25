#ifndef MILTECH04_DRONESTATE_TURNING_HPP
#define MILTECH04_DRONESTATE_TURNING_HPP

#include <memory>
#include "interfaces/drone_state.hpp"

namespace miltech04 {

struct DroneContext;

class DroneStateTurning : public IDroneState {
public:
    virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
    virtual EDroneState getState() const override {
        return EDroneState::TURNING;
    };
    const char* getStateName() const override {
        return "Turning";
    };
};

} // namespace miltech04

#endif // MILTECH04_DRONESTATE_TURNING_HPP
