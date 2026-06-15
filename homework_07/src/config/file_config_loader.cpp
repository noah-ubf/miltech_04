#include <fstream>
#include <cstring>
#include <cmath>
#include "basics/util.hpp"
#include "external/json.hpp"
#include "basics/coord.hpp"
#include "config/file_config_loader.hpp"
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

    // The previous implementation was better because it never needed additional data structures.
    // Adding a map nevertheless just for practice.
    std::map<std::string, AmmoParams> ammoMap = {};
    for (int i = 0; i < ammoSize; ++i) {
        AmmoParams params;
        params.mass = am[i]["mass"];
        params.drag = am[i]["drag"];
        params.lift = am[i]["lift"];
        ammoMap[am[i]["name"].get<std::string>()] = params;
    }
    finAmmo.close();

    if (ammoMap.find(ammo.name) != ammoMap.end()) {
        ammo = ammoMap[ammo.name];
    } else {
        LOG("Error: Ammo type not found in ammo.json file");
        loaded = false;
        return false;
    }
    loaded = true;
    return true;
};

DroneConfig FileConfigLoader::getConfig() const {
    return droneConfig;
}

AmmoParams FileConfigLoader::getAmmoParams() const {
    return ammo;
}