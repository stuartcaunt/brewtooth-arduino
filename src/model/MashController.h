#ifndef MASHCONTROLLER_H
#define MASHCONTROLLER_H

#include "MashControllerConfig.h"
#include "ThermometerWireData.h"
#include "TemperatureControlState.h"

class ThermometerWire;
class Relay;
class PID;
class PID_ATune;

class MashController : public Jsonable {

public:
    MashController(const MashControllerConfig & config);
    virtual ~MashController();

    void copyBasic(const MashControllerConfig & config) {
        _config.copyBasic(config);
        _state.windowSizeMs = _config.windowSizeMs;
        _state.sampleTimeMs = 0.25 * _config.windowSizeMs;
        _state.kp = _config.pidParams.kp;
        _state.ki = _config.pidParams.ki;
        _state.kd = _config.pidParams.kd;
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
        return _config.pidParams.kp;
    }

    double getKi() const {
        return _config.pidParams.ki;
    }

    double getKd() const {
        return _config.pidParams.kd;
    }

    void setOutputMax(int outputMax);

    int getOutputMax() const {
        return _config.pidParams.outputMax;
    }
    
    const PIDParams & getPIDParams() const {
        return _config.pidParams;
    }

    void setPIDParams(const PIDParams & pidParams);

    void setAutoTemperatureControl(bool isAuto);
    
        bool isAutoTemperatureControl() const {
            return _config.autoControl;
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
    float getMeanTemperatureC() const;

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

    void startAutoTune();
    void stopAutoTune();

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
    PID_ATune * _autoTune;
    bool _isAutoTuning;

    TemperatureControlState _state;
    unsigned long _lastTimeMs;
    unsigned long _startTimeMs;
    unsigned long _windowStartTimeMs;
};

#endif /*MASHCONTROLLER_H*/