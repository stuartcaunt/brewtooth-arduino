#include "Configuration.h"
#include "FileHelper.h"
#include "Log.h"
#include <FS.h>
#include <ArduinoJson.h>

Properties Configuration::properties = Properties();

void Configuration::init() {
    LOG("Loading properties file");
    
    std::string jsonData;
    FileHelper fileHelper;
    if (fileHelper.load(PROPERTIES_FILE_NAME, jsonData)) {
        char * jsonBuffer = new char[jsonData.length() + 1];
        strcpy(jsonBuffer, jsonData.c_str());
        
        deserialize(jsonBuffer);

        delete [] jsonBuffer;

    } else {
        LOG("Could not read properties file");
        makeDefaultConfiguration();
    }
}

void Configuration::save() {
    LOG("Saving properties file");
    
    StaticJsonBuffer<PROPERTIES_JSON_SIZE> jsonBuffer;
    
    // root object
    JsonObject & root = jsonBuffer.createObject();

    // Array of temperature configs
    JsonArray & temperatureConfigs = root.createNestedArray("temperatureReaders");
    
    // iteratuer over temperature readers
    for (std::vector<TemperatureReaderConfig>::iterator it = properties.temperatureReaders.begin(); it != properties.temperatureReaders.end(); it++) {
        TemperatureReaderConfig temperatureReaderConfig = *it;

        LOG("Saving temperature reader \"%s\", id = %d, port = %d ", temperatureReaderConfig.name.c_str(), temperatureReaderConfig.id, temperatureReaderConfig.port);
        
        JsonObject & temperatureConfigJson = temperatureConfigs.createNestedObject();
        temperatureConfigJson["port"] = temperatureReaderConfig.port;
        temperatureConfigJson["id"] = temperatureReaderConfig.id;
        temperatureConfigJson["name"] = temperatureReaderConfig.name;
    }

    // Array of temperature configs
    JsonArray & mashControllerConfigs = root.createNestedArray("mashControllers");
    
    // Iterate over mash controllers
    for (std::vector<MashControllerConfig>::iterator it = properties.mashControllers.begin(); it != properties.mashControllers.end(); it++) {
        MashControllerConfig mashControllerConfig = *it;

        LOG("Saving mash controller \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
        
        JsonObject & mashControllerConfigJson = mashControllerConfigs.createNestedObject();
        mashControllerConfigJson["id"] = mashControllerConfig.id;
        mashControllerConfigJson["name"] = mashControllerConfig.name;

        // Create array for temperatuer reader Ids
        JsonArray & temperatureReaderIds = root.createNestedArray("temperatureReaderIds");
        for (std::vector<unsigned int>::iterator itTempId = mashControllerConfig.temperatureReaderIds.begin(); itTempId != mashControllerConfig.temperatureReaderIds.end(); itTempId++) {
            unsigned int temperatureReaderId = *itTempId;

            // Add temperature reader Id to array
            temperatureReaderIds.add(temperatureReaderId);
        }
    }
    
    File propertiesFile = SPIFFS.open(PROPERTIES_FILE_NAME, "w");
    root.printTo(propertiesFile);
    propertiesFile.close(); 
}


void Configuration::reset() {
    LOG("Reset configuration");
    makeDefaultConfiguration();
}
    

void Configuration::makeDefaultConfiguration() {
    LOG("Making defaut configuration");
    properties = Properties();

    save();
}
    
void Configuration::deserialize(char * json) {
    StaticJsonBuffer<PROPERTIES_JSON_SIZE> jsonBuffer;
    
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
        ERROR("Failed to read JSON data");

    } else {
        // Set first use to false
        properties.isFirstUse = false;

        // Deserialize temperature readers
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

                LOG("Read temperature reader config \"%s\", id = %d, port = %d", temperatureReaderConfig.name.c_str(), temperatureReaderConfig.id, temperatureReaderConfig.port);
            }
        }

        // Deserialize mash controllers
        if (root.containsKey("mashControllers")) {
            JsonArray & mashControllers = root["mashControllers"];

            for (JsonArray::iterator it = mashControllers.begin(); it != mashControllers.end(); ++it) {
                // Convert to json object
                JsonObject & mashControllerConfigJson = *it;

                // Create new MashControllerConfig
                MashControllerConfig mashControllerConfig;
                mashControllerConfig.id = mashControllerConfigJson["id"];
                mashControllerConfig.name = mashControllerConfigJson["name"].as<String>();

                // Get temperature reader ids
                JsonArray & readerIds = mashControllerConfigJson["temperatureReaderIds"];
                for (JsonArray::iterator itReader = mashControllers.begin(); itReader != mashControllers.end(); ++itReader) {
                    unsigned int readerId = *it;
                    mashControllerConfig.temperatureReaderIds.push_back(readerId);
                }

                // Add to vector of temperature readers
                properties.mashControllers.push_back(mashControllerConfig);

                LOG("Read mash controller config \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
            }
        }
    }
}