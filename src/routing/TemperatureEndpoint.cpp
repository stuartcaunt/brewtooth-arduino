#include "TemperatureEndpoint.h"
#include <Utils/Log.h>

void TemperatureEndpoint::buildPaths(ESP8266WebServer * server) {
    LOG("Building paths for TemperatureEndpoint");
    
    server->on("/temperature", std::bind(&TemperatureEndpoint::getTemperature, this, server));
}

void TemperatureEndpoint::getTemperature(ESP8266WebServer * server) {
    LOG("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    server->send(200, "text/plain", message);
}