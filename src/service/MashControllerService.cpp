#include "MashControllerService.h"
#include "TemperatureReaderService.h"
#include <utils/Configuration.h>
#include <utils/Log.h>

MashControllerService * MashControllerService::instance = 0;

MashControllerService::MashControllerService() {
    
}

MashControllerService::~MashControllerService() {
    for (std::map<unsigned int, MashController *>::iterator it = _mashControllers.begin(); it != _mashControllers.end(); it++) {
        delete it->second;
    }

    _mashControllers.clear();
}

void MashControllerService::init() {
    // Initialise temperature reader service
    TemperatureReaderService::init();

    if (instance == 0) {
        instance = new MashControllerService();

        LOG("Initialising Mash Controller Service");

        const std::vector<MashControllerConfig> & mashControllers = Configuration::properties.mashControllers;
        bool isFirstUse = Configuration::properties.isFirstUse;
    
        if (isFirstUse) {
            LOG("Creating default mash controller");
            instance->createDefaultMashController();

        } else {
            LOG("Adding configured mash controllers");
            for (std::vector<MashControllerConfig>::const_iterator it = mashControllers.begin(); it != mashControllers.end(); it++) {
                instance->add(*it, false);
            }
        }
    }
}

MashControllerService * MashControllerService::_() {
    return instance;
}

void MashControllerService::add(const MashControllerConfig & mashControllerConfig, bool save) {
    LOG("Adding mash controller \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
    
    // Create a new MashController
    MashController * mashController = new MashController(mashControllerConfig);

    if (mashControllerConfig.id == 0) {
        // Get first available id
        unsigned int nextAvailableId = 1;
        while (_mashControllers.find(nextAvailableId) != _mashControllers.end()) {
            nextAvailableId++;
        }

        LOG("Creating new mash controller with Id %d", nextAvailableId);
        mashController->setId(nextAvailableId);
    }

    // Add to all mashControllers
    _mashControllers[mashController->getId()] = mashController;

    // Iterate over temperature readers
    this->addTemperatureReaders(mashController, mashControllerConfig.temperatureReaderIds);

    // Save current mashControllers
    if (save) {
        this->save();
    }
}

void MashControllerService::update(const MashControllerConfig & mashControllerConfig) {
    LOG("Updating mash controller \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
    
    if (_mashControllers.find(mashControllerConfig.id) == _mashControllers.end()) {
        WARN("Unable to update mash controller with Id %d as it does not exist", mashControllerConfig.id);
        return;
    }

    // Obtain current mash controller
    MashController * mashController = _mashControllers[mashControllerConfig.id];

    // Copy data
    mashController->setName(mashControllerConfig.name);

    // Temperature readers : clear current ones and add new ones
    mashController->clearTemperatureReaders();
    this->addTemperatureReaders(mashController, mashControllerConfig.temperatureReaderIds);

    // Save current mashControllers
    this->save();
}

void MashControllerService::addTemperatureReaders(MashController * mashController, const std::vector<unsigned int> & temperatureReaderIds) {
    for (std::vector<unsigned int>::const_iterator it = temperatureReaderIds.begin(); it != temperatureReaderIds.end(); it++) {
        unsigned int temperatureReaderId = *it;
        // Get temperature reader from termperature reader service
        TemperatureReader * temperatureReader = TemperatureReaderService::_()->get(temperatureReaderId);
        
        if (temperatureReader == NULL) {
            ERROR("Temperature reader %d cannot be added to mash controller \"%s\", id = %d because it does not exist", temperatureReaderId, mashController->getName().c_str(), mashController->getId());
        } else {
            // Add it
            ERROR("Adding temperature reader %d to mash controller \"%s\", id = %d", temperatureReaderId, mashController->getName().c_str(), mashController->getId());
            mashController->addTemperatureReader(temperatureReader);
        }
    }
}

MashController * MashControllerService::get(unsigned int id) const {
    LOG("Getting mash controller with Id %d", id);

    std::map<unsigned int, MashController *>::const_iterator it = _mashControllers.find(id);
    if (it == _mashControllers.end()) {
        WARN("Unable to get mash controller with Id %d as it does not exist", id);
        return NULL;
    }

    return it->second;
}

void MashControllerService::erase(unsigned int id) {
    LOG("Deleting mash controller with Id %d", id);
    
    std::map<unsigned int, MashController *>::iterator it = _mashControllers.find(id);
    if (it == _mashControllers.end()) {
        WARN("Unable to delete mash controller with Id %d as it does not exist", id);
        return;
    }

    // Obtain current mash controller
    MashController * mashController = it->second;
    
    // Remove from map
    _mashControllers.erase(it);

    delete mashController;

    // Save current mashControllers
    this->save();
}

void MashControllerService::createDefaultMashController() {
    MashControllerConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.name = "mash controller 0";
    defaultConfig.temperatureReaderIds.push_back(1);

    this->add(defaultConfig);
}

void MashControllerService::save() {
    LOG("Saving configuration with current mash controllers");

    std::vector<MashControllerConfig> mashControllerConfigs;
    for (std::map<unsigned int, MashController *>::const_iterator it = _mashControllers.begin(); it != _mashControllers.end(); it++) {
        mashControllerConfigs.push_back(*(it->second->getConfig()));
    }

    Configuration::properties.mashControllers = mashControllerConfigs;

    Configuration::save();
}
