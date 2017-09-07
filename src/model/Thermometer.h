#ifndef THERMOMETER_H
#define THERMOMETER_H

#include "ThermometerConfig.h"

class Thermometer : public Jsonable {

public:
    Thermometer(const ThermometerConfig & config) :
        _config(config) {}
    virtual ~Thermometer() {}

    const ThermometerConfig * getConfig() const {
        return &_config;
    }

    const unsigned int getId() const {
        return _config.id;
    }

    unsigned int setId(unsigned int id) {
        _config.id = id;
    }

    const String & getName() const {
        return _config.name;
    }

    void setName(String name) {
        _config.name = name;
    }

    unsigned int getPort() const {
        return _config.port;
    }

    void setPort(unsigned int port) {
        _config.port = port;
    }

    bool getPortIsValid() const {
        return _config.isPortValid;
    }

    void setPortIsValid(bool isValid) {
        _config.isPortValid = isValid;
    }
    
    float getTemperature() const {
        return _temperature;
    }

    void readTemperature();
    
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }
        
private:
    ThermometerConfig _config;
    bool _isPortValid;
    float _temperature;
};

#endif /*THERMOMETER_H*/