#ifndef MASHCONTROLLERCONFIG_H
#define MASHCONTROLLERCONFIG_H

#include "RelayConfig.h"
#include "Jsonable.h"
#include <vector>

struct MashControllerConfig : public Jsonable {
    MashControllerConfig() :
        id(0),
        name(""),
        kp(1.0),
        ki(1.0),
        kd(1.0),
        autoControl(true) {}

    MashControllerConfig(const JsonObject & json) :
        id(json["id"]),
        name(json["name"].as<String>()),
        kp(json["kp"]),
        ki(json["ki"]),
        kd(json["kd"]),
        autoControl(json["autoControl"]) {

        // Thermometers
        JsonArray & thermometerIds = json["thermometerIds"];
        for (JsonArray::iterator it = thermometerIds.begin(); it != thermometerIds.end(); ++it) {
            unsigned int readerId = *it;
            this->thermometerIds.push_back(readerId);
        }

        // Heater
        JsonObject & heaterJson = json["heater"];
        this->heater = RelayConfig(heaterJson);

        // Agitator
        JsonObject & agitatorJson = json["agitator"];
        this->agitator = RelayConfig(agitatorJson);
    }

    virtual void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["name"] = name;
        json["kp"] = kp;
        json["ki"] = ki;
        json["kd"] = kd;
        json["autoControl"] = autoControl;

        // Thermometers
        JsonArray & thermometerIds = json.createNestedArray("thermometerIds");
        for (std::vector<unsigned int>::const_iterator it = this->thermometerIds.begin(); it != this->thermometerIds.end(); it++) {
            unsigned int thermometerId = *it;
            thermometerIds.add(thermometerId);
        }

        // Heater
        JsonObject & heaterJson = json.createNestedObject("heater");
        heater.convertToJson(heaterJson);

        // Agitator
        JsonObject & agitatorJson = json.createNestedObject("agitator");
        agitator.convertToJson(agitatorJson);
    }

    unsigned int id;
    String name;
    double kp;
    double ki;
    double kd;
    bool autoControl;
    std::vector<unsigned int> thermometerIds;
    RelayConfig heater;
    RelayConfig agitator;
};

#endif /*MASHCONTROLLERCONFIG_H*/