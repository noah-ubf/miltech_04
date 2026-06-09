#ifndef MILTECH_INCLUDE_CLASSES_FILE_CONFIG_LOADER_HPP
#define MILTECH_INCLUDE_CLASSES_FILE_CONFIG_LOADER_HPP

#include <string>
#include "../basics/drone_config.hpp"
#include "../basics/ammo.hpp"
#include "../interfaces/config_loader.hpp"

namespace miltech04 {

class FileConfigLoader: public IConfigLoader {
    bool loaded;
    char* configFileName;
    char* ammoFileName;
    DroneConfig droneConfig;
    AmmoParams ammo;

    int init(const char* configFile, const char* ammoFile);
    
public:
    FileConfigLoader();
    FileConfigLoader(const char* configFile);
    FileConfigLoader(const char* configFile, const char* ammoFile);
    virtual bool load() override;
    virtual bool isLoaded() const override;
    virtual DroneConfig getConfig() const override;
    virtual AmmoParams getAmmoParams() const override;
    virtual ~FileConfigLoader() override;
};

} // namespace miltech04

#endif // MILTECH_INCLUDE_CLASSES_FILE_CONFIG_LOADER_HPP