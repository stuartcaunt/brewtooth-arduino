#ifndef ROOTENDPOINT_H
#define ROOTENDPOINT_H

#include "WebServerEndpoint.h"

class RootEndpoint : public WebServerEndpoint {
public:
    RootEndpoint() {}
    virtual ~RootEndpoint() {}

    virtual void buildPaths(BrewtoothWebServer * server);
    
private:
    void getMessage(ESP8266WebServer * server);
};

#endif /*ROOTENDPOINT_H*/
