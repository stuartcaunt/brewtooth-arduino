#ifndef THERMOMETERWIRE_H
#define THERMOMETERWIRE_H

#include "ThermometerWireConfig.h"
#include "ThermometerWireData.h"
#include <vector>

class OneWire;
class DallasTemperature;

class ThermometerWire : public Jsonable {
public:
    ThermometerWire(const ThermometerWireConfig & config) :
        _config(config),
        _temperatureReading(false),
        _oneWire(NULL),
        _sensors(NULL),
        _devicesAvailable(false) {}
    virtual ~ThermometerWire() {}

    void init();

    const ThermometerWireConfig * getConfig() const {
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
    
    float getMeanTemperatureC() const {
        return _data.meanTemperatureC;
    }

    const ThermometerWireData & getData() const {
        return _data;
    }

    void readTemperature();

    bool isValid() const {
        return _config.isPortValid && _devicesAvailable;
    }
    
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }
        
private:
    ThermometerWireConfig _config;
    bool _temperatureReading;

    OneWire * _oneWire;
    DallasTemperature * _sensors;
    bool _devicesAvailable;

    ThermometerWireData _data;
};

#endif /*THERMOMETERWIRE_H*/