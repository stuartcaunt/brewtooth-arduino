#ifndef WEBSERVERENDPOINT_H
#define WEBSERVERENDPOINT_H

#include <ESP8266WebServer.h>

class WebServerEndpoint {
public:
    WebServerEndpoint() {}
    virtual ~WebServerEndpoint() {}

    virtual void buildPaths(ESP8266WebServer * server) = 0;

protected:

};

#endif /*WEBSERVERENDPOINT_H*/
