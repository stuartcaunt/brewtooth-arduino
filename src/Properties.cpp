#include "Properties.h"
#include <FS.h>

Properties * Properties::_instance = 0;

Properties * Properties::getInstance() {
    if (_instance == 0) {
        _instance = new Properties();
    }
    return _instance;
}

void Properties::init() {

    File file = SPIFFS.open("app.properties", "r");
    if (!file){
        Serial.println("Cannot open file");

        // Create new file
        Serial.println("Write new file");
        File file = SPIFFS.open("app.properties", "w");
        file.println("test");
        file.close();
        
    
    } else {
        size_t size = file.size();
        if (size == 0) {
            Serial.println("Properties file is empty");

        } else {
            Serial.println("Reading file");
            std::unique_ptr<char[]> buf (new char[size]);
            file.readBytes(buf.get(), size);

            Serial.println("Read file");

      }
      file.close();
    }
}
