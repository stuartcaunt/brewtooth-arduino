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
    std::vector<TemperatureReaderConfig> temperatureReaders;
    std::vector<MashControllerConfig> mashControllers;
};

// https://bblanchon.github.io/ArduinoJson/assistant/
// {
//     "temperatureReaders": [
//         {
//             "id": 0,
//             "port": 6,
//             "name": "XXXXXXXXXXXXXXXXXXXX"
//         },
//         {
//             "id": 1,
//             "port": 1,
//             "name": "XXXXXXXXXXXXXXXXXXXX"
//         }
//     ],
//     "mashControllers": [
//         {
//             "id": 0,
//             "name": "XXXXXXXXXXXXXXXXXXXX",
//             "temperatureReaderIds": [0, 1]
//         }
//     ]
// }
#define PROPERTIES_FILE_NAME "application.properties"


class Configuration {
public:
    static void init(bool reset = false);
    static void save();
    
    static Properties properties;

private:
    static void makeDefaultConfiguration();
    static void deserialize(char * jsonBuffer);

};

#endif /*CONFIGURATION_H*/