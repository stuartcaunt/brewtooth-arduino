#include "RootEndpoint.h"

void RootEndpoint::buildPaths(ESP8266WebServer * server) {
    Serial.println("Building paths for RootEndpoint");
    
    server->on("/", std::bind(&RootEndpoint::getMessage, this, server));
}

void RootEndpoint::getMessage(ESP8266WebServer * server) {
//   digitalWrite(led, 1);
    Serial.println("getMessage");
    server->send(200, "text/plain", "Welcome to the brewtooth ESP8266 server!");
//   digitalWrite(led, 0);
}