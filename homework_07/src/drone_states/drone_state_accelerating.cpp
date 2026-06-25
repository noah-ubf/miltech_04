#include <cmath>
#include <memory>
#include "basics/drone_context.hpp"
#include "basics/util.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/drone_state.hpp"
#include "drone_states/drone_state_accelerating.hpp"
#include "drone_states/drone_state_moving.hpp"

using namespace miltech04;

std::unique_ptr<IDroneState> DroneStateAccelerating::execute(DroneContext& ctx) {
    double attackSpeed = ctx.config->getConfig().attackSpeed;
    double accelPath = ctx.config->getConfig().accelPath;
    double simTimeStep = ctx.config->getConfig().simTimeStep;
    Coord dir = { cos(ctx.direction), sin(ctx.direction)};
    double targetDirection = atan2(ctx.predictedTarget.y - ctx.pos.y, ctx.predictedTarget.x - ctx.pos.x);
    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    double distance = (ctx.predictedTarget - ctx.pos).length();
    double dDir = addAngles(targetDirection, -ctx.direction);

    ctx.pos = ctx.pos + dir * ctx.speed * simTimeStep;
    ctx.speed += acceleration * simTimeStep;
    if (ctx.speed >= attackSpeed) {
        ctx.speed = attackSpeed;
        return std::make_unique<DroneStateMoving>();
    }
    return std::make_unique<DroneStateAccelerating>();
};
