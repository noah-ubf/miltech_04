#include <memory>
#include "basics/drone_context.hpp"
#include "basics/util.hpp"
#include "interfaces/drone_state.hpp"
#include "drone_states/drone_state_moving.hpp"
#include "drone_states/drone_state_decelerating.hpp"

using namespace miltech04;

std::unique_ptr<IDroneState> DroneStateMoving::execute(DroneContext& ctx) {
    double attackSpeed = ctx.config->getConfig().attackSpeed;
    double accelPath = ctx.config->getConfig().accelPath;
    double simTimeStep = ctx.config->getConfig().simTimeStep;
    double turnThreshold = ctx.config->getConfig().turnThreshold;
    Coord dir = { cos(ctx.direction), sin(ctx.direction)};
    double targetDirection = atan2(ctx.predictedTarget.y - ctx.pos.y, ctx.predictedTarget.x - ctx.pos.x);
    double acceleration = attackSpeed * attackSpeed / (2 * accelPath);
    double distance = (ctx.predictedTarget - ctx.pos).length();
    double dDir = addAngles(targetDirection, -ctx.direction);

    ctx.pos = ctx.pos + dir * ctx.speed * simTimeStep;
    if (fabs(dDir) > turnThreshold) {
        return std::make_unique<DroneStateDecelerating>();
    }
    return std::make_unique<DroneStateMoving>();
};
