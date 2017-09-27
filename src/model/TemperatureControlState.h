#ifndef TEMPERATURECONTROLSTATE_H
#define TEMPERATURECONTROLSTATE_H

#include "Jsonable.h"
#include "TemperatureProfile.h"

typedef enum {
    Setpoint,
    Profile
} ControlType;

inline String toString(ControlType controlType) {
    switch(controlType) {
        case Setpoint: return "Setpoint";
        case Profile: return "Profile";
    }
}

class TemperatureControlState : public Jsonable {
public:
    TemperatureControlState() :
        running(false),
        autoTuning(false),
        runTimeMs(0.0),
        temperatureC(0.0),
        controlType(ControlType::Setpoint),
        setpointC(0.0),
        controllerOutput(0.0),
        heaterActive(false),
        agitatorActive(false),
        autoControl(false),
        kp(0.0),
        ki(0.0),
        kd(0.0),
        loopMs(0.0),
        sampleTimeMs(0.0),
        outputMax(0.0),
        windowSizeMs(0) {}

    void setControlType(ControlType controlType) {
        this->controlType = controlType;
    }

    void start(unsigned int timeMs, float temperatureC) {
        if (controlType == ControlType::Profile) {
            this->setpointC = temperatureProfile.start(timeMs, temperatureC);
        }
    }

    void stop() {
        if (controlType == ControlType::Profile) {
            temperatureProfile.stop();
        }
    }

    void update(unsigned int timeMs, float temperatureC) {
        if (controlType == ControlType::Profile) {
            this->setpointC = temperatureProfile.update(timeMs, temperatureC);
        }
    }

    virtual void convertToJson(JsonObject & json) const {
        json["running"] = running;
        json["autoTuning"] = autoTuning;
        json["runTimeMs"] = runTimeMs;
        json["temperatureC"] = temperatureC;
        json["controlType"] = toString(controlType);
        if (controlType == ControlType::Setpoint) {
            json["setpointC"] = setpointC;
        } else {
            JsonObject & profileJson = json.createNestedObject("temperatureProfile");
            temperatureProfile.convertToJson(profileJson);
        }
        json["controllerOutput"] = controllerOutput;
        json["heaterActive"] = heaterActive;
        json["agitatorActive"] = agitatorActive;
        json["autoControl"] = autoControl;
        json["kp"] = kp;
        json["ki"] = ki;
        json["kd"] = kd;
        json["loopMs"] = loopMs;
        json["sampleTimeMs"] = sampleTimeMs;
        json["outputMax"] = outputMax;
        json["windowSizeMs"] = windowSizeMs;
    }
    
public:
    bool running;
    bool autoTuning;
    unsigned long runTimeMs;
    float temperatureC;
    ControlType controlType;
    float setpointC;
    TemperatureProfile temperatureProfile;
    float controllerOutput;
    bool heaterActive;
    bool agitatorActive;
    bool autoControl;
    float kp;
    float ki;
    float kd;
    unsigned long loopMs;
    unsigned long sampleTimeMs;
    unsigned int outputMax;
    unsigned int windowSizeMs;
};

#endif /*TEMPERATURECONTROLSTATE_H*/