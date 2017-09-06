#include "TemperatureEndpoint.h"
#include <service/TemperatureReaderService.h>
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>
#include <utils/JsonStringBuilder.h>

void TemperatureEndpoint::buildPaths() {
    LOG("Building paths for TemperatureEndpoint");
    using namespace std::placeholders;

    _server->on("/temperature", std::bind(&TemperatureEndpoint::getTemperature, this));
    
    _server->on("/thermometers", HTTPMethod::HTTP_GET, std::bind(&TemperatureEndpoint::getTemperatureReaders, this));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_GET, std::bind(&TemperatureEndpoint::getTemperatureReader, this, _1));
    _server->on("/thermometers", HTTPMethod::HTTP_POST, std::bind(&TemperatureEndpoint::addTemperatureReader, this));
}

void TemperatureEndpoint::getTemperatureReaders() {
    LOG("Getting all temperature readers");
    std::vector<TemperatureReader *> temperatureReaders = TemperatureReaderService::_()->getAll();

    // convert vector to create json array
    std::vector<Jsonable *> jsonables;
    jsonables.insert(jsonables.end(), temperatureReaders.begin(), temperatureReaders.end());

    _server->send(200, "application/json", JsonStringBuilder::jsonString(jsonables).c_str());
}

void TemperatureEndpoint::getTemperatureReader(int id) {
    LOG("Getting temperature reader id = %d", id);

    TemperatureReader * temperatureReader = TemperatureReaderService::_()->get(id);
    if (temperatureReader != NULL) {
        _server->send(200, "application/json", JsonStringBuilder::jsonString(temperatureReader).c_str());
    } else {
        WARN("Temperature reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Temperature reader not found");
    }
}
    
void TemperatureEndpoint::addTemperatureReader() {
    LOG("Adding a temperature reader");
    
    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (!json.success()) {
            ERROR("Failed to parse JSON data");
            _server->send(400, "text/plain", "Failed to parse JSON data");
        
        } else {
            // Build new temperature reader
            TemperatureReaderConfig readerConfig(json);
            TemperatureReader * temperatureReader = TemperatureReaderService::_()->add(readerConfig);

            // Send json response
            _server->send(200, "application/json", JsonStringBuilder::jsonString(temperatureReader).c_str());
        }

    } else {
        ERROR("Body not received");
        _server->send(400, "text/plain", "Body not received");
        return;
    }
}
    
void TemperatureEndpoint::getTemperature() {
    LOG("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    _server->send(200, "text/plain", message);
}