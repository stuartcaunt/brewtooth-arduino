#include "WebServerRouter.h"
#include "Error404Endpoint.h"
#include "RootEndpoint.h"
#include "TemperatureEndpoint.h"
#include <utils/Log.h>
#include <ESP8266WebServer.h>

WebServerRouter::WebServerRouter(ESP8266WebServer * server) :
    _server(server) {

}

WebServerRouter::~WebServerRouter() {
    _server = NULL;

    for (std::vector<WebServerEndpoint *>::iterator it = _endpoints.begin(); it < _endpoints.end(); it++) {
        delete *it;
    }
    _endpoints.clear();
}

void WebServerRouter::buildRoutes() {
    LOG("Building all routes");
    this->addEndpoint(new Error404Endpoint());
    this->addEndpoint(new RootEndpoint());
    this->addEndpoint(new TemperatureEndpoint());
}

void WebServerRouter::addEndpoint(WebServerEndpoint * endpoint) {
    endpoint->buildPaths(_server);
    this->_endpoints.push_back(endpoint);
}


