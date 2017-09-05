#include "TemperatureEndpoint.h"
#include <service/TemperatureReaderService.h>
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <utils/JsonStringBuilder.h>

void TemperatureEndpoint::buildPaths(ESP8266WebServer * server) {
    LOG("Building paths for TemperatureEndpoint");
    
    server->on("/temperature", std::bind(&TemperatureEndpoint::getTemperature, this, server));
    
    server->on("/thermometers", HTTPMethod::HTTP_GET, std::bind(&TemperatureEndpoint::getTemperatureReaders, this, server));
    server->on("/thermometers", HTTPMethod::HTTP_POST, std::bind(&TemperatureEndpoint::addTemperatureReader, this, server));
}

void TemperatureEndpoint::getTemperatureReaders(ESP8266WebServer * server) {
    LOG("Getting all temperature readers");
    std::vector<TemperatureReader *> temperatureReaders = TemperatureReaderService::_()->getAll();
    std::vector<Jsonable *> jsonables;
    jsonables.insert(jsonables.end(), temperatureReaders.begin(), temperatureReaders.end());

    server->send(200, "application/json", JsonStringBuilder::jsonString(jsonables).c_str());
}

void TemperatureEndpoint::addTemperatureReader(ESP8266WebServer * server) {
    LOG("Adding a temperature reader");
    
    if (server->hasArg("plain")) {
        String jsonText = server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (!json.success()) {
            ERROR("Failed to parse JSON data");
            server->send(200, "text/plain", "Failed to parse JSON data");
        
        } else {
            // Build new temperature reader
            TemperatureReaderConfig readerConfig(json);
            TemperatureReader * temperatureReader = TemperatureReaderService::_()->add(readerConfig);

            // Send json response
            server->send(200, "application/json", JsonStringBuilder::jsonString(temperatureReader).c_str());
        }

    } else {
        ERROR("Body not received");
        server->send(200, "text/plain", "Body not received");
        return;
    }
}
    
void TemperatureEndpoint::getTemperature(ESP8266WebServer * server) {
    LOG("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    server->send(200, "text/plain", message);
}