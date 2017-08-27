#include "Configuration.h"
#include "FileHelper.h"
#include <FS.h>
#include <ArduinoJson.h>

Properties Configuration::properties = Properties();

void Configuration::init() {
    Serial.println("Loading properties file");
    
    std::string jsonData;
    FileHelper fileHelper;
    if (fileHelper.load(PROPERTIES_FILE_NAME, jsonData)) {
        char * jsonBuffer = new char[jsonData.length() + 1];
        strcpy(jsonBuffer, jsonData.c_str());
        
        deserialize(jsonBuffer);

        delete [] jsonBuffer;

    } else {
        Serial.println("Could not read properties file");
    }
}

void Configuration::save() {
    Serial.println("Saving properties file");
    
    StaticJsonBuffer<PROPERTIES_JSON_SIZE> jsonBuffer;
    
    JsonObject & root = jsonBuffer.createObject();
    JsonObject & wifiData = root.createNestedObject("wifiData");
    wifiData["isConfigured"] = properties.wifiData.isConfigured;
    wifiData["ssid"] = properties.wifiData.ssid;
    wifiData["password"] = properties.wifiData.password;
    
    File propertiesFile = SPIFFS.open(PROPERTIES_FILE_NAME, "w");
    root.printTo(propertiesFile);
    propertiesFile.close(); 
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