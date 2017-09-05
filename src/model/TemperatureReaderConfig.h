#ifndef TEMPERATUREREADERCONFIG_H
#define TEMPERATUREREADERCONFIG_H

#include "Jsonable.h"

class TemperatureReaderConfig : public Jsonable {
public:
    TemperatureReaderConfig() :
        id(0),
        port(0),
        name(""),
        isPortValid(false) {}

    TemperatureReaderConfig(const JsonObject & json) :
        id(json["id"]),
        port(json["port"]),
        name(json["name"].as<String>()),
        isPortValid(false) {}

    virtual void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["port"] = port;
        json["name"] = name;
        json["isValid"] = isPortValid;
    }
    
public:
    unsigned int id;
    unsigned int port;
    String name;
    bool isPortValid;
};

#endif /*TEMPERATUREREADERCONFIG_H*/