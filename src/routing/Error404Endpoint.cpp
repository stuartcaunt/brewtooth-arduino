#include "Error404Endpoint.h"
#include <Utils/Log.h>

void Error404Endpoint::buildPaths(ESP8266WebServer * server) {
    LOG("Building paths for Error404Endpoint");
    
    server->onNotFound(std::bind(&Error404Endpoint::handle404, this, server));
}

void Error404Endpoint::handle404(ESP8266WebServer * server) {
    LOG("handle404");
    
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server->uri();
    message += "\nMethod: ";
    message += (server->method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server->args();
    message += "\n";
    for (uint8_t i=0; i<server->args(); i++){
        message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
    }
    server->send(404, "text/plain", message);
}