#ifndef MILTECH04_IDRONESTATE_HPP
#define MILTECH04_IDRONESTATE_HPP

#include <memory>
#include "basics/drone_context.hpp"
#include "basics/drone_state_enum.hpp"

namespace miltech04 {

class IDroneState {
public:
    virtual ~IDroneState() = default;
    virtual std::unique_ptr<IDroneState> execute(DroneContext& ctx) = 0;
    virtual EDroneState getState() const = 0;
    virtual const char* getStateName() const = 0;
};

} // namespace miltech04

#endif // MILTECH04_IDRONESTATE_HPP