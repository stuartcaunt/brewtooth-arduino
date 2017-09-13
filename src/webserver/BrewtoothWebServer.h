#ifndef BREWTOOTHWEBSERVER_H
#define BREWTOOTHWEBSERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "PathParameterFunctionRequestHandler.h"
#include "DoublePathParameterFunctionRequestHandler.h"

class BrewtoothWebServer : public ESP8266WebServer {
public:
    BrewtoothWebServer(IPAddress addr, int port = 80) : 
        ::ESP8266WebServer(addr, port) {}
    BrewtoothWebServer(int port = 80) :
        ::ESP8266WebServer(port) {}
    virtual ~BrewtoothWebServer() {}

    template<typename T>
    void onPathParam(const char* uri, HTTPMethod method, std::function<void(T)> fn) {
        _addRequestHandler(new PathParameterFunctionRequestHandler<T>(fn, uri, method));
    }

    template<typename ParamA, typename ParamB>
    void onPathParam(const char* uri, HTTPMethod method, std::function<void(ParamA, ParamB)> fn) {
        _addRequestHandler(new DoublePathParameterFunctionRequestHandler<ParamA, ParamB>(fn, uri, method));
    }
    
private:
};

#endif /*BREWTOOTHWEBSERVER_H*/