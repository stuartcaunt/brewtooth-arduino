#include "ThermometerService.h"
#include "GPIOService.h"
#include <model/ThermometerWire.h>
#include <utils/Configuration.h>
#include <utils/Log.h>

ThermometerService * ThermometerService::instance = 0;

ThermometerService::ThermometerService() {
    
}

ThermometerService::~ThermometerService() {
    for (std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        delete *it;
    }

    _thermometerWires.clear();
}

void ThermometerService::init() {
    if (instance == 0) {
        instance = new ThermometerService();

        const std::vector<ThermometerWireConfig> & thermometerWires = Configuration::properties.thermometers;
        bool isFirstUse = Configuration::properties.isFirstUse;
    
        LOG("Initialising Thermometer Service");

        if (isFirstUse) {
            LOG("Creating default thermometerWire");
            instance->createDefaultThermometer();

        } else {
            LOG("Adding configured thermometerWires");
            for (std::vector<ThermometerWireConfig>::const_iterator it = thermometerWires.begin(); it != thermometerWires.end(); it++) {
                instance->add(*it, false);
            }
        }
    }
}

ThermometerService * ThermometerService::_() {
    return instance;
}

ThermometerWire * ThermometerService::add(const ThermometerWireConfig & thermometerWireConfig, bool save) {
    LOG("Adding thermometerWire \"%s\", id = %d, port = %d ", thermometerWireConfig.name.c_str(), thermometerWireConfig.id, thermometerWireConfig.port);
    
    // Create a new Thermometer
    ThermometerWire * thermometerWire = new ThermometerWire(thermometerWireConfig);

    if (thermometerWireConfig.id == 0) {
        // Get first available id
        unsigned int maxId = 0;
        for (std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
            maxId = maxId < (*it)->getId() ? (*it)->getId() : maxId;
        }

        unsigned int nextAvailableId = maxId + 1;

        LOG("Creating new thermometerWire with Id %d", nextAvailableId);
        thermometerWire->setId(nextAvailableId);
    
    } else {
        // Check if it exist
        std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin();
        while (it != _thermometerWires.end() && ((*it)->getId() != thermometerWireConfig.id)) {
            it++;
        }

        // replace if needed
        if (it != _thermometerWires.end()) {
            LOG("Thermometer with Id %d already exists: replacing it", thermometerWireConfig.id);
            _thermometerWires.erase(it);
        }
    }

    // Add to all thermometerWires
    _thermometerWires.push_back(thermometerWire);

    // Check/acquire GPIO port
    thermometerWire->setPortIsValid(GPIOService::_()->acquire(thermometerWireConfig.port));
    if (thermometerWire->getPortIsValid()) {
        GPIOService::_()->setPinMode(thermometerWire->getPort(), INPUT);

        thermometerWire->init();

    } else {
        WARN("Thermometer \"%s\" has an invalid GPIO port", thermometerWire->getName().c_str(), thermometerWire->getPort());
    }

    // Save current thermometerWires
    if (save) {
        this->save();
    }

    return thermometerWire;
}

ThermometerWire * ThermometerService::update(const ThermometerWireConfig & thermometerWireConfig) {
    LOG("Updating thermometerWire \"%s\", id = %d, port = %d ", thermometerWireConfig.name.c_str(), thermometerWireConfig.id, thermometerWireConfig.port);
    
    // Check if it exist
    std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin();
    while (it != _thermometerWires.end() && ((*it)->getId() != thermometerWireConfig.id)) {
        it++;
    }

    if (it == _thermometerWires.end()) {
        WARN("Unable to update thermometerWire with Id %d as it does not exist", thermometerWireConfig.id);
        return NULL;
    }

    // Obtain current thermometerWire
    ThermometerWire * thermometerWire = *it;

    // Release current port
    GPIOService::_()->release(thermometerWire->getPort());

    // Copy data
    thermometerWire->setPort(thermometerWireConfig.port);
    thermometerWire->setName(thermometerWireConfig.name);

    // Check/acquire GPIO port
    thermometerWire->setPortIsValid(GPIOService::_()->acquire(thermometerWireConfig.port));
    if (thermometerWire->getPortIsValid()) {
        GPIOService::_()->setPinMode(thermometerWire->getPort(), INPUT);
        
        thermometerWire->init();
    } else {
        WARN("Thermometer \"%s\" has an invalid GPIO port", thermometerWire->getName().c_str(), thermometerWire->getPort());
    }

    // Save current thermometerWires
    this->save();

    return thermometerWire;
}

ThermometerWire * ThermometerService::get(unsigned int id) const {
    LOG("Getting thermometerWire with Id %d", id);

    // Check if it exist
    std::vector<ThermometerWire *>::const_iterator it = _thermometerWires.begin();
    while (it != _thermometerWires.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometerWires.end()) {
        WARN("Unable to get thermometerWire with Id %d as it does not exist", id);
        return NULL;
    }

    ThermometerWire * temperatureReader = *it;
    DEBUG("Got thermometerWire \"%s\", id = %d, port = %d ", temperatureReader->getName().c_str(), temperatureReader->getId(), temperatureReader->getPort());
    
    return temperatureReader;
}

const std::vector<ThermometerWire *> & ThermometerService::getAll() const {
    LOG("Getting all thermometerWires");
  
    return _thermometerWires;
}

bool ThermometerService::erase(unsigned int id) {
    LOG("Deleting thermometerWire with Id %d", id);
    
    // Check if it exist
    std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin();
    while (it != _thermometerWires.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometerWires.end()) {
        WARN("Unable to delete thermometerWire with Id %d as it does not exist", id);
        return false;
    }

    // Obtain current thermometerWire
    ThermometerWire * thermometerWire = *it;
    
    // Remove from map
    _thermometerWires.erase(it);

    // Release port
    GPIOService::_()->release(thermometerWire->getPort());
    
    delete thermometerWire;

    // Save current thermometerWires
    this->save();

    return true;
}

float ThermometerService::getMeanTemperatureC(unsigned int id) const {
    LOG("Getting temperature from thermometerWire with Id %d", id);

    // Check if it exist
    std::vector<ThermometerWire *>::const_iterator it = _thermometerWires.begin();
    while (it != _thermometerWires.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometerWires.end()) {
        WARN("Unable to get temperature from thermometerWire with Id %d as it does not exist", id);
        return 0.0;
    }

    return (*it)->getMeanTemperatureC();
}

float ThermometerService::getMeanTemperatureC() const {
    LOG("Getting mean temperature");
    float meanTemperature = 0.0;
    for (std::vector<ThermometerWire *>::const_iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        meanTemperature += (*it)->getMeanTemperatureC();
    }

    meanTemperature /= _thermometerWires.size();
    LOG("Mean temperature calculated as %f from %d thermometerWire(s)", meanTemperature, _thermometerWires.size());
    
    return meanTemperature;
}

void ThermometerService::createDefaultThermometer() {
    ThermometerWireConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.port = 5;
    defaultConfig.name = "thermometerWire 0";

    this->add(defaultConfig);
}

void ThermometerService::save() {
    LOG("Saving configuration with current thermometerWires");
    std::vector<ThermometerWireConfig> thermometerWireConfigs;
    for (std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        thermometerWireConfigs.push_back(*((*it)->getConfig()));
    }

    Configuration::properties.thermometers = thermometerWireConfigs;

    Configuration::save();
}

void ThermometerService::readTemperatures() {
    for (std::vector<ThermometerWire *>::iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        ThermometerWire * thermometerWire = *it;
        thermometerWire->readTemperature();
    }
}
