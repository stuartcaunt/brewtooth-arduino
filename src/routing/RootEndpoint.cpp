#include "RootEndpoint.h"
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>

void RootEndpoint::buildPaths() {
    LOG("Building paths for RootEndpoint");
    
    _server->on("/", std::bind(&RootEndpoint::getMessage, this));
}

void RootEndpoint::getMessage() {
//   digitalWrite(led, 1);
    LOG("getMessage");
    _server->send(200, "text/plain", "Welcome to the brewtooth ESP8266 server!");
//   digitalWrite(led, 0);
}