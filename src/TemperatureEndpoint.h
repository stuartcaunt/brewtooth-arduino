#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "WebServerEndpoint.h"

class TemperatureEndpoint : public WebServerEndpoint {
public:
    TemperatureEndpoint() : _temperature(0.0) {}
    virtual ~TemperatureEndpoint() {}

    virtual void buildPaths(ESP8266WebServer * server);
    
private:
    void getTemperature(ESP8266WebServer * server);
    float _temperature;
};

#endif /*TEMPERATURE_H*/
