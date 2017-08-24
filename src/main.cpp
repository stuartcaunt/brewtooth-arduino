#include <Arduino.h>
#include <FS.h>
#include "utils/Configuration.h"
#include "app/BrewtoothMashController.h"
#include "app/WifiConfigurationStation.h"

ApplicationState * appState = 0;

const int led = 13;

void setup(void){
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);

    // Initialise serial port
    Serial.begin(9600);
    Serial.println("");
 
    // Initialise SPIFFS
    SPIFFS.begin();

    // Initialise configuration
    Configuration::init();

    if (Configuration::properties.wifiData.isConfigured) {
        Serial.println("Wifi is configured: connecting to wifi and running server");
    
        appState = new BrewtoothMashController();
    
    } else {
        Serial.println("Setting up wifi station to obtain network details");
     
        appState = new WifiConfigurationStation();
    }

    // Setup the current app state
    appState->setup();    
}

void loop(void){
  // Loop with the current app state
  appState->loop();
}
