#include <fstream>
#include "../include/basics/simulation.hpp"
#include "../include/util.hpp"
#include "../include/external/json.hpp"

using json = nlohmann::json;
using namespace miltech04;


SimulationResults::SimulationResults(int maxCount) {
    maxStepsCount = maxCount;
    stepCount = 0;
    steps = new SimStep[maxCount];
}

SimulationResults::~SimulationResults() {
    delete[] steps;
}

bool SimulationResults::save(const std::string& filename) {
    json out;
    out["totalSteps"] = stepCount;
    out["steps"] = json::array();
    for (int i = 0; i < stepCount; i++) {
        SimStep s = steps[i];
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