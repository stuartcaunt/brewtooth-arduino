#include "BrewtoothMashApp.h"
#include <routing/WebServerRouter.h>
#include <service/MashControllerService.h>
#include <utils/Log.h>
#include <Arduino.h>
#include <utils/BrewtoothWebServer.h>

BrewtoothMashApp::BrewtoothMashApp() :
    _server(new BrewtoothWebServer(80)),
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
#include <utils/Log.h>
    LOG("HTTP server started");
}

void BrewtoothMashApp::loop() {
    _server->handleClient();
}