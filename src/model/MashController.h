#ifndef MASHCONTROLLER_H
#define MASHCONTROLLER_H

#include "MashControllerConfig.h"
#include "ThermometerWireData.h"

class ThermometerWire;
class Relay;
class PID;

class MashController : public Jsonable {

public:
    MashController(const MashControllerConfig & config) :
        _config(config),
        _heater(NULL),
        _agitator(NULL),
        _temperatureController(NULL),
        _meanTemperatureC(0.0),
        _setpointC(0.0),
        _output(0.0) {
    }

    virtual ~MashController() {
        this->deleteHeater();
        this->deleteAgitator();

        if (_temperatureController != NULL) {
            delete _temperatureController;
            _temperatureController = NULL;
        }
    }

    MashController(const MashControllerConfig & config);
    virtual ~MashController();

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

    float getMeanTemperatureC() const {
        return _meanTemperatureC;
    }

    ThermometerWireData getThermometerData() const;

    void autoTune();
    void setTunings(float kp, float ki, float kd);
    
    void setSetpointC(float setpointC) {
        _setpointC = setpointC;
    }
    
    float getSetpointC() const {
        return _setpointC;
    }

    float getKp() const {
        return _config.kp;
    }
    
    float getKi() const {
        return _config.ki;
    }
    
    float getKd() const {
        return _config.kd;
    }

    void startTemperatureControl();
    void stopTemperatureControl();
    void setAutoTemperatureControl(bool isAuto);
    void update();
        
private:
    MashControllerConfig _config;
    std::vector<ThermometerWire *> _thermometerWires;
    Relay * _heater;
    Relay * _agitator;

    PID * _temperatureController;

    float _meanTemperatureC;
    float _setpointC;
    float _output;
};

#endif /*MASHCONTROLLER_H*/