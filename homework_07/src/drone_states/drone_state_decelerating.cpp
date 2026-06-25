#include <memory>
#include "basics/drone_context.hpp"
#include "basics/util.hpp"
#include "interfaces/drone_state.hpp"
#include "drone_states/drone_state_decelerating.hpp"
#include "drone_states/drone_state_stopped.hpp"

using namespace miltech04;

std::unique_ptr<IDroneState> DroneStateDecelerating::execute(DroneContext& ctx) {
    double attackSpeed = ctx.config->getConfig().attackSpeed;
    double accelPath = ctx.config->getConfig().accelPath;
    double simTimeStep = ctx.config->getConfig().simTimeStep;
    Coord dir = { cos(ctx.direction), sin(ctx.direction)};
    double targetDirection = atan2(ctx.predictedTarget.y - ctx.pos.y, ctx.predictedTarget.x - ctx.pos.x);
    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    double distance = (ctx.predictedTarget - ctx.pos).length();
    double dDir = addAngles(targetDirection, -ctx.direction);

    ctx.pos = ctx.pos + dir * ctx.speed * simTimeStep;
    ctx.speed -= acceleration * simTimeStep;
    if (ctx.speed <= 0) {
        ctx.speed = 0;
        return std::make_unique<DroneStateStopped>();
    }
    return std::make_unique<DroneStateDecelerating>();
};
