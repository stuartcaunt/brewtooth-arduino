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

    virtual void convertToJson(JsonObject & json) const {
        json["enabled"] = enabled;
        json["active"] = active;
        json["port"] = port;
        json["isValid"] = isPortValid;
    }

public:
    bool enabled;
    bool active;
    unsigned int port;
    bool isPortValid;
};

#endif /*RELAYCONFIG_H*/