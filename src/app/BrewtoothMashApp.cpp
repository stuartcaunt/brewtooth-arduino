#include "BrewtoothMashApp.h"
#include <routing/WebServerRouter.h>
#include <service/ThermometerService.h>
#include <service/MashControllerService.h>
#include <utils/Log.h>
#include <Arduino.h>
#include <webserver/BrewtoothWebServer.h>

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
    _server->enableCORS(true);
    _server->begin();
    LOG("HTTP server started");
}

void BrewtoothMashApp::loop() {
    // Update temperature readers
    ThermometerService::_()->readTemperatures();

    // Update mash controllers
    MashControllerService::_()->updateControllers();

    _server->handleClient();
}