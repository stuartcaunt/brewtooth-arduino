#include "WebServerRouter.h"
#include "Error404Endpoint.h"
#include "RootEndpoint.h"
#include "ConfigurationEndpoint.h"
#include "ThermometerEndpoint.h"
#include "MashControllerEndpoint.h"
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>

WebServerRouter::WebServerRouter(BrewtoothWebServer * server) :
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
    this->addEndpoint(new ConfigurationEndpoint());
    this->addEndpoint(new ThermometerEndpoint());
    this->addEndpoint(new MashControllerEndpoint());
}

void WebServerRouter::addEndpoint(WebServerEndpoint * endpoint) {
    endpoint->setWebServer(_server);
    endpoint->buildPaths();
    this->_endpoints.push_back(endpoint);
}


