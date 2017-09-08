#ifndef MASHCONTROLLERCONFIG_H
#define MASHCONTROLLERCONFIG_H

#include "ThermometerConfig.h"
#include "Jsonable.h"
#include <vector>

struct MashControllerConfig : public Jsonable {
    MashControllerConfig() :
        id(0),
        name("") {}

    MashControllerConfig(const JsonObject & json) :
        id(json["id"]),
        name(json["name"].as<String>()) {

        JsonArray & thermometerIds = json["thermometerIds"];
        for (JsonArray::iterator it = thermometerIds.begin(); it != thermometerIds.end(); ++it) {
            unsigned int readerId = *it;
            this->thermometerIds.push_back(readerId);
        }
    }

    virtual void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["name"] = name;
        JsonArray & thermometerIds = json.createNestedArray("thermometerIds");
        for (std::vector<unsigned int>::const_iterator it = this->thermometerIds.begin(); it != this->thermometerIds.end(); it++) {
            unsigned int thermometerId = *it;
            thermometerIds.add(thermometerId);
        }
    }

    unsigned int id;
    String name;
    std::vector<unsigned int> thermometerIds;
};

#endif /*MASHCONTROLLERCONFIG_H*/