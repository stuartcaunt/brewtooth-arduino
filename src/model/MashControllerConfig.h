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
        windowSizeMs(20000) {}

    MashControllerConfig(const MashControllerConfig & obj) :
        id(obj.id),
        name(obj.name),
        windowSizeMs(obj.windowSizeMs),
        pidParams(obj.pidParams),
        heater(obj.heater),
        agitator(obj.agitator) {
            for (std::vector<unsigned int>::const_iterator it = obj.thermometerIds.begin(); it != obj.thermometerIds.end(); it++) {
                thermometerIds.push_back(*it);
            }
        }

    MashControllerConfig(const JsonObject & json) :
        id(json["id"]),
        name(json["name"].as<String>()),
        windowSizeMs(json["windowSizeMs"]) {

        // Thermometers
        JsonArray & thermometerIdsJson = json["thermometerIds"];
        for (JsonArray::iterator it = thermometerIdsJson.begin(); it != thermometerIdsJson.end(); ++it) {
            unsigned int readerId = *it;
            thermometerIds.push_back(readerId);
        }
        
        // PID Params
        JsonObject & pidJson = json["pidParams"];
        pidParams.copyFromJson(pidJson);
        
        // Heater
        JsonObject & heaterJson = json["heater"];
        agitator = RelayConfig(heaterJson);
        
        // Agitator
        JsonObject & agitatorJson = json["agitator"];
        agitator = RelayConfig(agitatorJson);
    }

    MashControllerConfig & operator=(const MashControllerConfig & rhs) {
        id = rhs.id;
        name = rhs.name;
        windowSizeMs = rhs.windowSizeMs;

        thermometerIds.clear();
        for (std::vector<unsigned int>::const_iterator it = rhs.thermometerIds.begin(); it != rhs.thermometerIds.end(); it++) {
            thermometerIds.push_back(*it);
        }

        pidParams = rhs.pidParams;
        heater = rhs.heater;
        agitator = rhs.agitator;
        return *this;
    }

    void copyBasic(const MashControllerConfig & config) {
        name = config.name;
        windowSizeMs = config.windowSizeMs;
        pidParams = PIDParams(config.pidParams);
    }

    virtual void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["name"] = name;
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
    int windowSizeMs;
    PIDParams pidParams;
    std::vector<unsigned int> thermometerIds;
    RelayConfig heater;
    RelayConfig agitator;
};

#endif /*MASHCONTROLLERCONFIG_H*/