#ifndef WIFICONFIGURATIONSTATION_H
#define WIFICONFIGURATIONSTATION_H

#include "ApplicationState.h"

class WiFiManager;
class ESP8266WebServer;

class WifiConfigurationStation : public ApplicationState {
public:
    WifiConfigurationStation();
    virtual ~WifiConfigurationStation();

    virtual void setup();
    virtual void loop();

private:
    void handleRoot();
    void callback(WiFiManager * wiFiManager);
    
private:
    ESP8266WebServer * _server;
    WiFiManager * _wifiManager;
    
};

#endif /*WIFICONFIGURATIONSTATION_H*/