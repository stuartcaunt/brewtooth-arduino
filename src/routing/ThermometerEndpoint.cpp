#include "ThermometerEndpoint.h"
#include <service/ThermometerService.h>
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>
#include <utils/JsonStringBuilder.h>

void ThermometerEndpoint::buildPaths() {
    LOG("Building paths for ThermometerEndpoint");
    using namespace std::placeholders;
    
    _server->on("/thermometers", HTTPMethod::HTTP_POST, std::bind(&ThermometerEndpoint::addThermometer, this));
    _server->on("/thermometers", HTTPMethod::HTTP_GET, std::bind(&ThermometerEndpoint::getThermometers, this));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_GET, std::bind(&ThermometerEndpoint::getThermometer, this, _1));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_DELETE, std::bind(&ThermometerEndpoint::deleteThermometer, this, _1));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_PUT, std::bind(&ThermometerEndpoint::updateThermometer, this, _1));

    _server->on("/temperature", std::bind(&ThermometerEndpoint::getTemperature, this));
}
    
void ThermometerEndpoint::addThermometer() {
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
            ThermometerConfig readerConfig(json);
            Thermometer * thermometer = ThermometerService::_()->add(readerConfig);

            // Send json response
            _server->send(200, "application/json", JsonStringBuilder::jsonString(thermometer).c_str());
        }

    } else {
        ERROR("Body not received");
        _server->send(400, "text/plain", "Body not received");
        return;
    }
}

void ThermometerEndpoint::getThermometers() {
    LOG("Getting all temperature readers");
    std::vector<Thermometer *> thermometers = ThermometerService::_()->getAll();

    // convert vector to create json array
    std::vector<Jsonable *> jsonables;
    jsonables.insert(jsonables.end(), thermometers.begin(), thermometers.end());

    _server->send(200, "application/json", JsonStringBuilder::jsonString(jsonables).c_str());
}

void ThermometerEndpoint::getThermometer(int id) {
    LOG("Getting temperature reader id = %d", id);

    Thermometer * thermometer = ThermometerService::_()->get(id);
    if (thermometer != NULL) {
        _server->send(200, "application/json", JsonStringBuilder::jsonString(thermometer).c_str());
    } else {
        WARN("Cannote get temperature reader : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Temperature reader not found");
    }
}

void ThermometerEndpoint::updateThermometer(int id) {
    LOG("Updating temperature reader id = %d", id);

    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build new temperature reader
            ThermometerConfig readerConfig(json);
            
            if (readerConfig.id == id) {
                Thermometer * thermometer = ThermometerService::_()->update(readerConfig);
    
                if (thermometer != NULL) {
                    // Send json response
                    _server->send(200, "application/json", JsonStringBuilder::jsonString(thermometer).c_str());
                } else {
                    ERROR("The thermometer to update with Id (%d) does not exist", id);
                    _server->send(400, "text/plain", String("The thermometer to update with Id ") + id + String(" does not exist"));
                }

            } else {
                ERROR("The Id of the thermometer to update (%d) does not match the Id in the path (%d)", readerConfig.id, id);
                _server->send(400, "text/plain", String("The Id of the thermometer to update (") + readerConfig.id + String(") does not match the Id in the path (") + id + String(")"));
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

void ThermometerEndpoint::deleteThermometer(int id) {
    LOG("Deleting temperature reader id = %d", id);

    if (ThermometerService::_()->erase(id)) {
        // deleted
        _server->send(200);
    } else {
        // could not find
        WARN("Cannot delete temperature reader : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Temperature reader not deleted : not found");
    }
}
    
void ThermometerEndpoint::getTemperature() {
    LOG("getTemperature");
    
    String message = "Temperature = " + String(_temperature++);

    _server->send(200, "text/plain", message);
}