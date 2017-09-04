#ifndef TEMPERATUREREADERCONFIG_H
#define TEMPERATUREREADERCONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct TemperatureReaderConfig {
    TemperatureReaderConfig() :
        id(0),
        port(0),
        name(""),
        isPortValid(false) {}

    TemperatureReaderConfig(const JsonObject & json) :
        id(json["id"]),
        port(json["port"]),
        name(json["name"].as<String>()),
        isPortValid(false) {}

    unsigned int id;
    unsigned int port;
    String name;
    bool isPortValid;

    void convertToJson(JsonObject & json) const {
        json["id"] = id;
        json["port"] = port;
        json["name"] = name;
        json["isValid"] = isPortValid;
    }
};

#endif /*TEMPERATUREREADERCONFIG_H*/