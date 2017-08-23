#include "TemperatureEndpoint.h"

void TemperatureEndpoint::buildPaths(ESP8266WebServer * server) {
    Serial.println("Building paths for TemperatureEndpoint");
    
    server->on("/temperature", std::bind(&TemperatureEndpoint::getTemperature, this, server));
}

void TemperatureEndpoint::getTemperature(ESP8266WebServer * server) {
    Serial.println("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    server->send(200, "text/plain", message);
}