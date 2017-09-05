#ifndef WEBSERVERROUTER_H
#define WEBSERVERROUTER_H

#include <string>
#include <vector>

class BrewtoothWebServer;
class WebServerEndpoint;

class WebServerRouter {
public:
    WebServerRouter(BrewtoothWebServer * server);
    virtual ~WebServerRouter();

    void buildRoutes();
    
private:
    void addEndpoint(WebServerEndpoint * endpoint);
        
private:
    BrewtoothWebServer * _server;
    std::vector<WebServerEndpoint *> _endpoints;
};

#endif /*WEBSERVERROUTER_H*/