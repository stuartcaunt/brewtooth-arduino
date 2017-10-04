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
    Configuration() :
        _properties(new Properties()) {
    }
    virtual ~Configuration() {
        delete _properties;
        _properties = NULL;
    }
    static Configuration * _();
    static void initInstance(bool reset = false);
    void save();
    String getPropertiesJsonString();

    Properties * getProperties() const {
        return _properties;
    }

private:
    void init(bool reset = false);
    void convertPropertiesToJson(JsonObject & json);
    void makeDefaultConfiguration();
    void deserialize(char * jsonBuffer);
    
    static Configuration * _instance;
    Properties * _properties;
    
};

#endif /*CONFIGURATION_H*/