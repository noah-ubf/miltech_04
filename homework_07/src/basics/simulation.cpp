#include <fstream>
#include "basics/drone.hpp"
#include "basics/coord.hpp"
#include "basics/sim_step.hpp"
#include "basics/simulation.hpp"
#include "basics/util.hpp"
#include "external/json.hpp"

using json = nlohmann::json;
using namespace miltech04;


SimulationResults::SimulationResults(int maxCount) {
    maxStepsCount = maxCount;
}

bool SimulationResults::push(const Drone& step) {
    if (steps.size() >= maxStepsCount) {
        LOG("Error: Maximum steps count reached");
        return false;
    }
    SimStep item = {};
    item.pos = step.pos;
    item.direction = step.direction;
    item.state = step.state;
    item.targetIdx = step.targetIdx;
    item.dropPoint = step.dropPoint;
    item.aimPoint = step.aimPoint;
    item.predictedTarget = step.predictedTarget;

    steps.push_back(item);
    return true;
}

bool SimulationResults::save(const std::string& filename) {
    json out;
    out["totalSteps"] = steps.size();
    out["steps"] = json::array();
    for (const auto& s : steps) {
        json step;
        step["position"]        = {{"x", s.pos.x}, {"y", s.pos.y}};
        step["direction"]       = s.direction;
        step["state"]           = s.state;
        #ifdef ENABLE_DEBUG
            step["stateName"]       = STATE_NAMES[s.state];
        #endif
        step["targetIndex"]     = s.targetIdx;
        step["dropPoint"]       = {{"x", s.dropPoint.x}, {"y", s.dropPoint.y}};
        step["aimPoint"]        = {{"x", s.aimPoint.x}, {"y", s.aimPoint.y}};
        step["predictedTarget"] = {{"x", s.predictedTarget.x}, {"y", s.predictedTarget.y}};
        out["steps"].push_back(step);
    }
    std::ofstream fout(filename);
    if (!fout.is_open()) {
        LOG("Error: Unable to open simulation.json file");
        return false;
    }
    fout << out.dump(2);
    return true;
};
