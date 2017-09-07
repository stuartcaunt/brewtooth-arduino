#include "Configuration.h"
#include "FileHelper.h"
#include "Log.h"
#include <FS.h>
#include <ArduinoJson.h>

Properties Configuration::properties = Properties();

void Configuration::init(bool reset) {
    
    if (reset) {
        LOG("Resetting configuration");
        makeDefaultConfiguration();

    } else {
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

}

void Configuration::save() {
    LOG("Saving properties file");
    
    DynamicJsonBuffer jsonBuffer;
    
    // root object
    JsonObject & root = jsonBuffer.createObject();

    // Array of temperature configs
    JsonArray & thermometerConfigs = root.createNestedArray("thermometers");
    
    // iteratuer over temperature readers
    for (std::vector<ThermometerConfig>::iterator it = properties.thermometers.begin(); it != properties.thermometers.end(); it++) {
        ThermometerConfig thermometerConfig = *it;

        LOG("Saving temperature reader \"%s\", id = %d, port = %d ", thermometerConfig.name.c_str(), thermometerConfig.id, thermometerConfig.port);
        
        thermometerConfig.convertToJson(thermometerConfigs.createNestedObject());
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
        JsonArray & thermometerIds = root.createNestedArray("thermometerIds");
        for (std::vector<unsigned int>::iterator itTempId = mashControllerConfig.thermometerIds.begin(); itTempId != mashControllerConfig.thermometerIds.end(); itTempId++) {
            unsigned int thermometerId = *itTempId;

            // Add temperature reader Id to array
            thermometerIds.add(thermometerId);
        }
    }
    
    File propertiesFile = SPIFFS.open(PROPERTIES_FILE_NAME, "w");
    root.printTo(propertiesFile);
    propertiesFile.close(); 
}

void Configuration::makeDefaultConfiguration() {
    LOG("Making defaut configuration");
    properties = Properties();

    save();
}
    
void Configuration::deserialize(char * json) {
    DynamicJsonBuffer jsonBuffer;
    
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
        ERROR("Failed to read JSON data");

    } else {
        // Set first use to false
        properties.isFirstUse = false;

        // Deserialize temperature readers
        if (root.containsKey("thermometers")) {
            JsonArray & thermometerConfigs = root["thermometers"];

            for (JsonArray::iterator it = thermometerConfigs.begin(); it != thermometerConfigs.end(); ++it) {
                // Convert to json object
                JsonObject & thermometerConfigJson = *it;

                // Create new ThermometerConfig
                ThermometerConfig thermometerConfig(thermometerConfigJson);
                
                // Add to vector of temperature readers
                properties.thermometers.push_back(thermometerConfig);

                LOG("Read temperature reader config \"%s\", id = %d, port = %d", thermometerConfig.name.c_str(), thermometerConfig.id, thermometerConfig.port);
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
                JsonArray & readerIds = mashControllerConfigJson["thermometerIds"];
                for (JsonArray::iterator itReader = mashControllers.begin(); itReader != mashControllers.end(); ++itReader) {
                    unsigned int readerId = *it;
                    mashControllerConfig.thermometerIds.push_back(readerId);
                }

                // Add to vector of temperature readers
                properties.mashControllers.push_back(mashControllerConfig);

                LOG("Read mash controller config \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
            }
        }
    }
}