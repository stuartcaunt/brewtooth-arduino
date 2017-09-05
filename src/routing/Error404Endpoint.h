#ifndef ERROR404ENDPOINT_H
#define ERROR404ENDPOINT_H

#include "WebServerEndpoint.h"

class Error404Endpoint : public WebServerEndpoint {
public:
    Error404Endpoint() {}
    virtual ~Error404Endpoint() {}

    virtual void buildPaths(BrewtoothWebServer * server);
    
private:
    void handle404(ESP8266WebServer * server);
};

#endif /*ERROR404ENDPOINT_H*/
