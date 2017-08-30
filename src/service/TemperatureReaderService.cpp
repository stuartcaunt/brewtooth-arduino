#include "TemperatureReaderService.h"

TemperatureReaderService * TemperatureReaderService::instance = 0;

TemperatureReaderService * TemperatureReaderService::_() {
    if (instance == 0) {
        instance = new TemperatureReaderService();
    }

    return instance;
}

void TemperatureReaderService::add(const TemperatureReaderConfig & readerConfig) {

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
