#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>
#include <vector>
#include <model/TemperatureReaderConfig.h>
#include <model/MashControllerConfig.h>

struct Properties {
    Properties() :
        isFirstUse(true) {}
        
    bool isFirstUse;
    std::vector<MashControllerConfig> mashControllers;
    std::vector<TemperatureReaderConfig> temperatureReaders;
};

// https://bblanchon.github.io/ArduinoJson/assistant/
// {
//     "temperatureReaders": [
//         {
//             "port": 0,
//             "id": "XXXXXXXXXXXXXXXXXXXX"
//         },
//         {
//             "port": 1,
//             "id": "XXXXXXXXXXXXXXXXXXXX"
//         }
//     ]
// }
#define PROPERTIES_JSON_SIZE 256
#define PROPERTIES_FILE_NAME "application.properties"


class Configuration {
public:
    static void init();
    static void save();
    static void reset();
    
    static Properties properties;

private:
    static void makeDefaultConfiguration();
    static void deserialize(char * jsonBuffer);

};

#endif /*CONFIGURATION_H*/