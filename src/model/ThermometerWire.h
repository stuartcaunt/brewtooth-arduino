#ifndef THERMOMETER_H
#define THERMOMETER_H

#include "ThermometerConfig.h"

class OneWire;
class DallasTemperature;
typedef uint8_t DeviceAddress[8];

class Thermometer : public Jsonable {
public:
    Thermometer(const ThermometerConfig & config) :
        _config(config),
        _temperatureReading(false),
        _temperatureC(0.0),
        _oneWire(NULL),
        _sensors(NULL),
        _devicesAvailable(false) {}
    virtual ~Thermometer() {}

    void init();

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
    
    float getTemperatureC() const {
        return _temperatureC;
    }

    void readTemperature();

    bool isValid() const {
        return _config.isPortValid && _devicesAvailable;
    }
    
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }
        
private:
    ThermometerConfig _config;
    bool _temperatureReading;
    float _temperatureC;

    OneWire * _oneWire;
    DallasTemperature * _sensors;
    bool _devicesAvailable;
    DeviceAddress _deviceAddress;
};

#endif /*THERMOMETER_H*/