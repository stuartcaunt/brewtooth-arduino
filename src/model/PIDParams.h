#ifndef PIDPARAMS_H
#define PIDPARAMS_H

#include "Jsonable.h"

struct PIDParams : public Jsonable {
    PIDParams() :
        kp(1.0),
        ki(1.0),
        kd(1.0),
        outputMax(100) {}

    PIDParams(const PIDParams & obj) :
        kp(obj.kp),
        ki(obj.ki),
        kd(obj.kd),
        outputMax(obj.outputMax) {}

    PIDParams(const JsonObject & json) :
        kp(json["kp"]),
        ki(json["ki"]),
        kd(json["kd"]),
        outputMax(json["outputMax"]) {
    }

    PIDParams & operator=(const PIDParams & rhs) {
        kp = rhs.kp;
        ki = rhs.ki;
        kd = rhs.kd;
        outputMax = rhs.outputMax;
    }

    void copyFromJson(const JsonObject & json)  {
        kp = json["kp"];
        ki = json["ki"];
        kd =json["kd"];
        outputMax =json["outputMax"];
    }

    virtual void convertToJson(JsonObject & json) const {
        json["kp"] = kp;
        json["ki"] = ki;
        json["kd"] = kd;
        json["outputMax"] = outputMax;
    }

    float kp;
    float ki;
    float kd;
    int outputMax;
};

#endif /*PIDPARAMS_H*/