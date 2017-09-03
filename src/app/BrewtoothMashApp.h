#ifndef BREWTOOTHMASHAPP_H
#define BREWTOOTHMASHAPP_H

class ESP8266WebServer;
class WebServerRouter;

class BrewtoothMashApp {
public:
    BrewtoothMashApp();
    virtual ~BrewtoothMashApp();

    void setup();
    void loop();

private:
    ESP8266WebServer * _server;
    WebServerRouter * _router;
};

#endif /*BREWTOOTHMASHAPP_H*/