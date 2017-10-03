#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <vector>
#include <model/ThermometerWireConfig.h>
#include <model/MashControllerConfig.h>

struct Properties {
    Properties() :
        isFirstUse(true) {}
        
    bool isFirstUse;
    std::vector<ThermometerWireConfig> thermometers;
    std::vector<MashControllerConfig> mashControllers;
};

#define PROPERTIES_FILE_NAME "application.properties"


class Configuration {
public:
    static void init(bool reset = false);
    static void save();
    static String getPropertiesJsonString();
    
    static Properties properties;

private:
    static void convertPropertiesToJson(JsonObject & json);
    static void makeDefaultConfiguration();
    static void deserialize(char * jsonBuffer);

};

#endif /*CONFIGURATION_H*/