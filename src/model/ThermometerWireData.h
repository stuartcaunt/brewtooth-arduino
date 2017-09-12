#ifndef THERMOMETERWIREDATA_H
#define THERMOMETERWIREDATA_H

#include "Jsonable.h"
#include "Thermometer.h"
#include <vector>

class ThermometerWireData : public Jsonable {
public:
    ThermometerWireData() :
        meanTemperatureC(0.0) {}

    virtual void convertToJson(JsonObject & json) const {
        json["meanTemperatureC"] = meanTemperatureC;

        JsonArray & array = json.createNestedArray("thermometers");
        for (std::vector<Thermometer>::const_iterator it = thermometers.begin(); it != thermometers.end(); it++) {
            const Thermometer & thermometer = *it;
            JsonObject & thermometerJson = array.createNestedObject();
            
            thermometer.convertToJson(thermometerJson);
        }
    }
    
public:
    float meanTemperatureC;
    std::vector<Thermometer> thermometers;
};

#endif /*THERMOMETERWIREDATA_H*/