#include "TemperatureReaderService.h"

TemperatureReaderService * TemperatureReaderService::instance = 0;

TemperatureReaderService * TemperatureReaderService::_() {
    if (instance == 0) {
        instance = new TemperatureReaderService();
    }

    return instance;
}

