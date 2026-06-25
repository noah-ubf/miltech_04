#ifndef MILTECH_INCLUDE_CLASSES_FILE_CONFIG_LOADER_HPP
#define MILTECH_INCLUDE_CLASSES_FILE_CONFIG_LOADER_HPP

#include <string>
#include "basics/drone_config.hpp"
#include "basics/ammo.hpp"
#include "interfaces/config_loader.hpp"

namespace miltech04 {

class FileConfigLoader: public IConfigLoader {
    bool loaded;
    std::string configFileName;
    std::string ammoFileName;
    DroneConfig droneConfig;
    AmmoParams ammo;

    int init(const std::string& configFile, const std::string& ammoFile);
    
public:
    FileConfigLoader();
    FileConfigLoader(const std::string& configFile);
    FileConfigLoader(const std::string& configFile, const std::string& ammoFile);
    virtual bool load() override;
    virtual bool isLoaded() const override;
    virtual DroneConfig getConfig() const override;
    virtual AmmoParams getAmmoParams() const override;
    virtual ~FileConfigLoader() override = default;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_FILE_CONFIG_LOADER_HPP