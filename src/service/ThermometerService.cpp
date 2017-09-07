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
    
        LOG("Initialising Temperature Reader Service");

        if (isFirstUse) {
            LOG("Creating default temperature reader");
            instance->createDefaultThermometer();

        } else {
            LOG("Adding configured temperature readers");
            for (std::vector<ThermometerConfig>::const_iterator it = thermometers.begin(); it != thermometers.end(); it++) {
                instance->add(*it, false);
            }
        }
    }
}

ThermometerService * ThermometerService::_() {
    return instance;
}

Thermometer * ThermometerService::add(const ThermometerConfig & readerConfig, bool save) {
    LOG("Adding temperature reader \"%s\", id = %d, port = %d ", readerConfig.name.c_str(), readerConfig.id, readerConfig.port);
    
    // Create a new Thermometer
    Thermometer * reader = new Thermometer(readerConfig);

    if (readerConfig.id == 0) {
        // Get first available id
        unsigned int maxId = 0;
        for (std::vector<Thermometer *>::iterator it = _thermometers.begin(); it != _thermometers.end(); it++) {
            maxId = maxId < (*it)->getId() ? (*it)->getId() : maxId;
        }

        unsigned int nextAvailableId = maxId + 1;

        LOG("Creating new temperature reader with Id %d", nextAvailableId);
        reader->setId(nextAvailableId);
    
    } else {
        // Check if it exist
        std::vector<Thermometer *>::iterator it = _thermometers.begin();
        while (it != _thermometers.end() && ((*it)->getId() != readerConfig.id)) {
            it++;
        }

        // replace if needed
        if (it != _thermometers.end()) {
            LOG("Temperature reader with Id %d already exists: replacing it", readerConfig.id);
            _thermometers.erase(it);
        }
    }

    // Add to all readers
    _thermometers.push_back(reader);

    // Check/acquire GPIO port
    reader->setPortIsValid(GPIOService::_()->acquire(readerConfig.port));
    if (!reader->getPortIsValid()) {
        WARN("Temperature reader \"%s\" has an invalid GPIO port", reader->getName().c_str(), reader->getPort());
    }

    // Save current readers
    if (save) {
        this->save();
    }

    return reader;
}

Thermometer * ThermometerService::update(const ThermometerConfig & readerConfig) {
    LOG("Updating temperature reader \"%s\", id = %d, port = %d ", readerConfig.name.c_str(), readerConfig.id, readerConfig.port);
    
    // Check if it exist
    std::vector<Thermometer *>::iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != readerConfig.id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to update temperature reader with Id %d as it does not exist", readerConfig.id);
        return NULL;
    }

    // Obtain current temperature reader
    Thermometer * reader = *it;

    // Release current port
    GPIOService::_()->release(reader->getPort());

    // Copy data
    reader->setPort(readerConfig.port);
    reader->setName(readerConfig.name);

    // Check/acquire GPIO port
    reader->setPortIsValid(GPIOService::_()->acquire(readerConfig.port));
    if (!reader->getPortIsValid()) {
        WARN("Temperature reader \"%s\" has an invalid GPIO port", reader->getName().c_str(), reader->getPort());
    }

    // Save current readers
    this->save();

    return reader;
}

Thermometer * ThermometerService::get(unsigned int id) const {
    LOG("Getting temperature reader with Id %d", id);

    // Check if it exist
    std::vector<Thermometer *>::const_iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to get temperature reader with Id %d as it does not exist", id);
        return NULL;
    }

    Thermometer * temperatureReader = *it;
    DEBUG("Got temperature reader \"%s\", id = %d, port = %d ", temperatureReader->getName().c_str(), temperatureReader->getId(), temperatureReader->getPort());
    
    return temperatureReader;
}

const std::vector<Thermometer *> & ThermometerService::getAll() const {
    LOG("Getting all temperature readers");
  
    return _thermometers;
}

bool ThermometerService::erase(unsigned int id) {
    LOG("Deleting temperature reader with Id %d", id);
    
    // Check if it exist
    std::vector<Thermometer *>::iterator it = _thermometers.begin();
    while (it != _thermometers.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _thermometers.end()) {
        WARN("Unable to delete temperature reader with Id %d as it does not exist", id);
        return NULL;
    }

    // Obtain current temperature reader
    Thermometer * reader = *it;
    
    // Remove from map
    _thermometers.erase(it);

    // Release port
    GPIOService::_()->release(reader->getPort());
    
    delete reader;

    // Save current readers
    this->save();

    return true;
}

float ThermometerService::getTemperature(unsigned int id) const {
    LOG("Getting temperature from reader with Id %d", id);

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
    LOG("Mean temperature calculated as %f from %d reader(s)", meanTemperature, _thermometers.size());
    
    return meanTemperature;
}

void ThermometerService::createDefaultThermometer() {
    ThermometerConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.port = 9;
    defaultConfig.name = "temperature reader 0";

    this->add(defaultConfig);
}

void ThermometerService::save() {
    LOG("Saving configuration with current temperature readers");
    std::vector<ThermometerConfig> readerConfigs;
    for (std::vector<Thermometer *>::iterator it = _thermometers.begin(); it != _thermometers.end(); it++) {
        readerConfigs.push_back(*((*it)->getConfig()));
    }

    Configuration::properties.thermometers = readerConfigs;

    Configuration::save();
}

void ThermometerService::readTemperatures() {

}
