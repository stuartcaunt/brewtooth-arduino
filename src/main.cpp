#include <Arduino.h>
#include <FS.h>
#include <WiFiManager.h> 
#include <DoubleResetDetector.h>
#include <app/BrewtoothMashApp.h>
#include <utils/WifiConnector.h>
// #include <utils/Configuration.h>
#include <utils/Log.h>

#define DRD_TIMEOUT 10
#define DRD_ADDRESS 0

#define DEBUG_WIFI_CONNECTION 1

DoubleResetDetector doubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
BrewtoothMashApp * mashApp = 0;

void setup(void){
    // Initialise serial port
    Serial.begin(9600);
    Serial.println("");
    
    LOG("Application starting");
    
    // Initialise SPIFFS
    SPIFFS.begin();
    
    // Initialise configuration (reset if double reset detector activated)
    bool isDoubleReset = doubleResetDetector.detectDoubleReset();
    // Configuration::initInstance(isDoubleReset);
    
    // Create and setup mash controller
    mashApp = new BrewtoothMashApp();
    mashApp->setup();

#if (DEBUG_WIFI_CONNECTION == 1)
    WifiConnector wifiConnector("NUMERICABLE-21EE", "kzPqS3jm3MdyIjhl");
    wifiConnector.connect();
#else
    // reset saved settings when double reset occurs
    WiFiManager wiFiManager;
    if (isDoubleReset) {
        LOG("Double Reset Detected: resetting wifi manager");
        wiFiManager.resetSettings();
    }

    // Start wifi connection
    String macAddress = WiFi.softAPmacAddress();
    macAddress.replace(":", "");
    LOG("Mac addr = %s",  macAddress.c_str());
    String ssid = "BREWTOOTH-" + macAddress.substring(0, 6);

    LOG("Waiting for wifi setup...");
    if (!wiFiManager.autoConnect(ssid.c_str())) {
        LOG("Wifi setup failed... retry");
        
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);

    }
#endif
    
    LOG("... wifi setup terminated");

    LOG("App running");
}

void loop(void){
    // Loop over mash controller
    mashApp->loop();

    // Determine if double reset has been done
    doubleResetDetector.loop();
}
