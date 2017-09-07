#ifndef THERMOMETERENDPOINT_H
#define THERMOMETERENDPOINT_H

#include "WebServerEndpoint.h"

class ThermometerEndpoint : public WebServerEndpoint {
public:
    ThermometerEndpoint() : _temperature(0.0) {}
    virtual ~ThermometerEndpoint() {}

    virtual void buildPaths();
    
private:
    void addThermometer();
    void getThermometers();

    void getThermometer(int id);
    void deleteThermometer(int id);
    void updateThermometer(int id);
        
    void getTemperature();
    float _temperature;
};

#endif /*THERMOMETERENDPOINT_H*/
