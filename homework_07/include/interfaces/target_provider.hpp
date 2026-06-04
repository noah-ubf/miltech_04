#ifndef MILTECH_INCLUDE_INTERFACES_TARGET_PROVIDER_HPP
#define MILTECH_INCLUDE_INTERFACES_TARGET_PROVIDER_HPP

#include "../basics/target.hpp"

class ITargetProvider {
public:
  virtual bool isLoaded() const = 0;
  virtual void setArrayTimeStep(double timeStep) = 0;
  virtual int getTargetCount() const = 0;
  virtual void setSimTime(double time) = 0;
  virtual bool hasNextTarget() const = 0;
  virtual Target getNextTarget() = 0;
  virtual ~ITargetProvider() = default;
};

#endif // MILTECH_INCLUDE_INTERFACES_TARGET_PROVIDER_HPP
