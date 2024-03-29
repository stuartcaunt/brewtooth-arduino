#ifndef RELAY_H
#define RELAY_H

#include "RelayConfig.h"

class Relay : public Jsonable {

public:
    Relay(const RelayConfig & config) :
        _config(config) {}
    virtual ~Relay() {}

    const RelayConfig * getConfig() const {
        return &_config;
    }

    uint8_t getPort() const {
        return _config.port;
    }

    void setPort(uint8_t port) {
        _config.port = port;
    }

    bool getPortIsValid() const {
        return _config.isPortValid;
    }

    void setPortIsValid(bool isValid) {
        _config.isPortValid = isValid;
    }

    bool isActive() const {
        return _config.active;
    }
    
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }
        
    bool setActive(bool active);

private:
    RelayConfig _config;
};

#endif /*RELAY_H*/