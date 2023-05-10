#ifndef STATEHISTORY_H
#define STATEHISTORY_H

#include "Jsonable.h"
#include "TemperatureControlState.h"

struct StateHistory : public Jsonable {
    StateHistory() :
        timeS(0.0),
        temperatureC(0.0),
        setpointC(0.0),
        controllerOutputPercent(0.0),
        heaterActive(false),
        agitatorActive(false) {}

    StateHistory(const TemperatureControlState & state) :
        timeS(state.currentTimeS),
        temperatureC(state.temperatureC),
        setpointC(state.setpointC),
        controllerOutputPercent(100.0 * state.controllerOutput / state.outputMax),
        heaterActive(state.heaterActive),
        agitatorActive(state.agitatorActive) {}

    StateHistory(const StateHistory & obj) :
        timeS(obj.timeS),
        temperatureC(obj.temperatureC),
        setpointC(obj.setpointC),
        controllerOutputPercent(obj.controllerOutputPercent),
        heaterActive(obj.heaterActive),
        agitatorActive(obj.agitatorActive) {}
    
    StateHistory & operator=(const StateHistory & rhs) {
        timeS = rhs.timeS;
        temperatureC = rhs.temperatureC;
        setpointC = rhs.setpointC;
        controllerOutputPercent = rhs.controllerOutputPercent;
        heaterActive = rhs.heaterActive;
        agitatorActive = rhs.agitatorActive;
        return *this;
    }

    virtual void convertToJson(JsonObject & json) const {
        json["timeS"] = timeS;
        json["temperatureC"] = temperatureC;
        json["setpointC"] = setpointC;
        json["controllerOutputPercent"] = controllerOutputPercent;
        json["heaterActive"] = heaterActive;
        json["agitatorActive"] = agitatorActive;
    }

    float timeS;
    float temperatureC;
    float setpointC;
    float controllerOutputPercent;
    bool heaterActive;
    bool agitatorActive;
};

#endif /*STATEHISTORY_H*/