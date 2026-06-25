#ifndef MILTECH_INCLUDE_INTERFACES_CONFIG_LOADER_HPP
#define MILTECH_INCLUDE_INTERFACES_CONFIG_LOADER_HPP

#include "basics/drone_config.hpp"
#include "basics/ammo.hpp"

namespace miltech04 {

class IConfigLoader {
public:
    virtual bool load() = 0;
    virtual bool isLoaded() const = 0;
    virtual DroneConfig getConfig() const = 0;
    virtual AmmoParams getAmmoParams() const = 0;
    virtual ~IConfigLoader() = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_INTERFACES_CONFIG_LOADER_HPP