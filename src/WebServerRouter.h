#ifndef WEBSERVERROUTER_H
#define WEBSERVERROUTER_H

#include "WebServerEndpoint.h"
#include <string>

class ESP8266WebServer;

class WebServerRouter {
public:
    WebServerRouter(int port);
    virtual ~WebServerRouter();

    void addRoute(const std::string & path, const WebServerEndpoint & endpoint);
        
private:
    ESP8266WebServer * _server;
};

#endif /*WEBSERVERROUTER_H*/