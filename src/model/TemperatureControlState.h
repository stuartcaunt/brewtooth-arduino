#ifndef TEMPERATURECONTROLSTATE_H
#define TEMPERATURECONTROLSTATE_H

#include "Jsonable.h"

class TemperatureControlState : public Jsonable {
public:
    TemperatureControlState() :
        running(false),
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
        windowSizeMs(0) {}

    virtual void convertToJson(JsonObject & json) const {
        json["running"] = running;
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
        json["windowSizeMs"] = windowSizeMs;
    }
    
public:
    bool running;
    unsigned long runTimeMs;
    double temperatureC;
    double setpointC;
    double controllerOutput;
    bool heaterActive;
    bool agitatorActive;
    bool autoControl;
    double kp;
    double ki;
    double kd;
    unsigned long loopMs;
    unsigned long sampleTimeMs;
    unsigned int windowSizeMs;
};

#endif /*TEMPERATURECONTROLSTATE_H*/