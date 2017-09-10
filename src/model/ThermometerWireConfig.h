#ifndef THERMOMETERCONFIG_H
#define THERMOMETERCONFIG_H

#include "Jsonable.h"

class ThermometerConfig : public Jsonable {
public:
    ThermometerConfig() :
        id(0),
        port(0),
        name(""),
        isPortValid(false) {}

    ThermometerConfig(const JsonObject & json) :
        id(json["id"]),
        port(json["port"]),
        name(json["name"].as<String>()),
        isPortValid(json["isValid"]) {}

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

#endif /*THERMOMETERCONFIG_H*/