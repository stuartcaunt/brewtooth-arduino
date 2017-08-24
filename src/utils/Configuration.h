#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

struct WifiData {
    WifiData() :
        isConfigured(false),
        ssid(""),
        password("") {}

    bool isConfigured;
    String ssid;
    String password;
};

struct Properties {
    WifiData wifiData;
};

// https://bblanchon.github.io/ArduinoJson/assistant/
// {
//     "wifiData": {
//       "isConfigured": false,
//       "password": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
//       "ssid": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
//     }
// }
#define PROPERTIES_JSON_SIZE 256
#define PROPERTIES_FILE_NAME "application.properties"


class Configuration {
public:
    static void init();
    static void save();
    
    static Properties properties;

private:
    static void deserialize(char * jsonBuffer);

};

#endif /*CONFIGURATION_H*/