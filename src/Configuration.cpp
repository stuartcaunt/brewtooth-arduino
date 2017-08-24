#include "Configuration.h"
#include "FileHelper.h"
#include <FS.h>
#include <ArduinoJson.h>

Configuration * Configuration::_instance = 0;

Configuration * Configuration::getInstance() {
    if (_instance == 0) {
        _instance = new Configuration();
    }
    return _instance;
}

void Configuration::init() {

    std::string jsonData;
    FileHelper fileHelper;
    if (fileHelper.load("application.properties", jsonData)) {
        char * jsonBuffer = new char[jsonData.length() + 1];
        strcpy(jsonBuffer, jsonData.c_str());
        
        this->deserialize(jsonBuffer);

        delete [] jsonBuffer;

    } else {
        Serial.println("Could not read properties file");
    }
}

void Configuration::deserialize(char * json) {
    StaticJsonBuffer<PROPERTIES_JSON_SIZE> jsonBuffer;
    
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
        Serial.println("Failed to read JSON data");

    } else {
        // Deserialize wifiData
        if (root.containsKey("wifiData")) {
            JsonObject & wifiData = root["wifiData"];
            properties.wifiData.isConfigured = wifiData["isConfigured"];
            properties.wifiData.ssid = wifiData["ssid"].as<String>();
            properties.wifiData.password = wifiData["password"].as<String>();
        }        
    }

        
}