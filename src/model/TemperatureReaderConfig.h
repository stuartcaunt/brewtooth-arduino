#ifndef TEMPERATUREREADERCONFIG_H
#define TEMPERATUREREADERCONFIG_H

#include <Arduino.h>

struct TemperatureReaderConfig {
    TemperatureReaderConfig() :
        id(0),
        port(0),
        name(""),
        isPortValid(false) {}

    unsigned int id;
    unsigned int port;
    String name;
    bool isPortValid;
};

#endif /*TEMPERATUREREADERCONFIG_H*/