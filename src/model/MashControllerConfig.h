#ifndef MASHCONTROLLERCONFIG_H
#define MASHCONTROLLERCONFIG_H

#include "TemperatureReaderConfig.h"
#include <vector>

struct MashControllerConfig {
    MashControllerConfig() :
        id(0),
        name("") {}

    unsigned int id;
    String name;
    std::vector<TemperatureReaderConfig> temperatureReaders;
};

#endif /*MASHCONTROLLERCONFIG_H*/