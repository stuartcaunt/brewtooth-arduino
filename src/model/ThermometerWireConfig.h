#ifndef THERMOMETERWIRECONFIG_H
#define THERMOMETERWIRECONFIG_H

#include "Jsonable.h"

class ThermometerWireConfig : public Jsonable {
public:
    ThermometerWireConfig() :
        id(0),
        port(0),
        name(""),
        isPortValid(false) {}

    ThermometerWireConfig(const ThermometerWireConfig & obj) :
        id(obj.id),
        port(obj.port),
        name(obj.name),
        isPortValid(obj.isPortValid) {}

    ThermometerWireConfig(const JsonObject & json) :
        id(json["id"]),
        port(json["port"]),
        name(json["name"].as<String>()),
        isPortValid(json["isValid"]) {}

    ThermometerWireConfig & operator=(const ThermometerWireConfig & rhs) {
        id = rhs.id;
        port = rhs.port;
        name = rhs.name;
        isPortValid = rhs.isPortValid;
        return *this;
    }

    virtual void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["port"] = port;
        json["name"] = name;
        json["isValid"] = isPortValid;
    }
    
public:
    unsigned int id;
    uint8_t port;
    String name;
    bool isPortValid;
};

#endif /*THERMOMETERWIRECONFIG_H*/