#include <Arduino.h>
#include <FS.h>
#include <app/BrewtoothMashApp.h>
#include <utils/WifiConnector.h>
#include <utils/Log.h>

BrewtoothMashApp * mashApp = 0;

void setup(void){
    // Initialise serial port
    Serial.begin(9600);
    Serial.println("");
    
    LOG("Application starting");
    
    // Initialise SPIFFS
    SPIFFS.begin();
    
    WifiConnector wifiConnector(WIFI_SSID, WIFI_PASSWORD);
    wifiConnector.connect();
    
    LOG("... wifi setup terminated");
    
    // Create and setup mash controller
    mashApp = new BrewtoothMashApp();
    mashApp->setup();

    LOG("App running");
}

void loop(void){
    // Loop over mash controller
    mashApp->loop();
}
