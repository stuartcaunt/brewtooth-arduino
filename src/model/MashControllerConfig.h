#ifndef MASHCONTROLLERCONFIG_H
#define MASHCONTROLLERCONFIG_H

#include "RelayConfig.h"
#include "Jsonable.h"
#include "PIDParams.h"
#include <vector>

struct MashControllerConfig : public Jsonable {
    MashControllerConfig() :
        id(0),
        name(""),
        autoControl(true),
        windowSizeMs(5000) {}

    MashControllerConfig(const JsonObject & json) :
        id(json["id"]),
        name(json["name"].as<String>()),
        autoControl(json["autoControl"]),
        windowSizeMs(json["windowSizeMs"]) {

        // Thermometers
        JsonArray & thermometerIds = json["thermometerIds"];
        for (JsonArray::iterator it = thermometerIds.begin(); it != thermometerIds.end(); ++it) {
            unsigned int readerId = *it;
            this->thermometerIds.push_back(readerId);
        }

        // PID Params
        JsonObject & pidJson = json["pidParams"];
        this->pidParams = PIDParams(pidJson);
        
        // Heater
        JsonObject & heaterJson = json["heater"];
        this->heater = RelayConfig(heaterJson);

        // Agitator
        JsonObject & agitatorJson = json["agitator"];
        this->agitator = RelayConfig(agitatorJson);
    }

    void copyBasic(const MashControllerConfig & config) {
        name = config.name;
        autoControl = config.autoControl;
        windowSizeMs = config.windowSizeMs;
        pidParams = PIDParams(config.pidParams);
    }

    virtual void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["name"] = name;
        json["autoControl"] = autoControl;
        json["windowSizeMs"] = windowSizeMs;

        // Thermometers
        JsonArray & thermometerIds = json.createNestedArray("thermometerIds");
        for (std::vector<unsigned int>::const_iterator it = this->thermometerIds.begin(); it != this->thermometerIds.end(); it++) {
            unsigned int thermometerId = *it;
            thermometerIds.add(thermometerId);
        }

        // PID
        JsonObject & pidJson = json.createNestedObject("pidParams");
        pidParams.convertToJson(pidJson);

        // Heater
        JsonObject & heaterJson = json.createNestedObject("heater");
        heater.convertToJson(heaterJson);

        // Agitator
        JsonObject & agitatorJson = json.createNestedObject("agitator");
        agitator.convertToJson(agitatorJson);
    }

    unsigned int id;
    String name;
    bool autoControl;
    int windowSizeMs;
    PIDParams pidParams;
    std::vector<unsigned int> thermometerIds;
    RelayConfig heater;
    RelayConfig agitator;
};

#endif /*MASHCONTROLLERCONFIG_H*/