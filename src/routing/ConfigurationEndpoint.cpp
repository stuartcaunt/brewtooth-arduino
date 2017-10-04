#include "ConfigurationEndpoint.h"
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>

void ConfigurationEndpoint::buildPaths() {
    LOG("Building paths for ConfigurationEndpoint");
    
    _server->on("/configuration", std::bind(&ConfigurationEndpoint::getProperties, this));
}

void ConfigurationEndpoint::getProperties() {
    String jsonString  = Configuration::_()->getPropertiesJsonString();

    _server->send(200, "application/json", jsonString);
}