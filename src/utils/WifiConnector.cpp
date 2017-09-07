#include "WifiConnector.h"
#include "Log.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

void WifiConnector::connect() {
    // Initialise wifi connection
    WiFi.begin(_ssid.c_str(), _password.c_str());
    LOG("Connecting to %s", _ssid.c_str());
  
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
  
    LOG("Connected to %s", _ssid.c_str());
    LOG("IP address: %s", WiFi.localIP().toString().c_str());
}
