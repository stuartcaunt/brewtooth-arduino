#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H

#include <WiFiClient.h>

class WifiConnector {
public:
    WifiConnector(const std::string & ssid, const std::string & password) :
        _ssid(ssid),
        _password(password) {}
    virtual ~WifiConnector() {}

    void connect();
    IPAddress getLocalIPAddress();

private:
    std::string _ssid;
    std::string _password;   
};

#endif /*WIFICONNECTOR_H*/