#ifndef PIDPARAMS_H
#define PIDPARAMS_H

#include "Jsonable.h"

struct PIDParams : public Jsonable {
    PIDParams() :
        kp(1.0),
        ki(1.0),
        kd(1.0),
        windowSizeMs(5000) {}

    PIDParams(const JsonObject & json) :
        kp(json["kp"]),
        ki(json["ki"]),
        kd(json["kd"]),
        windowSizeMs(json["windowSizeMs"]) {
    }

    PIDParams & operator=(const PIDParams & rhs) {
        kp = rhs.kp;
        ki = rhs.ki;
        kd = rhs.kd;
        windowSizeMs = rhs.windowSizeMs;
    }

    virtual void convertToJson(JsonObject & json) const {
        json["kp"] = kp;
        json["ki"] = ki;
        json["kd"] = kd;
        json["windowSizeMs"] = windowSizeMs;
    }

    double kp;
    double ki;
    double kd;
    int windowSizeMs;
};

#endif /*PIDPARAMS_H*/