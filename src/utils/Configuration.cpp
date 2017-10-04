#include "Configuration.h"
#include "FileHelper.h"
#include "Log.h"
#include <FS.h>

Configuration * Configuration::_instance = NULL;

void Configuration::initInstance(bool reset) {
    if (_instance == NULL) {
        _instance = new Configuration();
        _instance->init(reset);
    }
}
    
Configuration * Configuration::_() {
    return _instance;
}

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
    
    // root object
    DynamicJsonBuffer jsonBuffer;
    JsonObject & root = jsonBuffer.createObject();

    // Convert properties to json
    convertPropertiesToJson(root);
    
    File propertiesFile = SPIFFS.open(PROPERTIES_FILE_NAME, "w");
    root.printTo(propertiesFile);
    propertiesFile.close(); 
}

String Configuration::getPropertiesJsonString() {
    // root object
    DynamicJsonBuffer jsonBuffer;
    JsonObject & root = jsonBuffer.createObject();

    // Convert properties to json
    convertPropertiesToJson(root);

    String jsonString;
    root.printTo(jsonString);

    return jsonString;
}

void Configuration::convertPropertiesToJson(JsonObject & json) {
    // Array of temperature configs
    JsonArray & thermometerConfigs = json.createNestedArray("thermometers");
    
    // iteratuer over temperature readers
    for (std::vector<ThermometerWireConfig>::iterator it = _properties->thermometers.begin(); it != _properties->thermometers.end(); it++) {
        ThermometerWireConfig thermometerConfig = *it;

        LOG("Saving temperature reader \"%s\", id = %d, port = %d ", thermometerConfig.name.c_str(), thermometerConfig.id, thermometerConfig.port);
        
        thermometerConfig.convertToJson(thermometerConfigs.createNestedObject());
    }

    // Array of temperature configs
    JsonArray & mashControllerConfigs = json.createNestedArray("mashControllers");
    
    // Iterate over mash controllers
    for (std::vector<MashControllerConfig>::iterator it = _properties->mashControllers.begin(); it != _properties->mashControllers.end(); it++) {
        MashControllerConfig mashControllerConfig = *it;

        LOG("Saving mash controller \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
        
        mashControllerConfig.convertToJson(mashControllerConfigs.createNestedObject());
    }
}

void Configuration::makeDefaultConfiguration() {
    LOG("Making defaut configuration");
    if (_properties != NULL) {
        delete _properties;
    }
    _properties = new Properties();

    save();
}
    
void Configuration::deserialize(char * json) {
    DynamicJsonBuffer jsonBuffer;
    
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
        ERROR("Failed to read JSON data");

    } else {
        if (_properties != NULL) {
            delete _properties;
        }
        _properties = new Properties();
        
        // Set first use to false
        _properties->isFirstUse = false;

        root.printTo(Serial);
        

        // Deserialize temperature readers
        if (root.containsKey("thermometers")) {
            JsonArray & thermometerConfigs = root["thermometers"];

            for (JsonArray::iterator it = thermometerConfigs.begin(); it != thermometerConfigs.end(); ++it) {
                // Convert to json object
                JsonObject & thermometerConfigJson = *it;

                // Create new ThermometerWireConfig
                ThermometerWireConfig thermometerConfig(thermometerConfigJson);
                
                // Add to vector of temperature readers
                _properties->thermometers.push_back(thermometerConfig);

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
                MashControllerConfig mashControllerConfig(mashControllerConfigJson);
                
                // Add to vector of temperature readers
                _properties->mashControllers.push_back(mashControllerConfig);
                
                LOG("Read mash controller config \"%s\", id = %d", mashControllerConfig.name.c_str(), mashControllerConfig.id);
            }
        }
    }
}