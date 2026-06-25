#include <memory>
#include "basics/drone_context.hpp"
#include "basics/util.hpp"
#include "interfaces/drone_state.hpp"
#include "drone_states/drone_state_stopped.hpp"
#include "drone_states/drone_state_accelerating.hpp"
#include "drone_states/drone_state_turning.hpp"

using namespace miltech04;

std::unique_ptr<IDroneState> DroneStateStopped::execute(DroneContext& ctx) {
    double attackSpeed = ctx.config->getConfig().attackSpeed;
    double accelPath = ctx.config->getConfig().accelPath;
    double turnThreshold = ctx.config->getConfig().turnThreshold;
    Coord dir = { cos(ctx.direction), sin(ctx.direction)};
    double targetDirection = atan2(ctx.predictedTarget.y - ctx.pos.y, ctx.predictedTarget.x - ctx.pos.x);
    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    double distance = (ctx.predictedTarget - ctx.pos).length();
    double dDir = addAngles(targetDirection, -ctx.direction);

    // if direction ok, -> ACCELERATING, else -> TURNING
    if (fabs(dDir) <= turnThreshold) {
        ctx.direction = targetDirection;
        return std::make_unique<DroneStateAccelerating>();
    }
    return std::make_unique<DroneStateTurning>();
};
