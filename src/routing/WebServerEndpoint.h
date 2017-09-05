#ifndef WEBSERVERENDPOINT_H
#define WEBSERVERENDPOINT_H

#include <Arduino.h>
#include <utils/BrewtoothWebServer.h>

class WebServerEndpoint {
public:
    WebServerEndpoint() {}
    virtual ~WebServerEndpoint() {}

    virtual void buildPaths(BrewtoothWebServer * server) = 0;

protected:

};

#endif /*WEBSERVERENDPOINT_H*/
