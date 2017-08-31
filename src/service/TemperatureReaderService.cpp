#include "TemperatureReaderService.h"
#include "GPIOService.h"
#include <utils/Configuration.h>

TemperatureReaderService * TemperatureReaderService::instance = 0;

TemperatureReaderService::TemperatureReaderService() {
    
}

TemperatureReaderService::~TemperatureReaderService() {
    for (std::map<unsigned int, TemperatureReader *>::iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        delete it->second;
    }

    _temperatureReaders.clear();
}

void TemperatureReaderService::init() {
    const std::vector<TemperatureReaderConfig> & temperatureReaders = Configuration::properties.temperatureReaders;
    bool isFirstUse = Configuration::properties.isFirstUse;

    if (instance == 0) {
        instance = new TemperatureReaderService();

        Serial.println("Initialising Temperature Reader Service");

        if (isFirstUse) {
            Serial.println("Creating default temperature reader");
            instance->createDefaultTemperatureReader();

        } else {
            Serial.println("Adding configured temperature readers");
            for (std::vector<TemperatureReaderConfig>::const_iterator it = temperatureReaders.begin(); it != temperatureReaders.end(); it++) {
                instance->add(*it, false);
            }
        }
    }
}

TemperatureReaderService * TemperatureReaderService::_() {
    return instance;
}

void TemperatureReaderService::add(const TemperatureReaderConfig & readerConfig, bool save) {
    Serial.println(String("Adding temperature reader \"") + readerConfig.name + String("\" with GPIO port ") + readerConfig.port);
    
    // Create a new TemperatureReader
    TemperatureReader * reader = new TemperatureReader(readerConfig);

    if (readerConfig.id == 0) {
        // Get first available id
        unsigned int nextAvailableId = 1;
        while (_temperatureReaders.find(nextAvailableId) != _temperatureReaders.end()) {
            nextAvailableId++;
        }

        Serial.println(String("Creating new temperature reader with Id ") + nextAvailableId);
        reader->setId(nextAvailableId);
    }

    // Add to all readers
    _temperatureReaders[readerConfig.id] = reader;

    // Check/acquire GPIO port
    reader->setPortIsValid(GPIOService::_()->acquire(readerConfig.port));
    if (!reader->getPortIsValid()) {
        Serial.println(String("Temperature reader \"") + reader->getName() + "\" has in invalid GPIO port: " + reader->getPort());
    }

    // Save current readers
    if (save) {
        this->save();
    }
}

void TemperatureReaderService::update(const TemperatureReaderConfig & readerConfig) {
    Serial.println(String("Updating temperature reader ") + readerConfig.id + String(": \"") + readerConfig.name + String("\" with GPIO port ") + readerConfig.port);
    
    if (_temperatureReaders.find(readerConfig.id) == _temperatureReaders.end()) {
        Serial.println("Unable to update temperature reader with Id " + readerConfig.id + String(" as it does not exist"));
        return;
    }

    // Obtain current temperature reader
    TemperatureReader * reader = _temperatureReaders[readerConfig.id];

    // Release current port
    GPIOService::_()->release(reader->getPort());

    // Copy data
    reader->setPort(readerConfig.port);
    reader->setName(readerConfig.name);

    // Check/acquire GPIO port
    reader->setPortIsValid(GPIOService::_()->acquire(readerConfig.port));
    if (!reader->getPortIsValid()) {
        Serial.println("Temperature reader \"" + reader->getName() + "\" has in invalid GPIO port: " + reader->getPort());
    }

    // Save current readers
    this->save();
}

const TemperatureReaderConfig * TemperatureReaderService::get(unsigned int id) const {
    Serial.println("Getting temperature reader with Id " + id);

    std::map<unsigned int, TemperatureReader *>::const_iterator it = _temperatureReaders.find(id);
    if (it == _temperatureReaders.end()) {
        Serial.println("Unable to get temperature reader with Id " + id + String(" as it does not exist"));
        return NULL;
    }

    return it->second->getConfig();
}

void TemperatureReaderService::erase(unsigned int id) {
    Serial.println("Deleting temperature reader with Id " + id);

    std::map<unsigned int, TemperatureReader *>::iterator it = _temperatureReaders.find(id);
    if (it == _temperatureReaders.end()) {
        Serial.println("Unable to delete temperature reader with Id " + id + String(" as it does not exist"));
        return;
    }

    // Obtain current temperature reader
    TemperatureReader * reader = it->second;
    
    // Remove from map
    _temperatureReaders.erase(it);

    // Release port
    GPIOService::_()->release(reader->getPort());
    
    delete reader;

    // Save current readers
    this->save();
}

float TemperatureReaderService::getTemperature(unsigned int id) const {
    Serial.println("Getting temperature from reader with Id " + id);

    std::map<unsigned int, TemperatureReader *>::const_iterator it = _temperatureReaders.find(id);
    if (it == _temperatureReaders.end()) {
        Serial.println("Unable to get temperature from reader with Id " + id + String(" as it does not exist"));
        return 0.0;
    }

    return it->second->getTemperature();
}

float TemperatureReaderService::getMeanTemperature() const {
    Serial.println("Getting mean temperature");
    float meanTemperature = 0.0;
    for (std::map<unsigned int, TemperatureReader *>::const_iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        meanTemperature += it->second->getTemperature();
    }

    meanTemperature /= _temperatureReaders.size();
    Serial.println(String("Mean temperature calculated as ") + meanTemperature + String(" from ") + _temperatureReaders.size() + String(" reader(s)"));
    
    return meanTemperature;
}

void TemperatureReaderService::createDefaultTemperatureReader() {
    TemperatureReaderConfig defaultConfig;
    defaultConfig.port = 9;
    defaultConfig.name = "temperature reader 0";

    this->add(defaultConfig);
}

void TemperatureReaderService::save() {
    Serial.println("Saving configuration with current temperature readers");
    std::vector<TemperatureReaderConfig> readerConfigs;
    for (std::map<unsigned int, TemperatureReader *>::const_iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        readerConfigs.push_back(*(it->second->getConfig()));
    }

    Configuration::properties.temperatureReaders = readerConfigs;

    Configuration::save();
}

void TemperatureReaderService::readTemperatures() {

}
