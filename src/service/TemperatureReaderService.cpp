#include "TemperatureReaderService.h"
#include "GPIOService.h"

TemperatureReaderService * TemperatureReaderService::instance = 0;

TemperatureReaderService::TemperatureReaderService() {
    
}

TemperatureReaderService::~TemperatureReaderService() {
    for (std::map<unsigned int, TemperatureReader *>::iterator it = _temperatureReaders.begin(); it != _temperatureReaders.end(); it++) {
        delete it->second;
    }

    _temperatureReaders.clear();
}


TemperatureReaderService * TemperatureReaderService::_() {
    if (instance == 0) {
        instance = new TemperatureReaderService();
    }

    return instance;
}

void TemperatureReaderService::add(const TemperatureReaderConfig & readerConfig) {

    // Create a new TemperatureReader
    TemperatureReader * temperatureReader = new TemperatureReader(readerConfig);

    if (readerConfig.id == 0) {

    }

    _temperatureReaders[readerConfig.id] = temperatureReader;

}

void TemperatureReaderService::update(const TemperatureReaderConfig & readerConfig) {

}

const TemperatureReaderConfig & TemperatureReaderService::get(unsigned int id) const {

}

void TemperatureReaderService::erase(unsigned int id) {

}

float TemperatureReaderService::getTemperature(unsigned int readerId) const {

}

float TemperatureReaderService::getMeanTemperature() const {

}

void TemperatureReaderService::readTemperatures() {

}
