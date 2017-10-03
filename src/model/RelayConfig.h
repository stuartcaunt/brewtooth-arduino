#ifndef RELAYCONFIG_H
#define RELAYCONFIG_H

#include "Jsonable.h"

class RelayConfig : public Jsonable {
public:
    RelayConfig() :
        enabled(false),
        active(false),
        port(0),
        isPortValid(false) {}

    RelayConfig(const JsonObject & json) :
        enabled(json["enabled"]),
        active(json["active"]),
        port(json["port"]),
        isPortValid(json["isValid"]) {}

    RelayConfig & operator=(const RelayConfig & rhs) {
        enabled = rhs.enabled;
        active = rhs.active;
        port = rhs.port;
        isPortValid = rhs.isPortValid;
    }

    virtual void convertToJson(JsonObject & json) const {
        json["enabled"] = enabled;
        json["active"] = active;
        json["port"] = port;
        json["isValid"] = isPortValid;
    }

public:
    bool enabled;
    bool active;
    uint8_t port;
    bool isPortValid;
};

#endif /*RELAYCONFIG_H*/