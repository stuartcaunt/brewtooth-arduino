#ifndef JSONABLE_H
#define JSONABLE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Jsonable {
public:
    Jsonable() {}
    virtual ~Jsonable() {}

    virtual void convertToJson(JsonObject & json) const = 0;
};

#endif /*JSONABLE_H*/