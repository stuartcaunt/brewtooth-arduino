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
        currentTimeS(0.0),
        runTimeS(0.0),
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

    void start(float timeS, float temperatureC) {
        if (controlType == ControlType::Profile) {
            this->setpointC = temperatureProfile.start(timeS, temperatureC);
        }
    }

    void stop() {
        if (controlType == ControlType::Profile) {
            temperatureProfile.stop();
        }
    }

    void update(float timeS, float temperatureC) {
        if (controlType == ControlType::Profile) {
            this->setpointC = temperatureProfile.update(timeS, temperatureC);
        }
    }

    virtual void convertToJson(JsonObject & json) const {
        json["running"] = running;
        json["autoTuning"] = autoTuning;
        json["currentTimeS"] = currentTimeS;
        json["runTimeS"] = runTimeS;
        json["temperatureC"] = temperatureC;
        json["controlType"] = toString(controlType);
        json["setpointC"] = setpointC;
        if (controlType == ControlType::Profile) {
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
    float currentTimeS;
    float runTimeS;
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