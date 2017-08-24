#include "Properties.h"
#include "FileHelper.h"
#include <FS.h>
#include <ArduinoJson.h>

Properties * Properties::_instance = 0;

Properties * Properties::getInstance() {
    if (_instance == 0) {
        _instance = new Properties();
    }
    return _instance;
}

void Properties::init() {

    std::string jsonData;
    FileHelper fileHelper;
    if (fileHelper.load("application.properties", jsonData)) {
        


    } else {
        Serial.println("Could not read properties file");
    }

}
