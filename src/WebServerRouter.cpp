#include "WebServerRouter.h"
#include "TemperatureEndpoint.h"
#include <ESP8266WebServer.h>

WebServerRouter::WebServerRouter(int port) :
    _server(new ESP8266WebServer(port)) {

}

WebServerRouter::~WebServerRouter() {
    delete _server;
    _server = NULL;

    for (std::vector<WebServerEndpoint *>::iterator it = _endpoints.begin(); it < _endpoints.end(); it++) {
        delete *it;
    }
    _endpoints.clear();
}

void WebServerRouter::buildRoutes() {
    Serial.println("Building all routes");
    this->addEndpoint(new TemperatureEndpoint());
}

void WebServerRouter::begin() {
    _server->begin();
}

void WebServerRouter::handleClient() {
    _server->handleClient();
}


void WebServerRouter::addEndpoint(WebServerEndpoint * endpoint) {
    endpoint->buildPaths(_server);
    this->_endpoints.push_back(endpoint);
}


