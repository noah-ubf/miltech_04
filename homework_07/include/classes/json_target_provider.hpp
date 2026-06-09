#ifndef MILTECH_INCLUDE_CLASSES_JSON_TARGET_PROVIDER_HPP
#define MILTECH_INCLUDE_CLASSES_JSON_TARGET_PROVIDER_HPP

#include <string>
#include <vector>
#include "../interfaces/target_provider.hpp"
#include "../basics/target.hpp"

namespace miltech04 {

class JsonTargetProvider : public ITargetProvider {
    bool loaded;
    std::vector<std::vector<Coord>> targets;
    int targetCount;
    int stepCount;
    double timeStep;
    int currentTargetIdx;

    double arrayTimeStep = 1.0;
    double simTime = 0;
    int currentIdx = 0;
    int nextIdx = 1;
    double frac = 0;

    void load(const std::string& fileName);

public:
    JsonTargetProvider(const std::string& fileName);
    virtual bool isLoaded() const override;
    virtual void setArrayTimeStep(double timeStep) override;
    virtual int getTargetCount() const override;
    virtual void setSimTime(double time) override;
    virtual bool hasNextTarget() const override;
    virtual Target getNextTarget() override;
    virtual ~JsonTargetProvider() override = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_JSON_TARGET_PROVIDER_HPP