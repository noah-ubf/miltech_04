#include <cmath>
#include <memory>
#include "basics/drone_context.hpp"
#include "basics/util.hpp"
#include "interfaces/config_loader.hpp"
#include "interfaces/drone_state.hpp"
#include "drone_states/drone_state_turning.hpp"
#include "drone_states/drone_state_stopped.hpp"

using namespace miltech04;

std::unique_ptr<IDroneState> DroneStateTurning::execute(DroneContext& ctx) {
    double angularSpeed = ctx.config->getConfig().angularSpeed;
    double simTimeStep = ctx.config->getConfig().simTimeStep;
    double turnThreshold = ctx.config->getConfig().turnThreshold;
    double targetDirection = atan2(ctx.predictedTarget.y - ctx.pos.y, ctx.predictedTarget.x - ctx.pos.x);
    double dDir = addAngles(targetDirection, -ctx.direction);

    if (dDir >= 0) {
        ctx.direction = addAngles(ctx.direction, angularSpeed * simTimeStep);
    } else {
        ctx.direction = addAngles(ctx.direction, -angularSpeed * simTimeStep);
    }
    dDir = addAngles(targetDirection, -ctx.direction);
    if (fabs(dDir) < turnThreshold) {
        ctx.direction = targetDirection;
        return std::make_unique<DroneStateStopped>();
    }
    return std::make_unique<DroneStateTurning>();
};
