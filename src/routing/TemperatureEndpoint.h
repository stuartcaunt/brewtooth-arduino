#ifndef TEMPERATUREENDPOINT_H
#define TEMPERATUREENDPOINT_H

#include "WebServerEndpoint.h"

class TemperatureEndpoint : public WebServerEndpoint {
public:
    TemperatureEndpoint() : _temperature(0.0) {}
    virtual ~TemperatureEndpoint() {}

    virtual void buildPaths();
    
private:
    void getTemperatureReaders();
    void getTemperatureReader(int id);
    void addTemperatureReader();
        
    void getTemperature();
    float _temperature;
};

#endif /*TEMPERATUREENDPOINT_H*/
