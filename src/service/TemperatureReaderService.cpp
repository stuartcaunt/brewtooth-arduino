#include "TemperatureReaderService.h"
#include "GPIOService.h"
#include <utils/Configuration.h>
#include <utils/Log.h>

TemperatureReaderService * TemperatureReaderService::instance = 0;

TemperatureReaderService::TemperatureReaderService() {
    
}

TemperatureReaderService::~TemperatureReaderService() {
    for (std::vector<TemperatureReader *>::iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        delete *it;
    }

    _temperatureReaders.clear();
}

void TemperatureReaderService::init() {
    if (instance == 0) {
        instance = new TemperatureReaderService();

        const std::vector<TemperatureReaderConfig> & temperatureReaders = Configuration::properties.temperatureReaders;
        bool isFirstUse = Configuration::properties.isFirstUse;
    
        LOG("Initialising Temperature Reader Service");

        if (isFirstUse) {
            LOG("Creating default temperature reader");
            instance->createDefaultTemperatureReader();

        } else {
            LOG("Adding configured temperature readers");
            for (std::vector<TemperatureReaderConfig>::const_iterator it = temperatureReaders.begin(); it != temperatureReaders.end(); it++) {
                instance->add(*it, false);
            }
        }
    }
}

TemperatureReaderService * TemperatureReaderService::_() {
    return instance;
}

TemperatureReader * TemperatureReaderService::add(const TemperatureReaderConfig & readerConfig, bool save) {
    LOG("Adding temperature reader \"%s\", id = %d, port = %d ", readerConfig.name.c_str(), readerConfig.id, readerConfig.port);
    
    // Create a new TemperatureReader
    TemperatureReader * reader = new TemperatureReader(readerConfig);

    if (readerConfig.id == 0) {
        // Get first available id
        unsigned int maxId = 0;
        for (std::vector<TemperatureReader *>::iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
            maxId = maxId < (*it)->getId() ? (*it)->getId() : maxId;
        }

        unsigned int nextAvailableId = maxId + 1;

        LOG("Creating new temperature reader with Id %d", nextAvailableId);
        reader->setId(nextAvailableId);
    
    } else {
        // Check if it exist
        std::vector<TemperatureReader *>::iterator it = _temperatureReaders.begin();
        while (it != _temperatureReaders.end() && ((*it)->getId() != readerConfig.id)) {
            it++;
        }

        // replace if needed
        if (it != _temperatureReaders.end()) {
            LOG("Temperature reader with Id %d already exists: replacing it", readerConfig.id);
            _temperatureReaders.erase(it);
        }
    }

    // Add to all readers
    _temperatureReaders.push_back(reader);

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

TemperatureReader * TemperatureReaderService::update(const TemperatureReaderConfig & readerConfig) {
    LOG("Updating temperature reader \"%s\", id = %d, port = %d ", readerConfig.name.c_str(), readerConfig.id, readerConfig.port);
    
    // Check if it exist
    std::vector<TemperatureReader *>::iterator it = _temperatureReaders.begin();
    while (it != _temperatureReaders.end() && ((*it)->getId() != readerConfig.id)) {
        it++;
    }

    if (it == _temperatureReaders.end()) {
        WARN("Unable to update temperature reader with Id %d as it does not exist", readerConfig.id);
        return NULL;
    }

    // Obtain current temperature reader
    TemperatureReader * reader = *it;

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

TemperatureReader * TemperatureReaderService::get(unsigned int id) const {
    LOG("Getting temperature reader with Id %d", id);

    // Check if it exist
    std::vector<TemperatureReader *>::const_iterator it = _temperatureReaders.begin();
    while (it != _temperatureReaders.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _temperatureReaders.end()) {
        WARN("Unable to get temperature reader with Id %d as it does not exist", id);
        return NULL;
    }

    TemperatureReader * temperatureReader = *it;
    DEBUG("Got temperature reader \"%s\", id = %d, port = %d ", temperatureReader->getName().c_str(), temperatureReader->getId(), temperatureReader->getPort());
    
    return temperatureReader;
}

const std::vector<TemperatureReader *> & TemperatureReaderService::getAll() const {
    LOG("Getting all temperature readers");
  
    return _temperatureReaders;
}

bool TemperatureReaderService::erase(unsigned int id) {
    LOG("Deleting temperature reader with Id %d", id);
    
    // Check if it exist
    std::vector<TemperatureReader *>::iterator it = _temperatureReaders.begin();
    while (it != _temperatureReaders.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _temperatureReaders.end()) {
        WARN("Unable to delete temperature reader with Id %d as it does not exist", id);
        return NULL;
    }

    // Obtain current temperature reader
    TemperatureReader * reader = *it;
    
    // Remove from map
    _temperatureReaders.erase(it);

    // Release port
    GPIOService::_()->release(reader->getPort());
    
    delete reader;

    // Save current readers
    this->save();

    return true;
}

float TemperatureReaderService::getTemperature(unsigned int id) const {
    LOG("Getting temperature from reader with Id %d", id);

    // Check if it exist
    std::vector<TemperatureReader *>::const_iterator it = _temperatureReaders.begin();
    while (it != _temperatureReaders.end() && ((*it)->getId() != id)) {
        it++;
    }

    if (it == _temperatureReaders.end()) {
        WARN("Unable to get temperature from thermometer with Id %d as it does not exist", id);
        return 0.0;
    }

    return (*it)->getTemperature();
}

float TemperatureReaderService::getMeanTemperature() const {
    LOG("Getting mean temperature");
    float meanTemperature = 0.0;
    for (std::vector<TemperatureReader *>::const_iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        meanTemperature += (*it)->getTemperature();
    }

    meanTemperature /= _temperatureReaders.size();
    LOG("Mean temperature calculated as %f from %d reader(s)", meanTemperature, _temperatureReaders.size());
    
    return meanTemperature;
}

void TemperatureReaderService::createDefaultTemperatureReader() {
    TemperatureReaderConfig defaultConfig;
    defaultConfig.id = 1;
    defaultConfig.port = 9;
    defaultConfig.name = "temperature reader 0";

    this->add(defaultConfig);
}

void TemperatureReaderService::save() {
    LOG("Saving configuration with current temperature readers");
    std::vector<TemperatureReaderConfig> readerConfigs;
    for (std::vector<TemperatureReader *>::iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        readerConfigs.push_back(*((*it)->getConfig()));
    }

    Configuration::properties.temperatureReaders = readerConfigs;

    Configuration::save();
}

void TemperatureReaderService::readTemperatures() {

}
