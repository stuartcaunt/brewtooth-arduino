#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "BrewtoothMashController.h"
#include "../wifi/WifiConnector.h"
#include "../routing/WebServerRouter.h"

BrewtoothMashController::BrewtoothMashController() :
    _server(new ESP8266WebServer(80)),
    _wifiConnector(new WifiConnector ("NUMERICABLE-21EE", "kzPqS3jm3MdyIjhl")),
    _router(new WebServerRouter(_server)) {
    
}

BrewtoothMashController::~BrewtoothMashController() {
    delete _router;
    delete _wifiConnector;
    delete _server;
}


void BrewtoothMashController::setup() {
        // Connect to wifi
        _wifiConnector->connect();
        
        // Build server routes
        _router->buildRoutes();
    
        // start the server
        _server->begin();
        Serial.println("HTTP server started");
}

void BrewtoothMashController::loop() {
    _server->handleClient();
}