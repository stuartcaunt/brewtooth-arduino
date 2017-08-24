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
 
    // Initialise SPIFFS
    SPIFFS.begin();

    // Initialise configuration
    Configuration::init();

    if (Configuration::properties.wifiData.isConfigured) {
        Serial.println("Wifi is configured: connecting to wifi and running server");
    
        // Connect to wifi
        wifiConnector.connect();
    
        // Build server routes
        router.buildRoutes();
    
        // start the server
        server.begin();
        Serial.println("HTTP server started");
    
    } else {
        Serial.println("Setting up wifi station to obtain network details");
     
        Configuration::properties.wifiData.isConfigured = true;
        Configuration::save();
    }
}

void loop(void){
    if (Configuration::properties.wifiData.isConfigured) {
        server.handleClient();
    }
}
