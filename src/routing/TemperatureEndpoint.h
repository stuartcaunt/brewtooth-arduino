#ifndef TEMPERATUREENDPOINT_H
#define TEMPERATUREENDPOINT_H

#include "WebServerEndpoint.h"

class TemperatureEndpoint : public WebServerEndpoint {
public:
    TemperatureEndpoint() : _temperature(0.0) {}
    virtual ~TemperatureEndpoint() {}

    virtual void buildPaths(ESP8266WebServer * server);
    
private:
    void getTemperatureReaders(ESP8266WebServer * server);
    void addTemperatureReader(ESP8266WebServer * server);
        
    void getTemperature(ESP8266WebServer * server);
    float _temperature;
};

#endif /*TEMPERATUREENDPOINT_H*/
