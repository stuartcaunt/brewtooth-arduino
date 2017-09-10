#include "MashControllerEndpoint.h"
#include <model/MashController.h>
#include <model/Relay.h>
#include <service/MashControllerService.h>
#include <utils/Configuration.h>
#include <utils/Log.h>
#include <webserver/BrewtoothWebServer.h>
#include <utils/JsonStringBuilder.h>

void MashControllerEndpoint::buildPaths() {
    LOG("Building paths for MashControllerEndpoint");
    using namespace std::placeholders;
    
    _server->on("/controllers", HTTPMethod::HTTP_POST, std::bind(&MashControllerEndpoint::addMashController, this));
    _server->on("/controllers", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getMashControllers, this));
    _server->onPathParam<int>("/controllers/{id}", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getMashController, this, _1));
    _server->onPathParam<int>("/controllers/{id}", HTTPMethod::HTTP_DELETE, std::bind(&MashControllerEndpoint::deleteMashController, this, _1));
    _server->onPathParam<int>("/controllers/{id}", HTTPMethod::HTTP_PUT, std::bind(&MashControllerEndpoint::updateMashController, this, _1));

    _server->onPathParam<int>("/controllers/{id}/temperature", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getTemperature, this, _1));

    _server->onPathParam<int>("/controllers/{id}/heater", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getHeater, this, _1));
    _server->onPathParam<int>("/controllers/{id}/heater", HTTPMethod::HTTP_PUT, std::bind(&MashControllerEndpoint::updateHeater, this, _1));
    _server->onPathParam<int>("/controllers/{id}/heater/start", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setHeaterActive, this, _1, true));
    _server->onPathParam<int>("/controllers/{id}/heater/stop", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setHeaterActive, this, _1, false));
    
    _server->onPathParam<int>("/controllers/{id}/agitator", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getAgitator, this, _1));
    _server->onPathParam<int>("/controllers/{id}/agitator", HTTPMethod::HTTP_PUT, std::bind(&MashControllerEndpoint::updateAgitator, this, _1));
    _server->onPathParam<int>("/controllers/{id}/agitator/start", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setAgitatorActive, this, _1, true));
    _server->onPathParam<int>("/controllers/{id}/agitator/stop", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setAgitatorActive, this, _1, false));
}
    
void MashControllerEndpoint::addMashController() {
    LOG("Adding a mashController");
    
    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (!json.success()) {
            ERROR("Failed to parse JSON data");
            _server->send(400, "text/plain", "Failed to parse JSON data");
        
        } else {
            // Build new mashController
            MashControllerConfig mashControllerConfig(json);
            MashController * mashController = MashControllerService::_()->add(mashControllerConfig);

            // Send json response
            _server->send(200, "application/json", JsonStringBuilder::jsonString(mashController).c_str());
        }

    } else {
        ERROR("Body not received");
        _server->send(400, "text/plain", "Body not received");
        return;
    }
}

void MashControllerEndpoint::getMashControllers() {
    LOG("Getting all controller");
    std::vector<MashController *> controller = MashControllerService::_()->getAll();

    // convert vector to create json array
    std::vector<Jsonable *> jsonables;
    jsonables.insert(jsonables.end(), controller.begin(), controller.end());

    _server->send(200, "application/json", JsonStringBuilder::jsonString(jsonables).c_str());
}

void MashControllerEndpoint::getMashController(int id) {
    LOG("Getting mashController id = %d", id);

    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        _server->send(200, "application/json", JsonStringBuilder::jsonString(mashController).c_str());
    
    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::updateMashController(int id) {
    LOG("Updating mashController id = %d", id);

    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build new mashController
            MashControllerConfig mashControllerConfig(json);
            
            if (mashControllerConfig.id == id) {
                MashController * mashController = MashControllerService::_()->update(mashControllerConfig);
    
                if (mashController != NULL) {
                    // Send json response
                    _server->send(200, "application/json", JsonStringBuilder::jsonString(mashController).c_str());
                } else {
                    ERROR("Failed to update mashController with Id %d", id);
                    _server->send(400, "text/plain", String("Failed to update mashController with Id ") + id);
                }

            } else {
                ERROR("The Id of the mashController to update (%d) does not match the Id in the path (%d)", mashControllerConfig.id, id);
                _server->send(400, "text/plain", String("The Id of the mashController to update (") + mashControllerConfig.id + String(") does not match the Id in the path (") + id + String(")"));
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

void MashControllerEndpoint::deleteMashController(int id) {
    LOG("Deleting mashController id = %d", id);

    if (MashControllerService::_()->erase(id)) {
        // deleted
        _server->send(200);
    } else {
        // could not find
        WARN("Cannot delete mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not deleted : not found");
    }
}

void MashControllerEndpoint::getTemperature(int id) {
    LOG("Getting temperature from mashController id = %d", id);

    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        String output(mashController->getTemperatureC());
        _server->send(200, "text/plain", output.c_str());


        _server->send(200, "text/plain", "OK");

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::getHeater(int id) {
    LOG("Getting heater from mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        Relay * heater = mashController->getHeater();

        _server->send(200, "application/json", JsonStringBuilder::jsonString(heater).c_str());

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::updateHeater(int id) {
    LOG("Updating heater for mashController id = %d", id);

    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build new mashController
            RelayConfig relayConfig(json);
            
            Relay * relay = MashControllerService::_()->updateHeater(id, relayConfig);

            if (relay != NULL) {
                // Send json response
                _server->send(200, "application/json", JsonStringBuilder::jsonString(relay).c_str());
            
            } else {
                ERROR("Failed to update heater for mashController with Id %d", id);
                _server->send(400, "text/plain", String("Failed to update heater for mashController with Id ") + id);
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

void MashControllerEndpoint::setHeaterActive(int id, bool active) {
    LOG("Getting heater from mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        Relay * heater = mashController->getHeater();

        if (heater->setActive(active)) {
            _server->send(200, "application/json", JsonStringBuilder::jsonString(heater).c_str());
            
        } else {
            WARN("Cannot activate/deactivate heater for mashController with Id = %d", id);
            _server->send(400, "text/plain", "Failed to activate/deactivate heater");
        }

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::getAgitator(int id) {
    LOG("Getting agitator from mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        Relay * agitator = mashController->getAgitator();

        _server->send(200, "application/json", JsonStringBuilder::jsonString(agitator).c_str());

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::updateAgitator(int id) {
    LOG("Updating agitator for mashController id = %d", id);
    
    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build new mashController
            RelayConfig relayConfig(json);
            
            Relay * relay = MashControllerService::_()->updateAgitator(id, relayConfig);

            if (relay != NULL) {
                // Send json response
                _server->send(200, "application/json", JsonStringBuilder::jsonString(relay).c_str());
            
            } else {
                ERROR("Failed to update agitator for mashController with Id %d", id);
                _server->send(400, "text/plain", String("Failed to update agitator for mashController with Id ") + id);
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
   
void MashControllerEndpoint::setAgitatorActive(int id, bool active) {
    LOG("Getting agitator from mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        Relay * agitator = mashController->getAgitator();

        if (agitator->setActive(active)) {
            _server->send(200, "application/json", JsonStringBuilder::jsonString(agitator).c_str());
            
        } else {
            WARN("Cannot activate/deactivate agitator for mashController with Id = %d", id);
            _server->send(400, "text/plain", "Failed to activate/deactivate agitator");
        }

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}