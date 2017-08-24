#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

struct WifiData {
    WifiData() :
        isConfigured(false),
        ssid(""),
        password("") {}

    bool isConfigured;
    std::string ssid;
    std::string password;
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


class Configuration {
public:
    virtual ~Configuration() {}

    static Configuration * getInstance();
    void init();

    Properties properties;
    
private:
    void deserialize(char * jsonBuffer);
        
private:
    Configuration() {}
    static Configuration * _instance;

};

#endif /*CONFIGURATION_H*/