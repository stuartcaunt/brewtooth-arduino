#include "FileHelper.h"
#include "Log.h"
#include <FS.h>

bool FileHelper::load(const String & filename, std::string & data) {
    data = "";
    
    File file = SPIFFS.open(filename, "r");
    if (!file){
        WARN("File \"%s\" does not exist", filename.c_str());
        data = "";
        return false;
    }

    size_t size = file.size();
    if (size > 0) {
        LOG("Reading file \"%s\" containing %d bytes", filename.c_str(), size);
        std::unique_ptr<char[]> buf (new char[size]);
        file.readBytes(buf.get(), size);
        
        // copy data into string
        data = std::string(buf.get());
    } else {
        WARN("File \"%s\" is empty ", filename.c_str());        
    }
    file.close();

    return (size > 0);
}




