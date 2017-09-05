#ifndef PATHPARAMETERFUNCTIONREQUESTHANDLER_H
#define PATHPARAMETERFUNCTIONREQUESTHANDLER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

template<typename T>
class PathParameterFunctionRequestHandler : public RequestHandler {
public:
    PathParameterFunctionRequestHandler(std::function<void(T)> fn, const char* uri, HTTPMethod method) :
        _fn(fn),
        _uri(uri),
        _method(method) {
    }
    virtual ~PathParameterFunctionRequestHandler() {
    }

    virtual bool canHandle(HTTPMethod requestMethod, String requestUri) {
        if (_method != HTTP_ANY && _method != requestMethod) {
            return false;
            
        }

        if (requestUri != _uri) {
            return false;
        }

        return true;
    }

    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
        // Get param from path


    }
    
private:
    std::function<void(T)> _fn;
    String _uri;
    HTTPMethod _method;
};

#endif /*PATHPARAMETERFUNCTIONREQUESTHANDLER_H*/