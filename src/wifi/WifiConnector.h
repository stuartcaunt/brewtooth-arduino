#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H

#include <Arduino.h>

class WifiConnector {
public:
    WifiConnector(const String & ssid, const String & password) :
        _ssid(ssid),
        _password(password) {}
    virtual ~WifiConnector() {}

    void connect();

private:
    String _ssid;
    String _password;   
};

#endif /*WIFICONNECTOR_H*/