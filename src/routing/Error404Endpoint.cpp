#include "Error404Endpoint.h"
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>

void Error404Endpoint::buildPaths() {
    LOG("Building paths for Error404Endpoint");
    
    _server->onNotFound(std::bind(&Error404Endpoint::handle404, this));
}

void Error404Endpoint::handle404() {
    LOG("handle404");
    
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server->uri();
    message += "\nMethod: ";
    message += (_server->method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += _server->args();
    message += "\n";
    for (uint8_t i=0; i<_server->args(); i++){
        message += " " + _server->argName(i) + ": " + _server->arg(i) + "\n";
    }
    _server->send(404, "text/plain", message);
}