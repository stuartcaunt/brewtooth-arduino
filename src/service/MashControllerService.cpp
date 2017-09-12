#include "MashControllerService.h"
#include "ThermometerService.h"
#include <model/MashController.h>
#include <model/Relay.h>
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

    // Handle heater and agitator
    this->setHeater(mashController, mashControllerConfig.heater);
    this->setAgitator(mashController, mashControllerConfig.agitator);

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
    mashController->copyBasic(mashControllerConfig);

    // Temperature readers : clear current ones and add new ones
    mashController->clearThermometers();
    this->addThermometers(mashController, mashControllerConfig.thermometerIds);

    // Handle heater and agitator
    this->setHeater(mashController, mashControllerConfig.heater);
    this->setAgitator(mashController, mashControllerConfig.agitator);

    // Save current mashControllers
    this->save();

    return mashController;
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
    
    return mashController;
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

Relay * MashControllerService::updateHeater(unsigned int mashControllerId, const RelayConfig & relayConfig) {
    LOG("Updating heater for mash controller with Id %d", mashControllerId);
    
    // Check if it exist
    std::vector<MashController *>::iterator it = _mashControllers.begin();
    while (it != _mashControllers.end() && ((*it)->getId() != mashControllerId)) {
        it++;
    }

    if (it == _mashControllers.end()) {
        WARN("Unable to update heater for mash controller with Id %d as it does not exist", mashControllerId);
        return NULL;
    }

    // Obtain current mash controller
    MashController * mashController = *it;

    // Set new heater
    this->setHeater(mashController, relayConfig);

    // Save current mashControllers
    this->save();

    return mashController->getHeater();;
}


Relay * MashControllerService::updateAgitator(unsigned int mashControllerId, const RelayConfig & relayConfig) {
    LOG("Updating agitator for mash controller with Id %d", mashControllerId);
    
    // Check if it exist
    std::vector<MashController *>::iterator it = _mashControllers.begin();
    while (it != _mashControllers.end() && ((*it)->getId() != mashControllerId)) {
        it++;
    }

    if (it == _mashControllers.end()) {
        WARN("Unable to update agitator for mash controller with Id %d as it does not exist", mashControllerId);
        return NULL;
    }

    // Obtain current mash controller
    MashController * mashController = *it;

    // Set new heater
    this->setAgitator(mashController, relayConfig);

    // Save current mashControllers
    this->save();

    return mashController->getHeater();;
}

void MashControllerService::addThermometers(MashController * mashController, const std::vector<unsigned int> & thermometerIds) {
    for (std::vector<unsigned int>::const_iterator it = thermometerIds.begin(); it != thermometerIds.end(); it++) {
        unsigned int thermometerId = *it;
        // Get thermometer from thermometer service
        ThermometerWire * thermometerWire = ThermometerService::_()->get(thermometerId);
        
        if (thermometerWire == NULL) {
            ERROR("Temperature reader %d cannot be added to mash controller \"%s\", id = %d because it does not exist", thermometerId, mashController->getName().c_str(), mashController->getId());
        } else {
            // Add it
            LOG("Adding thermometer %d to mash controller \"%s\", id = %d", thermometerId, mashController->getName().c_str(), mashController->getId());
            mashController->addThermometer(thermometerWire);
        }
    }
}

void MashControllerService::setHeater(MashController * mashController, const RelayConfig & heater) {
    LOG("Creating heater: enabled %s, port %d", heater.enabled ? "true" : "false", heater.port);
    Relay * relay = new Relay(heater);
    LOG("Adding heater with to mash controller \"%s\", id = %d", mashController->getName().c_str(), mashController->getId());
    mashController->setHeater(relay);
}

void MashControllerService::setAgitator(MashController * mashController, const RelayConfig & agitator) {
    LOG("Creating agitator: enabled %s, port %d", agitator.enabled ? "true" : "false", agitator.port);
    Relay * relay = new Relay(agitator);
    LOG("Adding agitator with to mash controller \"%s\", id = %d", mashController->getName().c_str(), mashController->getId());
    mashController->setAgitator(relay);
}

void MashControllerService::createDefaultMashController() {
    // Thermometers
    MashControllerConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.name = "mash controller 0";
    defaultConfig.thermometerIds.push_back(1);

    // Heater
    RelayConfig defaultHeater;
    defaultHeater.enabled = true;
    defaultHeater.port = 4;
    defaultConfig.heater = defaultHeater;

    // Agitator
    RelayConfig defaultAgitator;
    defaultHeater.enabled = false;
    defaultHeater.port = 5;
    defaultConfig.agitator = defaultAgitator;

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

void MashControllerService::updateControllers() {
    for (std::vector<MashController *>::iterator it = _mashControllers.begin(); it != _mashControllers.end(); it++) {
        MashController * mashController = *it;
        mashController->update();
    }
}
