#include "WifiConfigurationStation.h"
#include "../utils/Configuration.h"
#include <ESP8266Wifi.h>
#include <ESP8266WebServer.h>

WifiConfigurationStation::WifiConfigurationStation() :
    _server(new ESP8266WebServer(80)) {

}

WifiConfigurationStation::~WifiConfigurationStation() {
    delete _server;
}

void WifiConfigurationStation::setup() {

    String macAddress = WiFi.softAPmacAddress();
    Serial.println("Mac addr = " + macAddress);
    String ssid = "BREWTOOTH-" + macAddress.substring(0, 4);

    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    _server->on("/", std::bind(&WifiConfigurationStation::handleRoot, this));
    
    _server->begin();
    Serial.println("HTTP server started");

    // Configuration::properties.wifiData.isConfigured = true;
    // Configuration::save();

}

void WifiConfigurationStation::loop() {
    
}

void WifiConfigurationStation::handleRoot() {
    _server->send(200, "text/html", "<h1>You are connected</h1>");
}