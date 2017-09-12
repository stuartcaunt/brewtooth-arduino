#ifndef MASHCONTROLLER_H
#define MASHCONTROLLER_H

#include "MashControllerConfig.h"
#include "ThermometerWireData.h"

class ThermometerWire;
class Relay;

class MashController : public Jsonable {

public:
    MashController(const MashControllerConfig & config) :
        _config(config),
        _heater(NULL),
        _agitator(NULL) {}
    virtual ~MashController() {
        this->deleteHeater();
        this->deleteAgitator();
    }

    const MashControllerConfig * getConfig() const {
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

    void addThermometer(ThermometerWire * thermometerWire) {
        _thermometerWires.push_back(thermometerWire);
    }

    const std::vector<ThermometerWire *> & getThermometers() const {
        return _thermometerWires;
    }

    void clearThermometers() {
        _thermometerWires.clear();
    }
    
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }

    Relay * getHeater() const {
        return _heater;
    }
    void setHeater(Relay * heater);
    void deleteHeater();

    Relay * getAgitator() const {
        return _agitator;
    }
    void setAgitator(Relay * agitator);
    void deleteAgitator();

    float getMeanTemperatureC() const;

    ThermometerWireData getThermometerData() const;

private:
    MashControllerConfig _config;
    std::vector<ThermometerWire *> _thermometerWires;
    Relay * _heater;
    Relay * _agitator;
};

#endif /*MASHCONTROLLER_H*/