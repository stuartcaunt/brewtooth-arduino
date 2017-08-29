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
        makeDefaultConfiguration();
    }
}

void Configuration::save() {
    Serial.println("Saving properties file");
    
    StaticJsonBuffer<PROPERTIES_JSON_SIZE> jsonBuffer;
    
    // root object
    JsonObject & root = jsonBuffer.createObject();

    // Array of temperature configs
    JsonArray & temperatureConfigs = root.createNestedArray("temperatureReaders");
    
    // iteratuer over temperature readers
    for (std::vector<TemperatureReaderConfig>::iterator it = properties.temperatureReaders.begin(); it != properties.temperatureReaders.end(); it++) {
        TemperatureReaderConfig temperatureReaderConfig = *it;
        JsonObject & temperatureConfigJson = temperatureConfigs.createNestedObject();
        temperatureConfigJson["port"] = temperatureReaderConfig.port;
        temperatureConfigJson["id"] = temperatureReaderConfig.id;
    }
    
    File propertiesFile = SPIFFS.open(PROPERTIES_FILE_NAME, "w");
    root.printTo(propertiesFile);
    propertiesFile.close(); 
}

void Configuration::makeDefaultConfiguration() {
    Serial.println("Making defaut configuration");
    properties = Properties();

    save();
}
    
void Configuration::deserialize(char * json) {
    StaticJsonBuffer<PROPERTIES_JSON_SIZE> jsonBuffer;
    
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
        Serial.println("Failed to read JSON data");

    } else {
        // Deserialize wifiData
        if (root.containsKey("temperatureReaders")) {
            // JsonArray & temperatureConfigs = root["temperatureReaders"];


            // properties.wifiData.isConfigured = wifiData["isConfigured"];
            // properties.wifiData.ssid = wifiData["ssid"].as<String>();
            // properties.wifiData.password = wifiData["password"].as<String>();
        }
    }
}