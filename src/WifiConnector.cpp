#include "WifiConnector.h"
#include <ESP8266WiFi.h>

void WifiConnector::connect() {
    Serial.print("Connecting to ");
    Serial.println(_ssid.c_str());
    WiFi.begin(_ssid.c_str(), _password.c_str());
    Serial.println("");
  
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(_ssid.c_str());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
}

IPAddress WifiConnector::getLocalIPAddress() {
    return WiFi.localIP();
}
