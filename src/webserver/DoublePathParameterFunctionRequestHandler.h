#ifndef DOUBLEPATHPARAMETERFUNCTIONREQUESTHANDLER_H
#define DOUBLEPATHPARAMETERFUNCTIONREQUESTHANDLER_H

#include "PathPart.h"
#include <Arduino.h>
#include <vector>
#include <string>
#include <ESP8266WebServer.h>
#include <utils/StringFunctions.h>
#include <utils/Converter.h>

template<typename ParamA, typename ParamB>
class DoublePathParameterFunctionRequestHandler : public RequestHandler {
public:
    DoublePathParameterFunctionRequestHandler(std::function<void(ParamA, ParamB)> fn, const char* uri, HTTPMethod method) :
        _fn(fn),
        _uri(uri),
        _method(method),
        _paramIndexA(-1),
        _paramIndexB(-1) {
        std::vector<std::string> pathTexts = split(uri, "/");

        for (int index = 0; index < pathTexts.size(); index++) {
            std::string pathText = pathTexts[index];
            if (startsWith(pathText, "{") && endsWith(pathText, "}")) {
                pathText = replace_all(pathText, "{", "");
                pathText = replace_all(pathText, "}", "");
                _pathParts.push_back(PathPart(pathText, true));

                if (_paramIndexA == -1) {
                    _paramIndexA = index;
                } else {
                    _paramIndexB = index;
                }

            } else {
                _pathParts.push_back(PathPart(pathText));
            }
        }
    }
    virtual ~DoublePathParameterFunctionRequestHandler() {
    }

    virtual bool canHandle(HTTPMethod requestMethod, const String& requestUri) {
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

    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, const String& requestUri) {
        std::string paramValueStringA;
        std::string paramValueStringB;
        if (_lastRequestUri != requestUri) {
            // re-obtain paramValue string
            if (!canHandle(requestMethod, requestUri)) {
                return false;
            }

        }
        paramValueStringA = _pathParts[_paramIndexA].lastParamValue;
        paramValueStringB = _pathParts[_paramIndexB].lastParamValue;

        // Get param from path
        ParamA paramValueA = Converter<ParamA>::convert(paramValueStringA);
        ParamB paramValueB = Converter<ParamB>::convert(paramValueStringB);

        // Call function
        _fn(paramValueA, paramValueB);

        return true;
    }
    
private:
    std::function<void(ParamA, ParamB)> _fn;
    String _uri;
    HTTPMethod _method;
    std::vector<PathPart> _pathParts;
    
    int _paramIndexA;
    int _paramIndexB;
    String _lastRequestUri;
};

#endif /*DOUBLEPATHPARAMETERFUNCTIONREQUESTHANDLER_H*/