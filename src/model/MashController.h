#ifndef MASHCONTROLLER_H
#define MASHCONTROLLER_H

#include "MashControllerConfig.h"
#include "ThermometerWireData.h"
#include "TemperatureControlState.h"

class ThermometerWire;
class Relay;
class PID;

class MashController : public Jsonable {

public:
    MashController(const MashControllerConfig & config);
    virtual ~MashController();

    void copyBasic(const MashControllerConfig & config) {
        _config.copyBasic(config);
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

    void setTunings(double kp, double ki, double kd);

    double getKp() const {
        return _config.kp;
    }

    double getKi() const {
        return _config.ki;
    }

    double getKd() const {
        return _config.kd;
    }
    
    void setAutoTemperatureControl(bool isAuto);

    bool isAutoTemperatureControl() const {
        return _config.autoControl;
    }

    void setWindowSizeMs(int windowSizeMs) {
        _config.windowSizeMs = _state.windowSizeMs = windowSizeMs;
    }

    int getWindowSizeMs() const {
        return _config.windowSizeMs;
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

    Relay * getHeater() const {
        return _heater;
    }
    void setHeater(Relay * heater);
    void deleteHeater();

    bool setHeaterActive(bool active);
    bool isHeaterActive() const {
        return _state.heaterActive;
    }

    Relay * getAgitator() const {
        return _agitator;
    }
    void setAgitator(Relay * agitator);
    void deleteAgitator();

    bool setAgitatorActive(bool active);
    bool isAgitatorActive() const {
        return _state.agitatorActive;
    }

    float getTemperatureC() const {
        return _state.temperatureC;
    }

    ThermometerWireData getThermometerData() const;

    const TemperatureControlState & getTemperatureControlState() const {
        return _state;
    }

    void setSetpointC(double setpointC) {
        _state.setpointC = setpointC;
    }
    
    double getSetpointC() const {
        return _state.setpointC;
    }

    void startTemperatureControl();
    void stopTemperatureControl();

    void update();
          
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }
  
private:
    MashControllerConfig _config;
    std::vector<ThermometerWire *> _thermometerWires;
    Relay * _heater;
    Relay * _agitator;

    PID * _temperatureController;

    TemperatureControlState _state;
    unsigned long _lastTimeMs;
    unsigned long _windowStartTimeMs;
};

#endif /*MASHCONTROLLER_H*/