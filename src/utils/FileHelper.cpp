#include "FileHelper.h"
#include <FS.h>

bool FileHelper::load(const String & filename, std::string & data) {
    data = "";
    
    File file = SPIFFS.open(filename, "r");
    if (!file){
        Serial.println("File " + filename + " does not exist");
        data = "";
        return false;
    }

    size_t size = file.size();
    if (size > 0) {
        Serial.println("Reading file " + filename + ", containing " + size + " bytes");
        std::unique_ptr<char[]> buf (new char[size]);
        file.readBytes(buf.get(), size);
        
        // copy data into string
        data = std::string(buf.get());
    } else {
        Serial.println("File " + filename + " is empty");        
    }
    file.close();

    return (size > 0);
}




