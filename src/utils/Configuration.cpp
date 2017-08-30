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

        Serial.println(String("Saving temperature reader config: id = ") + temperatureReaderConfig.id + String(", port = ") + temperatureReaderConfig.port);
        
        JsonObject & temperatureConfigJson = temperatureConfigs.createNestedObject();
        temperatureConfigJson["port"] = temperatureReaderConfig.port;
        temperatureConfigJson["id"] = temperatureReaderConfig.id;
        temperatureConfigJson["name"] = temperatureReaderConfig.name;
    }
    
    File propertiesFile = SPIFFS.open(PROPERTIES_FILE_NAME, "w");
    root.printTo(propertiesFile);
    propertiesFile.close(); 
}


void Configuration::reset() {
    Serial.println("Reset configuration");
    makeDefaultConfiguration();
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
        // Set first use to false
        properties.isFirstUse = false;

        // Deserialize wifiData
        if (root.containsKey("temperatureReaders")) {
            JsonArray & temperatureConfigs = root["temperatureReaders"];

            for (JsonArray::iterator it = temperatureConfigs.begin(); it != temperatureConfigs.end(); ++it) {
                // Convert to json object
                JsonObject & temperatureConfigJson = *it;

                // Create new TemperatureReaderConfig
                TemperatureReaderConfig temperatureReaderConfig;
                temperatureReaderConfig.id = temperatureConfigJson["id"];
                temperatureReaderConfig.port = temperatureConfigJson["port"];
                temperatureReaderConfig.name = temperatureConfigJson["name"].as<String>();
                
                // Add to vector of temperature readers
                properties.temperatureReaders.push_back(temperatureReaderConfig);

                Serial.println(String("Read temperature reader config: id = ") + temperatureReaderConfig.id + String(", port = ") + temperatureReaderConfig.port);
            }
        }
    }
}