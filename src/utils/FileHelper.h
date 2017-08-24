#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <Arduino.h>
#include <string>

class FileHelper {
public:
    FileHelper() {}
    virtual ~FileHelper() {}

    bool load(const String & filename, std::string & data);

private:
};

#endif /*FILEHELPER_H*/