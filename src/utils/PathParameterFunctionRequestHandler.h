#ifndef PATHPARAMETERFUNCTIONREQUESTHANDLER_H
#define PATHPARAMETERFUNCTIONREQUESTHANDLER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <utils/StringFunctions.h>

struct PathPart {
    PathPart(const std::string & text, bool isParam = false) :
        text(text),
        isParam(isParam) {}
    std::string text;
    boolean isParam;
    std::String lastParamValue;
};

template<typename T>
class PathParameterFunctionRequestHandler : public RequestHandler {
public:
    PathParameterFunctionRequestHandler(std::function<void(T)> fn, const char* uri, HTTPMethod method) :
        _fn(fn),
        _uri(uri),
        _method(method) {
        std::vector<std::string> pathTexts = split(uri, "/");

        int index = 0;
        for (int index = 0; index < pathTexts.length(); index++) {
            std::string pathText = *it;
            if (startsWith(pathText, "{") && endsWith(pathText, "}")) {
                _pathParts.push_back(PathPart(pathText, true));
            } else {
                _pathParts.push_back(PathPart(pathText));
            }
        }
    }
    virtual ~PathParameterFunctionRequestHandler() {
    }

    virtual bool canHandle(HTTPMethod requestMethod, String requestUri) {
        if (_method != HTTP_ANY && _method != requestMethod) {
            return false;
        }

        // Verify path component size match
        std::vector<std::string> requestUriParts = split(requestUri.c_str(), "/");
        if (requestUriParts.size() != _pathParts.size()) {
            return false;
        }

        // compare each path part to check if strings match when not a path param
        for (int i = 0; i < _pathParts.size(); i++) {
            if (_pathParts[i].isParam) {
                _pathParts[i].lastParamValue = requestUriParts[i];

            } else {
                if (_pathParts[i].text != requestUriParts[i]) {
                    return false;
                }
            }
        }

        _lastRequestUri = requestUri;

        return true;
    }

    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
        // Get param from path


    }
    
private:
    std::function<void(T)> _fn;
    String _uri;
    HTTPMethod _method;
    std::vector<PathPart> _pathParts;
    int paramIndex;
    String _lastRequestUri;
};

#endif /*PATHPARAMETERFUNCTIONREQUESTHANDLER_H*/