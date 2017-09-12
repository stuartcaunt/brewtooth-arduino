#ifndef THERMOMETER_H
#define THERMOMETER_H

#include "Jsonable.h"

typedef uint8_t DeviceAddress[8];

class Thermometer : public Jsonable {
public:
    DeviceAddress deviceAddress;
    float temperatureC;

    virtual void convertToJson(JsonObject & json) const {
        json["temperatureC"] = temperatureC;

        char address[18];
        sprintf(address, "0x%02X%02X%02X%02X%02X%02X%02X%02X", deviceAddress[0], deviceAddress[1], deviceAddress[2], deviceAddress[3],
        deviceAddress[4], deviceAddress[5], deviceAddress[6], deviceAddress[7]);
        json["address"] = String(address);
    }
};

#endif /*THERMOMETER_H*/