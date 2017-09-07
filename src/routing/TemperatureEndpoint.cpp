#include "TemperatureEndpoint.h"
#include <service/TemperatureReaderService.h>
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>
#include <utils/JsonStringBuilder.h>

void TemperatureEndpoint::buildPaths() {
    LOG("Building paths for TemperatureEndpoint");
    using namespace std::placeholders;
    
    _server->on("/thermometers", HTTPMethod::HTTP_POST, std::bind(&TemperatureEndpoint::addTemperatureReader, this));
    _server->on("/thermometers", HTTPMethod::HTTP_GET, std::bind(&TemperatureEndpoint::getTemperatureReaders, this));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_GET, std::bind(&TemperatureEndpoint::getTemperatureReader, this, _1));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_DELETE, std::bind(&TemperatureEndpoint::deleteTemperatureReader, this, _1));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_PUT, std::bind(&TemperatureEndpoint::updateTemperatureReader, this, _1));

    _server->on("/temperature", std::bind(&TemperatureEndpoint::getTemperature, this));
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
        WARN("Cannote get temperature reader : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Temperature reader not found");
    }
}

void TemperatureEndpoint::updateTemperatureReader(int id) {
    LOG("Updating temperature reader id = %d", id);

    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build new temperature reader
            TemperatureReaderConfig readerConfig(json);
            
            if (readerConfig.id == id) {
                TemperatureReader * temperatureReader = TemperatureReaderService::_()->update(readerConfig);
    
                // Send json response
                _server->send(200, "application/json", JsonStringBuilder::jsonString(temperatureReader).c_str());

            } else {
                ERROR("The Id of the thermometer to update (%d) does not match the Id in the path (%d)", readerConfig.id, id);
                _server->send(400, "text/plain", String("The Id of the thermometer to update (") + readerConfig.id + String("does not match the Id in the path (") + id + String(")"));
            }

        } else {
            ERROR("Failed to parse JSON data");
            _server->send(400, "text/plain", "Failed to parse JSON data");
        }

    } else {
        ERROR("Body not received");
        _server->send(400, "text/plain", "Body not received");
        return;
    }

}

void TemperatureEndpoint::deleteTemperatureReader(int id) {
    LOG("Deleting temperature reader id = %d", id);

    if (TemperatureReaderService::_()->erase(id)) {
        // deleted
        _server->send(200);
    } else {
        // could not find
        WARN("Cannot delete temperature reader : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Temperature reader not deleted : not found");
    }
}
    
void TemperatureEndpoint::getTemperature() {
    LOG("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    _server->send(200, "text/plain", message);
}