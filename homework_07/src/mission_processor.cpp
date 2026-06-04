#include "../include/core/mission_processor.hpp"
#include "../include/interfaces/config_loader.hpp"
#include "../include/interfaces/ballistic_solver.hpp"


void MissionProcessor::init(IConfigLoader* configSource) {
    config = configSource;
};

void MissionProcessor::setTargetProvider(ITargetProvider* provider) {
    targetProvider = provider;
}

void MissionProcessor::changeSolver(IBallisticSolver* solverName) {
    solver = solverName;
};

void MissionProcessor::setSimTime(const double simTime) {
    targetProvider->setSimTime(simTime);
};

bool MissionProcessor::hasNext() {
  return currentIdx < targetProvider->getTargetCount();
};

void MissionProcessor::step() {
    ++currentIdx;
};

void MissionProcessor::reset() {
    currentIdx = 0;
};

