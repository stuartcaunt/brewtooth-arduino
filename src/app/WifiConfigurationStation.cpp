#include "WifiConfigurationStation.h"
#include "../utils/Configuration.h"
#include <ESP8266Wifi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 

WifiConfigurationStation::WifiConfigurationStation() :
    _server(new ESP8266WebServer(80)),
    _wifiManager(new WiFiManager()) {
        
}

WifiConfigurationStation::~WifiConfigurationStation() {
    delete _server;
    delete _wifiManager;
}

void WifiConfigurationStation::setup() {

    String macAddress = WiFi.softAPmacAddress();
    Serial.println("Mac addr = " + macAddress);
    String ssid = "BREWTOOTH-" + macAddress.substring(0, 4);


    _wifiManager->autoConnect("BREWTOOTH");
    // _wifiManager->setAPCallback(std::bind(&WifiConfigurationStation::callback, this, _wifiManager));


    /* You can remove the password parameter if you want the AP to be open. */
    // WiFi.softAP(ssid.c_str());
    // IPAddress myIP = WiFi.softAPIP();
    // Serial.print("AP IP address: ");
    // Serial.println(myIP);

    // _server->on("/", std::bind(&WifiConfigurationStation::handleRoot, this));
    
    // _server->begin();
    // Serial.println("HTTP server started");

    // Configuration::properties.wifiData.isConfigured = true;
    // Configuration::save();

}

void WifiConfigurationStation::loop() {
    
}

void WifiConfigurationStation::callback(WiFiManager * wiFiManager) {
    Serial.println("hello");   
}

void WifiConfigurationStation::handleRoot() {
    _server->send(200, "text/html", "<h1>You are connected</h1>");
}