#ifndef TEMPERATURECONTROLSTATE_H
#define TEMPERATURECONTROLSTATE_H

#include "Jsonable.h"

class TemperatureControlState : public Jsonable {
public:
    TemperatureControlState() :
        running(false),
        autoTuning(false),
        runTimeMs(0.0),
        temperatureC(0.0),
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

    virtual void convertToJson(JsonObject & json) const {
        json["running"] = running;
        json["autoTuning"] = autoTuning;
        json["runTimeMs"] = runTimeMs;
        json["temperatureC"] = temperatureC;
        json["setpointC"] = setpointC;
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
    float setpointC;
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