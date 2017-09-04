#include "TemperatureEndpoint.h"
#include <utils/Configuration.h>
#include <utils/Log.h>

void TemperatureEndpoint::buildPaths(ESP8266WebServer * server) {
    LOG("Building paths for TemperatureEndpoint");
    
    server->on("/temperature", std::bind(&TemperatureEndpoint::getTemperature, this, server));
    
    server->on("/thermometers", HTTPMethod::HTTP_GET, std::bind(&TemperatureEndpoint::getTemperatureReaders, this, server));
    server->on("/thermometers", HTTPMethod::HTTP_POST, std::bind(&TemperatureEndpoint::addTemperatureReader, this, server));
}

void TemperatureEndpoint::getTemperatureReaders(ESP8266WebServer * server) {
    const std::vector<TemperatureReaderConfig> & temperatureReaders = Configuration::properties.temperatureReaders;

    DynamicJsonBuffer jsonBuffer;
    JsonArray & json = jsonBuffer.createArray();

    for (std::vector<TemperatureReaderConfig>::const_iterator it = temperatureReaders.begin(); it != temperatureReaders.end(); it++) {
        const TemperatureReaderConfig & readerConfig = *it;
        readerConfig.convertToJson(json.createNestedObject());
    }

    String jsonString;
    json.printTo(jsonString);
    server->send(200, "application/json", jsonString.c_str());
}

void TemperatureEndpoint::addTemperatureReader(ESP8266WebServer * server) {

}
    
void TemperatureEndpoint::getTemperature(ESP8266WebServer * server) {
    LOG("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    server->send(200, "text/plain", message);
}