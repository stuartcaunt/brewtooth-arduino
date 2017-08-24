#include <Arduino.h>
#include <FS.h>
#include <ESP8266WebServer.h>

#include "WifiConnector.h"
#include "Configuration.h"
#include "routing/WebServerRouter.h"

ESP8266WebServer server(80);
WifiConnector wifiConnector("NUMERICABLE-21EE", "kzPqS3jm3MdyIjhl");
WebServerRouter router(&server);

const int led = 13;

void setup(void){
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);

    // Initialise serial port
    Serial.begin(9600);

    // Connect to wifi
    wifiConnector.connect();

    // Build server routes
    router.buildRoutes();
 
    // Initialise SPIFFS
    SPIFFS.begin();
    Configuration::getInstance()->init();

    // start the server
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
