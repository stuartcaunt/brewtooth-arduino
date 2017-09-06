#ifndef PATHPARAMETERFUNCTIONREQUESTHANDLER_H
#define PATHPARAMETERFUNCTIONREQUESTHANDLER_H

#include <Arduino.h>
#include <vector>
#include <string>
#include <ESP8266WebServer.h>
#include <utils/StringFunctions.h>
#include <utils/Converter.h>

struct PathPart {
    PathPart(const std::string & text, bool isParam = false) :
        text(text),
        isParam(isParam) {}
    std::string text;
    boolean isParam;
    std::string lastParamValue;
};

template<typename T>
class PathParameterFunctionRequestHandler : public RequestHandler {
public:
    PathParameterFunctionRequestHandler(std::function<void(T)> fn, const char* uri, HTTPMethod method) :
        _fn(fn),
        _uri(uri),
        _method(method) {
        std::vector<std::string> pathTexts = split(uri, "/");

        for (int index = 0; index < pathTexts.size(); index++) {
            std::string pathText = pathTexts[index];
            if (startsWith(pathText, "{") && endsWith(pathText, "}")) {
                pathText = replace_all(pathText, "{", "");
                pathText = replace_all(pathText, "}", "");
                _pathParts.push_back(PathPart(pathText, true));
                _paramIndex = index;

            } else {
                _pathParts.push_back(PathPart(pathText));
            }
        }
    }
    virtual ~PathParameterFunctionRequestHandler() {
    }

    virtual bool canHandle(HTTPMethod requestMethod, String requestUri) {
        _lastRequestUri = "";

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

        // Store last request Uri
        _lastRequestUri = requestUri;

        return true;
    }

    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) {
        std::string paramValueString;
        if (_lastRequestUri == requestUri) {
            paramValueString = _pathParts[_paramIndex].lastParamValue;
        
        } else {
            // re-obtain paramValue string
            if (!canHandle(requestMethod, requestUri)) {
                return false;
            }

            paramValueString = _pathParts[_paramIndex].lastParamValue;
        }

        // Get param from path
        T paramValue = Converter<T>::convert(paramValueString);

        // Call function
        _fn(paramValue);

        return true;
    }
    
private:
    std::function<void(T)> _fn;
    String _uri;
    HTTPMethod _method;
    std::vector<PathPart> _pathParts;
    
    int _paramIndex;
    String _lastRequestUri;
};

#endif /*PATHPARAMETERFUNCTIONREQUESTHANDLER_H*/