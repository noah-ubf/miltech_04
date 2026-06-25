#ifndef MILTECH04_DRONESTATE_STOPPED_HPP
#define MILTECH04_DRONESTATE_STOPPED_HPP

#include <memory>
#include "interfaces/drone_state.hpp"

namespace miltech04 {

struct DroneContext;

class DroneStateStopped : public IDroneState {
public:
    virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) override;
    virtual EDroneState getState() const override {
        return EDroneState::STOPPED;
    };
    const char* getStateName() const override {
        return "Stopped";
    };
};

} // namespace miltech04

#endif // MILTECH04_DRONESTATE_STOPPED_HPP
