#ifndef WEBSERVERENDPOINT_H
#define WEBSERVERENDPOINT_H

#include <Arduino.h>

class BrewtoothWebServer;

class WebServerEndpoint {
public:
    WebServerEndpoint() {}
    virtual ~WebServerEndpoint() {}

    void setWebServer(BrewtoothWebServer * server) {
        _server = server;
    }
    virtual void buildPaths() = 0;

protected:
    BrewtoothWebServer * _server;

};

#endif /*WEBSERVERENDPOINT_H*/
