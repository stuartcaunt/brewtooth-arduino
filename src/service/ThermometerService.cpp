#include "ThermometerService.h"
#include "GPIOService.h"
#include <utils/Configuration.h>
#include <utils/Log.h>

ThermometerService * ThermometerService::instance = 0;

ThermometerService::ThermometerService() {
    
}

ThermometerService::~ThermometerService() {
    for (std::vector<Thermometer *>::iterator it = _thermometers.begin(); it != _thermometers.end(); it++) {
        delete *it;
    }

    _thermometers.clear();
}

void ThermometerService::init() {
    if (instance == 0) {
        instance = new ThermometerService();

        const std::vector<ThermometerConfig> & thermometers = Configuration::properties.thermometers;
        bool isFirstUse = Configuration::properties.isFirstUse;
    
        LOG("Initialising Thermometer Service");

        if (isFirstUse) {
            LOG("Creating default thermometer");
            instance->createDefaultThermometer();

        } else {
            LOG("Adding configured thermometers");
            for (std::vector<ThermometerConfig>::const_iterator it = thermometers.begin(); it != thermometers.end(); it++) {
                instance->add(*it, false);
            }
        }
    }
}

ThermometerService * ThermometerService::_() {
    return instance;
}

Thermometer * ThermometerService::add(const ThermometerConfig & thermometerConfig, bool save) {
    LOG("Adding thermometer \"%s\", id = %d, port = %d ", thermometerConfig.name.c_str(), thermometerConfig.id, thermometerConfig.port);
    
    // Create a new Thermometer
    Thermometer * thermometer = new Thermometer(thermometerConfig);

    if (thermometerConfig.id == 0) {
        // Get first available id
        unsigned int maxId = 0;
        for (std::vector<Thermometer *>::iterator it = _thermometers.begin(); it != _thermometers.end(); it++) {
            maxId = maxId < (*it)->getId() ? (*it)->getId() : maxId;
        }

        unsigned int nextAvailableId = maxId + 1;

        LOG("Creating new thermometer with Id %d", nextAvailableId);
        thermometer->setId(nextAvailableId);
    
    } else {
        // Check if it exist
        std::vector<Thermometer *>::iterator it = _thermometers.begin();
        while (it != _thermometers.end() && ((*it)->getId() != thermometerConfig.id)) {
            it++;
        }

        // replace if needed
        if (it != _thermometers.end()) {
            LOG("Thermometer with Id %d already exists: replacing it", thermometerConfig.id);
            _thermometers.erase(it);
        }
    }

    // Add to all thermometers
    _thermometers.push_back(thermometer);

    // Check/acquire GPIO port
    thermometer->setPortIsValid(GPIOService::_()->acquire(thermometerConfig.port));
    if (thermometer->getPortIsValid()) {
        GPIOService::_()->setPinMode(thermometer->getPort(), INPUT);

    } else {
        WARN("Thermometer \"%s\" has an invalid GPIO port", thermometer->getName().c_str(), thermometer->getPort());
    }

    // Save current thermometers
    if (save) {
        this->save();
    }

    return thermometer;
}

Thermometer * ThermometerService::update(const ThermometerConfig & thermometerConfig) {
    LOG("Updating thermometer \"%s\", id = %d, port = %d ", thermometerConfig.name.c_str(), thermometerConfig.id, thermometerConfig.port);
    
    // Check if it exist
    std::vector<Thermometer *>::iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != thermometerConfig.id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to update thermometer with Id %d as it does not exist", thermometerConfig.id);
        return NULL;
    }

    // Obtain current thermometer
    Thermometer * thermometer = *it;

    // Release current port
    GPIOService::_()->release(thermometer->getPort());

    // Copy data
    thermometer->setPort(thermometerConfig.port);
    thermometer->setName(thermometerConfig.name);

    // Check/acquire GPIO port
    thermometer->setPortIsValid(GPIOService::_()->acquire(thermometerConfig.port));
    if (!thermometer->getPortIsValid()) {
        WARN("Thermometer \"%s\" has an invalid GPIO port", thermometer->getName().c_str(), thermometer->getPort());
    }

    // Save current thermometers
    this->save();

    return thermometer;
}

Thermometer * ThermometerService::get(unsigned int id) const {
    LOG("Getting thermometer with Id %d", id);

    // Check if it exist
    std::vector<Thermometer *>::const_iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to get thermometer with Id %d as it does not exist", id);
        return NULL;
    }

    Thermometer * temperatureReader = *it;
    DEBUG("Got thermometer \"%s\", id = %d, port = %d ", temperatureReader->getName().c_str(), temperatureReader->getId(), temperatureReader->getPort());
    
    return temperatureReader;
}

const std::vector<Thermometer *> & ThermometerService::getAll() const {
    LOG("Getting all thermometers");
  
    return _thermometers;
}

bool ThermometerService::erase(unsigned int id) {
    LOG("Deleting thermometer with Id %d", id);
    
    // Check if it exist
    std::vector<Thermometer *>::iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to delete thermometer with Id %d as it does not exist", id);
        return false;
    }

    // Obtain current thermometer
    Thermometer * thermometer = *it;
    
    // Remove from map
    _thermometers.erase(it);

    // Release port
    GPIOService::_()->release(thermometer->getPort());
    
    delete thermometer;

    // Save current thermometers
    this->save();

    return true;
}

float ThermometerService::getTemperature(unsigned int id) const {
    LOG("Getting temperature from thermometer with Id %d", id);

    // Check if it exist
    std::vector<Thermometer *>::const_iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to get temperature from thermometer with Id %d as it does not exist", id);
        return 0.0;
    }

    return (*it)->getTemperature();
}

float ThermometerService::getMeanTemperature() const {
    LOG("Getting mean temperature");
    float meanTemperature = 0.0;
    for (std::vector<Thermometer *>::const_iterator it = _thermometers.begin(); it != _thermometers.end(); it++) {
        meanTemperature += (*it)->getTemperature();
    }

    meanTemperature /= _thermometers.size();
    LOG("Mean temperature calculated as %f from %d thermometer(s)", meanTemperature, _thermometers.size());
    
    return meanTemperature;
}

void ThermometerService::createDefaultThermometer() {
    ThermometerConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.port = 9;
    defaultConfig.name = "thermometer 0";

    this->add(defaultConfig);
}

void ThermometerService::save() {
    LOG("Saving configuration with current thermometers");
    std::vector<ThermometerConfig> thermometerConfigs;
    for (std::vector<Thermometer *>::iterator it = _thermometers.begin(); it != _thermometers.end(); it++) {
        thermometerConfigs.push_back(*((*it)->getConfig()));
    }

    Configuration::properties.thermometers = thermometerConfigs;

    Configuration::save();
}

void ThermometerService::readTemperatures() {

}
