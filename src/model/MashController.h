#ifndef MASHCONTROLLER_H
#define MASHCONTROLLER_H

#include "MashControllerConfig.h"
class TemperatureReader;

class MashController {

public:
    MashController(const MashControllerConfig & config) :
        _config(config) {}
    virtual ~MashController() {}

    const MashControllerConfig * getConfig() const {
        return &_config;
    }

    const unsigned int getId() const {
        return _config.id;
    }

    unsigned int setId(unsigned int id) {
        _config.id = id;
    }

    const String & getName() const {
        return _config.name;
    }

    void setName(String name) {
        _config.name = name;
    }

private:
    MashControllerConfig _config;
    std::vector<TemperatureReader *> _temperatureReaders;
};

#endif /*MASHCONTROLLER_H*/