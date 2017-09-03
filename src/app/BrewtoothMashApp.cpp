#include "BrewtoothMashApp.h"
#include <routing/WebServerRouter.h>
#include <service/MashControllerService.h>
#include <Utils/Log.h>
#include <Arduino.h>
#include <ESP8266WebServer.h>

BrewtoothMashApp::BrewtoothMashApp() :
    _server(new ESP8266WebServer(80)),
    _router(new WebServerRouter(_server)) {
}

BrewtoothMashApp::~BrewtoothMashApp() {
    delete _router;
    delete _server;
}

void BrewtoothMashApp::setup() {
    // Initialise mash controller service
    MashControllerService::init();

    // Build server routes
    _router->buildRoutes();
    
    // start the server
    _server->begin();
#include <Utils/Log.h>
    LOG("HTTP server started");
}

void BrewtoothMashApp::loop() {
    _server->handleClient();
}