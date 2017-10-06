#ifndef RELAYCONFIG_H
#define RELAYCONFIG_H

#include "Jsonable.h"

class RelayConfig : public Jsonable {
public:
    RelayConfig() :
        active(false),
        port(0),
        isPortValid(false) {}

    RelayConfig(const RelayConfig & obj) :
        active(obj.active),
        port(obj.port),
        isPortValid(obj.isPortValid) {}

    RelayConfig(const JsonObject & json) :
        active(json["active"]),
        port(json["port"]),
        isPortValid(json["isValid"]) {}

    RelayConfig & operator=(const RelayConfig & rhs) {
        active = rhs.active;
        port = rhs.port;
        isPortValid = rhs.isPortValid;
    }

    void copyFromJson(const JsonObject & json) {
        active = json["active"];
        port = json["port"];
        isPortValid = json["isValid"];
    }
    
    virtual void convertToJson(JsonObject & json) const {
        json["active"] = active;
        json["port"] = port;
        json["isValid"] = isPortValid;
    }

public:
    bool active;
    uint8_t port;
    bool isPortValid;
};

#endif /*RELAYCONFIG_H*/