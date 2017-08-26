#ifndef BREWTOOTHMASHCONTROLLER_H
#define BREWTOOTHMASHCONTROLLER_H

class ESP8266WebServer;
class WebServerRouter;

class BrewtoothMashController {
public:
    BrewtoothMashController();
    virtual ~BrewtoothMashController();

    void setup();
    void loop();

private:
    ESP8266WebServer * _server;
    WebServerRouter * _router;
};

#endif /*BREWTOOTHMASHCONTROLLER_H*/