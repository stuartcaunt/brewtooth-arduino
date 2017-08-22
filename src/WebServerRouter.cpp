#include "WebServerRouter.h"
#include <ESP8266WebServer.h>

WebServerRouter::WebServerRouter(int port) :
    _server(new ESP8266WebServer(port)) {

}

WebServerRouter::~WebServerRouter() {
    delete _server;
    _server = NULL;
}

void WebServerRouter::addRoute(const std::string & path, const WebServerEndpoint & endpoint) {
    using std::placeholders::_1;
    ESP8266WebServer::THandlerFunction handler = std::bind(&WebServerEndpoint::pathHandler, endpoint, _1);

    _server->on(path.c_str, handler);
}
