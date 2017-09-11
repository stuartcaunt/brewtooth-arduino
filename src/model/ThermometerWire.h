#ifndef THERMOMETERWIRE_H
#define THERMOMETERWIRE_H

#include "ThermometerWireConfig.h"
#include <vector>

class OneWire;
class DallasTemperature;
typedef uint8_t DeviceAddress[8];

struct Thermometer {
    DeviceAddress deviceAddress;
    float temperatureC;
};

class ThermometerWire : public Jsonable {
public:
    ThermometerWire(const ThermometerWireConfig & config) :
        _config(config),
        _temperatureReading(false),
        _meanTemperatureC(0.0),
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
    
    float getMeanTemperatureC() const {
        return _meanTemperatureC;
    }

    const std::vector<Thermometer> & getThermometers() const {
        return _thermometers;
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
    float _meanTemperatureC;

    OneWire * _oneWire;
    DallasTemperature * _sensors;
    bool _devicesAvailable;
    std::vector<Thermometer> _thermometers;
};

#endif /*THERMOMETER_HWIRE*/