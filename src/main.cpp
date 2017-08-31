#include <Arduino.h>
#include <FS.h>
#include <WiFiManager.h> 
#include <DoubleResetDetector.h>
#include "app/BrewtoothMashController.h"
#include "utils/WifiConnector.h"
#include "utils/Configuration.h"
#include "service/TemperatureReaderService.h"

#define DRD_TIMEOUT 4
#define DRD_ADDRESS 0

#define DEBUG_WIFI_CONNECTION 1

#define DEBUG(__fmt, ...) Serial.printf("[DEBUG] %.30s %s %4d " __fmt"\n", __FILE__, __TIME__, __LINE__, ##__VA_ARGS__)
#define LOG(__fmt, ...) Serial.printf("[INFO ] %.30s %s %4d " __fmt"\n", __FILE__, __TIME__, __LINE__, ##__VA_ARGS__)
#define WARN(__fmt, ...) Serial.printf("[WARN ] %.30s %4d " __fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define ERROR(__fmt, ...) Serial.printf("[ERROR] %.30s %4d " __fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)

DoubleResetDetector doubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
BrewtoothMashController * mashController = 0;

void setup(void){
    LOG("This is a test");
    LOG("This is another %s test", "hi");

    // Initialise serial port
    Serial.begin(9600);
 
    // Initialise SPIFFS
    SPIFFS.begin();
    
    // Initialise configuration
    Configuration::init();

    // Reset if double reset detected
    if (doubleResetDetector.detectDoubleReset()) {
        Serial.println("Double Reset Detected: resetting configuration");
        Configuration::reset();
    }

    // Initialise Temperature Reader Service
    TemperatureReaderService::_()->init();

    Serial.println("Application started");
    
#if (DEBUG_WIFI_CONNECTION == 1)
    WifiConnector wifiConnector("NUMERICABLE-21EE", "kzPqS3jm3MdyIjhl");
    wifiConnector.connect();
#else
    // reset saved settings when double reset occurs
    WiFiManager wiFiManager;
    if (doubleResetDetector.detectDoubleReset()) {
        Serial.println("Double Reset Detected: resetting wifi manager");
        wiFiManager.resetSettings();
    }

    // Start wifi connection
    String macAddress = WiFi.softAPmacAddress();
    macAddress.replace(":", "");
    Serial.println("Mac addr = " + macAddress;
    String ssid = "BREWTOOTH-" + macAddress.substring(0, 4);

    Serial.println("Waiting for wifi setup...");
    if (!wiFiManager.autoConnect(ssid.c_str())) {
        Serial.println("Wifi setup failed... retry");
        
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);

    }
    #endif
    
    Serial.println("... wifi setup terminated");

    // Create mash controller
    mashController = new BrewtoothMashController();
    
    // Setup the mash controller
    mashController->setup();    
}

void loop(void){
    // Loop over mash controller
    mashController->loop();

    // Determine if double reset has been done
    doubleResetDetector.loop();
}
