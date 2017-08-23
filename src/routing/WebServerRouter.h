#ifndef WEBSERVERROUTER_H
#define WEBSERVERROUTER_H

#include <string>
#include <vector>

class ESP8266WebServer;
class WebServerEndpoint;

class WebServerRouter {
public:
    WebServerRouter(ESP8266WebServer * server);
    virtual ~WebServerRouter();

    void buildRoutes();
    
private:
    void addEndpoint(WebServerEndpoint * endpoint);
        
private:
    ESP8266WebServer * _server;
    std::vector<WebServerEndpoint *> _endpoints;
};

#endif /*WEBSERVERROUTER_H*/