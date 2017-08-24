#ifndef BREWTOOTHMASHCONTROLLER_H
#define BREWTOOTHMASHCONTROLLER_H

#include "ApplicationState.h"

class ESP8266WebServer;
class WifiConnector;
class WebServerRouter;

class BrewtoothMashController : public ApplicationState {
public:
    BrewtoothMashController();
    virtual ~BrewtoothMashController();

    virtual void setup();
    virtual void loop();

private:
    ESP8266WebServer * _server;
    WifiConnector * _wifiConnector;
    WebServerRouter * _router;
    
};

#endif /*BREWTOOTHMASHCONTROLLER_H*/