#include <fstream>
#include <string>
#include "../include/providers/json_target_provider.hpp"
#include "../include/util.hpp"
#include "../include/external/json.hpp"
using json = nlohmann::json;

using namespace miltech04;

JsonTargetProvider::JsonTargetProvider(const std::string& fileName) {
    load(fileName);
};

void JsonTargetProvider::load(const std::string& fileName) {
  std::ifstream ft(fileName);
    if (!ft.is_open()) {
        LOG("Error: Unable to open targets file");
        loaded = false;
        return;
    }
    json jt;
    ft >> jt;
    targetCount = jt["targetCount"];
    stepCount = jt["timeSteps"];
 
    targets.resize(targetCount);
    for (int i = 0; i < targetCount; i++) {
        targets[i].resize(stepCount);
        for (int j = 0; j < stepCount; j++) {
            targets[i][j].x = jt["targets"][i]["positions"][j]["x"];
            targets[i][j].y = jt["targets"][i]["positions"][j]["y"];
        }
    }
    ft.close();
    loaded = true;
};

bool JsonTargetProvider::isLoaded() const {
    return loaded;
};

void JsonTargetProvider::setArrayTimeStep(double timeStep) {
    arrayTimeStep = timeStep;
};

int JsonTargetProvider::getTargetCount() const {
    return targetCount;
};

void JsonTargetProvider::setSimTime(double time) {
    currentTargetIdx = 0;
    simTime = time;
    currentIdx = (int)floor(simTime / arrayTimeStep) % stepCount;
    nextIdx = (currentIdx + 1) % stepCount;
    frac = (simTime - currentIdx * arrayTimeStep) / arrayTimeStep;
};

bool JsonTargetProvider::hasNextTarget() const {
    return currentTargetIdx < targetCount;
}

Target JsonTargetProvider::getNextTarget() {
    Target result;
    Coord lastCoord = targets[currentTargetIdx][currentIdx];
    Coord nextCoord = targets[currentTargetIdx][nextIdx];
    result.pos = lastCoord + (nextCoord - lastCoord) * frac;
    result.velocity = (nextCoord - lastCoord) / arrayTimeStep;
    result.idx = currentTargetIdx;
    ++currentTargetIdx;
    return result;
};