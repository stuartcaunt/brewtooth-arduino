#ifndef TEMPERATUREENDPOINT_H
#define TEMPERATUREENDPOINT_H

#include "WebServerEndpoint.h"

class TemperatureEndpoint : public WebServerEndpoint {
public:
    TemperatureEndpoint() : _temperature(0.0) {}
    virtual ~TemperatureEndpoint() {}

    virtual void buildPaths(BrewtoothWebServer * server);
    
private:
    void getTemperatureReaders(ESP8266WebServer * server);
    void getTemperatureReader(ESP8266WebServer * server, int id);
    void addTemperatureReader(ESP8266WebServer * server);
        
    void getTemperature(ESP8266WebServer * server);
    float _temperature;
};

#endif /*TEMPERATUREENDPOINT_H*/
