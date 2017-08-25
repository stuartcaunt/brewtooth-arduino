#ifndef WIFICONFIGURATIONSTATION_H
#define WIFICONFIGURATIONSTATION_H

#include "ApplicationState.h"

class ESP8266WebServer;

class WifiConfigurationStation : public ApplicationState {
public:
    WifiConfigurationStation();
    virtual ~WifiConfigurationStation();

    virtual void setup();
    virtual void loop();

private:
    void handleRoot();
        
private:
    ESP8266WebServer * _server;
    
};

#endif /*WIFICONFIGURATIONSTATION_H*/