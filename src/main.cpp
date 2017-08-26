#include <Arduino.h>
#include <WiFiManager.h> 
#include <DoubleResetDetector.h>
#include "app/BrewtoothMashController.h"

// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector doubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
BrewtoothMashController * mashController = 0;

void setup(void){

    // Initialise serial port
    Serial.begin(9600);
    Serial.println("");

    // reset saved settings when double reset occurs
    WiFiManager wiFiManager;
    if (doubleResetDetector.detectDoubleReset()) {
        Serial.println("Double Reset Detected: resetting wifi");
        wiFiManager.resetSettings();
    }

    // Start wifi connection
    String macAddress = WiFi.softAPmacAddress();
    macAddress.replace(":", "");
    Serial.println("Mac addr = " + macAddress);
    String ssid = "BREWTOOTH-" + macAddress.substring(0, 4);

    Serial.println("Waiting for wifi setup...");
    if (!wiFiManager.autoConnect(ssid.c_str())) {
        Serial.println("Wifi setup failed... retry");
        
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);

    } else {
        Serial.println("... wifi setup terminated");

        // Create mash controller
        mashController = new BrewtoothMashController();
        
        // Setup the mash controller
        mashController->setup();    
    }
}

void loop(void){
    if (mashController != 0) {
        mashController->loop();
    }
    doubleResetDetector.loop();
}
