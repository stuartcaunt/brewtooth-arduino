#ifndef TEMPERATUREREADERCONFIG_H
#define TEMPERATUREREADERCONFIG_H

#include <Arduino.h>

struct TemperatureReaderConfig {
    unsigned int id;
    unsigned int port;
    String name;
    bool isPortValid;
};

#endif /*TEMPERATUREREADERCONFIG_H*/