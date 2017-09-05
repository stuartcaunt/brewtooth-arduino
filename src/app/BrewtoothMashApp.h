#ifndef BREWTOOTHMASHAPP_H
#define BREWTOOTHMASHAPP_H

class BrewtoothWebServer;
class WebServerRouter;

class BrewtoothMashApp {
public:
    BrewtoothMashApp();
    virtual ~BrewtoothMashApp();

    void setup();
    void loop();

private:
    BrewtoothWebServer * _server;
    WebServerRouter * _router;
};

#endif /*BREWTOOTHMASHAPP_H*/