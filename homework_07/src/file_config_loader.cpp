#include <fstream>
#include <cstring>
#include <cmath>
#include "../include/util.hpp"
#include "../include/external/json.hpp"
#include "../include/basics/coord.hpp"
#include "../include/core/file_config_loader.hpp"
using json = nlohmann::json;
using namespace std;
using namespace miltech04;

int FileConfigLoader::init(const std::string& configFile, const std::string& ammoFile) {
    configFileName = configFile;
    ammoFileName = ammoFile;
    return 0;
};

FileConfigLoader::FileConfigLoader(const std::string& configFile, const std::string& ammoFile) {
    init(configFile, ammoFile);
};

FileConfigLoader::~FileConfigLoader() {
};

bool FileConfigLoader::isLoaded() const {
    return loaded;
}

bool FileConfigLoader::load() {
    std::ifstream fin(configFileName);
    if (!fin.is_open()) {
        LOG("Error: Unable to open config.json file");
        loaded = false;
        return false;
    }
    json j;
    fin >> j;

    droneConfig.startPos.x    = j["drone"]["position"]["x"];
    droneConfig.startPos.y    = j["drone"]["position"]["y"];
    droneConfig.altitude      = j["drone"]["altitude"];
    droneConfig.initialDir    = j["drone"]["initialDirection"];
    droneConfig.attackSpeed   = j["drone"]["attackSpeed"];
    droneConfig.accelPath     = j["drone"]["accelerationPath"];
    droneConfig.angularSpeed  = j["drone"]["angularSpeed"];
    droneConfig.turnThreshold = j["drone"]["turnThreshold"];
    droneConfig.arrayTimeStep = j["targetArrayTimeStep"];
    droneConfig.simTimeStep   = j["simulation"]["timeStep"];
    droneConfig.hitRadius     = j["simulation"]["hitRadius"];

    ammo.name = j["ammo"].get<std::string>();
    fin.close();

    std::ifstream finAmmo(ammoFileName);
    if (!finAmmo.is_open()) {
        LOG("Error: Unable to open ammo.json file");
        loaded = false;
        return false;
    }
    json am;
    finAmmo >> am;
    
    int ammoSize = am.size();
    for (int i = 0; i < ammoSize; ++i) {
        if (ammo.name == am[i]["name"].get<std::string>()) {
            ammo.mass = am[i]["mass"];
            ammo.drag = am[i]["drag"];
            ammo.lift = am[i]["lift"];
            break;
        }
    }
    finAmmo.close();
    loaded = true;
    return true;
};

DroneConfig FileConfigLoader::getConfig() const {
    return droneConfig;
}

AmmoParams FileConfigLoader::getAmmoParams() const {
    return ammo;
}