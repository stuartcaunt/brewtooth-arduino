#ifndef STATEHISTORY_H
#define STATEHISTORY_H

#include "Jsonable.h"

struct StateHistory : public Jsonable {
    StateHistory() {}
    StateHistory(const StateHistory & obj) {}
    StateHistory(const JsonObject & json) {}

    StateHistory & operator=(const StateHistory & rhs) {}

    virtual void convertToJson(JsonObject & json) const {}

    float timeS;
    float temperatureC;
    float setpointC;
    float controllerOutputPercent;
    bool heaterActive;
    bool agitatorActive;
};

#endif /*STATEHISTORY_H*/