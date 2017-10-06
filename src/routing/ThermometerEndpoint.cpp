#include "ThermometerEndpoint.h"
#include <model/ThermometerWire.h>
#include <service/ThermometerService.h>
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>
#include <utils/JsonStringBuilder.h>

void ThermometerEndpoint::buildPaths() {
    LOG("Building paths for ThermometerEndpoint");
    using namespace std::placeholders;
    
    // _server->on("/thermometers", HTTPMethod::HTTP_POST, std::bind(&ThermometerEndpoint::addThermometer, this));
    _server->on("/thermometers", HTTPMethod::HTTP_GET, std::bind(&ThermometerEndpoint::getThermometers, this));
    _server->on("/thermometers/temperature", std::bind(&ThermometerEndpoint::getMeanTemperature, this));
    _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_GET, std::bind(&ThermometerEndpoint::getThermometer, this, _1));
    // _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_DELETE, std::bind(&ThermometerEndpoint::deleteThermometer, this, _1));
    // _server->onPathParam<int>("/thermometers/{id}", HTTPMethod::HTTP_PUT, std::bind(&ThermometerEndpoint::updateThermometer, this, _1));

    _server->onPathParam<int>("/thermometers/{id}/temperature", HTTPMethod::HTTP_GET, std::bind(&ThermometerEndpoint::getThermometerTemperature, this, _1));
}
    
void ThermometerEndpoint::addThermometer() {
    LOG("Adding a thermometerWire");
    
    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (!json.success()) {
            ERROR("Failed to parse JSON data");
            _server->send(400, "text/plain", "Failed to parse JSON data");
        
        } else {
            // Build new thermometerWire
            ThermometerWireConfig readerConfig(json);
            ThermometerWire * thermometerWire = ThermometerService::_()->add(readerConfig);

            // Send json response
            _server->send(200, "application/json", JsonStringBuilder::jsonString(thermometerWire).c_str());
        }

    } else {
        ERROR("Body not received");
        _server->send(400, "text/plain", "Body not received");
        return;
    }
}

void ThermometerEndpoint::getThermometers() {
    LOG("Getting all thermometerWires");
    std::vector<ThermometerWire *> thermometerWires = ThermometerService::_()->getAll();

    // convert vector to create json array
    std::vector<Jsonable *> jsonables;
    jsonables.insert(jsonables.end(), thermometerWires.begin(), thermometerWires.end());

    _server->send(200, "application/json", JsonStringBuilder::jsonString(jsonables).c_str());
}

void ThermometerEndpoint::getThermometer(int id) {
    LOG("Getting thermometerWire id = %d", id);

    ThermometerWire * thermometerWire = ThermometerService::_()->get(id);
    if (thermometerWire != NULL) {
        _server->send(200, "application/json", JsonStringBuilder::jsonString(thermometerWire).c_str());

    } else {
        WARN("Cannot get thermometerWire : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Thermometer not found");
    }
}


void ThermometerEndpoint::updateThermometer(int id) {
    LOG("Updating thermometerWire id = %d", id);

    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build new thermometerWire
            ThermometerWireConfig readerConfig(json);
            
            if (readerConfig.id == id) {
                ThermometerWire * thermometerWire = ThermometerService::_()->update(readerConfig);
    
                if (thermometerWire != NULL) {
                    // Send json response
                    _server->send(200, "application/json", JsonStringBuilder::jsonString(thermometerWire).c_str());
                } else {
                    ERROR("The thermometerWire to update with Id (%d) does not exist", id);
                    _server->send(400, "text/plain", String("The thermometerWire to update with Id ") + id + String(" does not exist"));
                }

            } else {
                ERROR("The Id of the thermometerWire to update (%d) does not match the Id in the path (%d)", readerConfig.id, id);
                _server->send(400, "text/plain", String("The Id of the thermometerWire to update (") + readerConfig.id + String(") does not match the Id in the path (") + id + String(")"));
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
    LOG("Deleting thermometerWire id = %d", id);

    if (ThermometerService::_()->erase(id)) {
        // deleted
        _server->send(200);
    } else {
        // could not find
        WARN("Cannot delete thermometerWire : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Thermometer not deleted : not found");
    }
}

void ThermometerEndpoint::getThermometerTemperature(int id) {
    LOG("Getting temperature from thermometerWire id = %d", id);

    ThermometerWire * thermometerWire = ThermometerService::_()->get(id);
    if (thermometerWire != NULL) {
        const ThermometerWireData & thermometerWireData = thermometerWire->getData();

        _server->send(200, "application/json", JsonStringBuilder::jsonString(&thermometerWireData).c_str());

    } else {
        WARN("Cannot get thermometerWire : reader with Id = %d does not exist", id);
        _server->send(404, "text/plain", "Thermometer not found");
    }
}

void ThermometerEndpoint::getMeanTemperature() {
    LOG("getMeanTemperature");
    
    float meanTemperature = ThermometerService::_()->getMeanTemperatureC();
    
    String output(meanTemperature);

    _server->send(200, "text/plain", output.c_str());
}