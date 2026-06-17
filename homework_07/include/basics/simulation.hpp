#ifndef MILTECH_INCLUDE_BASICS_SIMULATION_HPP
#define MILTECH_INCLUDE_BASICS_SIMULATION_HPP

#include <string>
#include <vector>
#include "basics/sim_step.hpp"

namespace miltech04 {

class SimulationResults {
    int maxStepsCount;
    std::vector<SimStep> steps;
public:
    SimulationResults(int maxCount);
    bool push(const Drone& step);
    bool save(const std::string& filename);
    ~SimulationResults() = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_BASICS_SIMULATION_HPP