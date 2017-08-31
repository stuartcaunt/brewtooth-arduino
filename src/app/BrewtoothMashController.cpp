#include "BrewtoothMashController.h"
#include <routing/WebServerRouter.h>
#include <Utils/Log.h>
#include <Arduino.h>
#include <ESP8266WebServer.h>

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
#include <Utils/Log.h>
    LOG("HTTP server started");
}

void BrewtoothMashController::loop() {
    _server->handleClient();
}