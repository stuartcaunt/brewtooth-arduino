#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "BrewtoothMashController.h"
#include <routing/WebServerRouter.h>

BrewtoothMashController::BrewtoothMashController() :
    _server(new ESP8266WebServer(80)),
    _router(new WebServerRouter(_server)) {
}

BrewtoothMashController::~BrewtoothMashController() {
    delete _router;
    delete _server;
}

void BrewtoothMashController::setup() {
    // Build server routes
    _router->buildRoutes();
    
    // start the server
    _server->begin();
    Serial.println("HTTP server started");
}

void BrewtoothMashController::loop() {
    _server->handleClient();
}