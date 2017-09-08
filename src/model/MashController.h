#ifndef MASHCONTROLLER_H
#define MASHCONTROLLER_H

#include "MashControllerConfig.h"
class Thermometer;

class MashController : public Jsonable {

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

    void addThermometer(Thermometer * thermometer) {
        _thermometers.push_back(thermometer);
    }

    const std::vector<Thermometer *> & getThermometers() const {
        return _thermometers;
    }

    void clearThermometers() {
        _thermometers.clear();
    }
    
    virtual void convertToJson(JsonObject & json) const {
        _config.convertToJson(json);
    }

private:
    MashControllerConfig _config;
    std::vector<Thermometer *> _thermometers;
};

#endif /*MASHCONTROLLER_H*/