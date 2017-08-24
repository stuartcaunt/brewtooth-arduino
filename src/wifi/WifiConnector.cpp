#include "WifiConnector.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

void WifiConnector::connect() {
    // Initialise wifi connection
    WiFi.begin(_ssid.c_str(), _password.c_str());
    Serial.print("Connecting to ");
    Serial.print(_ssid);
  
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected to " + _ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
}


