#include "MashControllerService.h"
#include "ThermometerService.h"
#include <utils/Configuration.h>
#include <utils/Log.h>

MashControllerService * MashControllerService::instance = 0;

MashControllerService::MashControllerService() {
    
}

MashControllerService::~MashControllerService() {
    for (std::vector<MashController *>::iterator it = _mashControllers.begin(); it != _mashControllers.end(); it++) {
        delete *it;
    }

    _mashControllers.clear();
}

void MashControllerService::init() {
    // Initialise thermometer service
    ThermometerService::init();

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

MashController * MashControllerService::add(const MashControllerConfig & mashControllerConfig, bool save) {
    LOG("Adding mash controller \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
    
    // Create a new MashController
    MashController * mashController = new MashController(mashControllerConfig);

    if (mashControllerConfig.id == 0) {
        // Get first available id
        unsigned int maxId = 0;
        for (std::vector<MashController *>::iterator it = _mashControllers.begin(); it != _mashControllers.end(); it++) {
            maxId = maxId < (*it)->getId() ? (*it)->getId() : maxId;
        }

        unsigned int nextAvailableId = maxId + 1;

        LOG("Creating new mash controller with Id %d", nextAvailableId);
        mashController->setId(nextAvailableId);
    
    } else {
        // Check if it exist
        std::vector<MashController *>::iterator it = _mashControllers.begin();
        while (it != _mashControllers.end() && ((*it)->getId() != mashControllerConfig.id)) {
            it++;
        }

        // replace if needed
        if (it != _mashControllers.end()) {
            LOG("Mash controller with Id %d already exists: replacing it", mashControllerConfig.id);
            _mashControllers.erase(it);
        }

    }

    // Add to all mashControllers
    _mashControllers.push_back(mashController);

    // Iterate over thermometers
    this->addThermometers(mashController, mashControllerConfig.thermometerIds);

    // Save current mashControllers
    if (save) {
        this->save();
    }

    return mashController;
}

MashController * MashControllerService::update(const MashControllerConfig & mashControllerConfig) {
    LOG("Updating mash controller \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
    
    // Check if it exist
    std::vector<MashController *>::iterator it = _mashControllers.begin();
    while (it != _mashControllers.end() && ((*it)->getId() != mashControllerConfig.id)) {
        it++;
    }

    if (it == _mashControllers.end()) {
        WARN("Unable to update mash controller with Id %d as it does not exist", mashControllerConfig.id);
        return NULL;
    }

    // Obtain current mash controller
    MashController * mashController = *it;

    // Copy data
    mashController->setName(mashControllerConfig.name);

    // Temperature readers : clear current ones and add new ones
    mashController->clearThermometers();
    this->addThermometers(mashController, mashControllerConfig.thermometerIds);

    // Save current mashControllers
    this->save();

    return mashController;
}

void MashControllerService::addThermometers(MashController * mashController, const std::vector<unsigned int> & thermometerIds) {
    for (std::vector<unsigned int>::const_iterator it = thermometerIds.begin(); it != thermometerIds.end(); it++) {
        unsigned int thermometerId = *it;
        // Get thermometer from thermometer service
        Thermometer * thermometer = ThermometerService::_()->get(thermometerId);
        
        if (thermometer == NULL) {
            ERROR("Temperature reader %d cannot be added to mash controller \"%s\", id = %d because it does not exist", thermometerId, mashController->getName().c_str(), mashController->getId());
        } else {
            // Add it
            ERROR("Adding thermometer %d to mash controller \"%s\", id = %d", thermometerId, mashController->getName().c_str(), mashController->getId());
            mashController->addThermometer(thermometer);
        }
    }
}

MashController * MashControllerService::get(unsigned int id) const {
    LOG("Getting mash controller with Id %d", id);

    // Check if it exist
    std::vector<MashController *>::const_iterator it = _mashControllers.begin();
    while (it != _mashControllers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _mashControllers.end()) {
        WARN("Unable to get mash controller with Id %d as it does not exist", id);
        return NULL;
    }

    MashController * mashController = *it;
    DEBUG("Got mash controller \"%s\", id = %d", mashController->getName().c_str(), mashController->getId());
        
}

const std::vector<MashController *> & MashControllerService::getAll() const {
    LOG("Getting all mash controllers");
  
    return _mashControllers;
}

bool MashControllerService::erase(unsigned int id) {
    LOG("Deleting mash controller with Id %d", id);
        
    // Check if it exist
    std::vector<MashController *>::iterator it = _mashControllers.begin();
    while (it != _mashControllers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _mashControllers.end()) {
        WARN("Unable to delete mash controller with Id %d as it does not exist", id);
        return false;
    }

    // Obtain current mash controller
    MashController * mashController = *it;
    
    // Remove from map
    _mashControllers.erase(it);

    delete mashController;

    // Save current mashControllers
    this->save();

    return true;
}

void MashControllerService::createDefaultMashController() {
    MashControllerConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.name = "mash controller 0";
    defaultConfig.thermometerIds.push_back(1);

    this->add(defaultConfig);
}

void MashControllerService::save() {
    LOG("Saving configuration with current mash controllers");

    std::vector<MashControllerConfig> mashControllerConfigs;
    for (std::vector<MashController *>::iterator it = _mashControllers.begin(); it != _mashControllers.end(); it++) {
        mashControllerConfigs.push_back(*((*it)->getConfig()));
    }

    Configuration::properties.mashControllers = mashControllerConfigs;

    Configuration::save();
}
