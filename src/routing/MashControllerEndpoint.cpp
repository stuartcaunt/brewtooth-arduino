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
    
    // _server->on("/controllers", HTTPMethod::HTTP_POST, std::bind(&MashControllerEndpoint::addMashController, this));
    _server->on("/controllers", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getMashControllers, this));
    _server->onPathParam<int>("/controllers/{id}", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getMashController, this, _1));
    // _server->onPathParam<int>("/controllers/{id}", HTTPMethod::HTTP_DELETE, std::bind(&MashControllerEndpoint::deleteMashController, this, _1));
    // _server->onPathParam<int>("/controllers/{id}", HTTPMethod::HTTP_PUT, std::bind(&MashControllerEndpoint::updateMashController, this, _1));

    _server->onPathParam<int>("/controllers/{id}/pid", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getPIDParams, this, _1));
    _server->onPathParam<int>("/controllers/{id}/pid", HTTPMethod::HTTP_POST, std::bind(&MashControllerEndpoint::updatePIDParams, this, _1));

    _server->onPathParam<int>("/controllers/{id}/temperature", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getTemperature, this, _1));

    _server->onPathParam<int>("/controllers/{id}/heater", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getHeater, this, _1));
    // _server->onPathParam<int>("/controllers/{id}/heater", HTTPMethod::HTTP_PUT, std::bind(&MashControllerEndpoint::updateHeater, this, _1));
    _server->onPathParam<int>("/controllers/{id}/heater/start", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setHeaterActive, this, _1, true));
    _server->onPathParam<int>("/controllers/{id}/heater/stop", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setHeaterActive, this, _1, false));
    
    _server->onPathParam<int>("/controllers/{id}/agitator", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getAgitator, this, _1));
    // _server->onPathParam<int>("/controllers/{id}/agitator", HTTPMethod::HTTP_PUT, std::bind(&MashControllerEndpoint::updateAgitator, this, _1));
    _server->onPathParam<int>("/controllers/{id}/agitator/start", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setAgitatorActive, this, _1, true));
    _server->onPathParam<int>("/controllers/{id}/agitator/stop", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setAgitatorActive, this, _1, false));

     _server->onPathParam<int>("/controllers/{id}/state", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getTemperatureControlState, this, _1));
    _server->onPathParam<int, float>("/controllers/{id}/start/{setpoint}", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::startTemperatureControlWithSetpoint, this, _1, _2));
    _server->onPathParam<int>("/controllers/{id}/start", HTTPMethod::HTTP_POST, std::bind(&MashControllerEndpoint::startTemperatureControlWithProfile, this, _1));
    _server->onPathParam<int>("/controllers/{id}/stop", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::stopTemperatureControl, this, _1));
    _server->onPathParam<int>("/controllers/{id}/autotune/start", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setAutoTuneActive, this, _1, true));
    _server->onPathParam<int>("/controllers/{id}/autotune/stop", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setAutoTuneActive, this, _1, false));
    _server->onPathParam<int>("/controllers/{id}/automatic", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setTemperatureControlAutomatic, this, _1, true));
    _server->onPathParam<int>("/controllers/{id}/manual", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setTemperatureControlAutomatic, this, _1, false));
    _server->onPathParam<int>("/controllers/{id}/setpoint", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::getTemperatureControlSetpoint, this, _1));
    _server->onPathParam<int, float>("/controllers/{id}/setpoint/{setpoint}", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::setTemperatureControlSetpoint, this, _1, _2));
    _server->onPathParam<int>("/controllers/{id}/profile/start", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::startTemperatureControlProfileLevel, this, _1));
    _server->onPathParam<int>("/controllers/{id}/profile/skip", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::skipTemperatureControlProfileLevel, this, _1));
    
    _server->onPathParam<int>("/controllers/{id}/history", HTTPMethod::HTTP_GET, std::bind(&MashControllerEndpoint::serveHistoryFile, this, _1));
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

void MashControllerEndpoint::getPIDParams(int id) {
    LOG("Getting PID params for mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&mashController->getPIDParams()).c_str());
    
    } else {
        WARN("Cannot get PID params for mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::updatePIDParams(int id) {
    LOG("Updating mashController id = %d", id);
    
    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            // Build PID params
            PIDParams pidParams(json);
            
            const PIDParams *  validatedParams = MashControllerService::_()->updatePIDParams(id, pidParams);
            if (validatedParams != NULL) {
                // Send json response
                _server->send(200, "application/json", JsonStringBuilder::jsonString(validatedParams).c_str());
            
            } else {
                ERROR("Failed to update PID Params of mashController with Id %d", id);
                _server->send(400, "text/plain", String("Failed to update PID params of mashController with Id ") + id);
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

void MashControllerEndpoint::getTemperature(int id) {
    LOG("Getting temperature from mashController id = %d", id);

    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        ThermometerWireData thermometerWireData = mashController->getThermometerData();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&thermometerWireData).c_str());

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
        if (mashController->setHeaterActive(active)) {
            const TemperatureControlState state = mashController->getTemperatureControlState();
            _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
            
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
        if (mashController->setAgitatorActive(active)) {
            const TemperatureControlState state = mashController->getTemperatureControlState();
            _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
            
        } else {
            WARN("Cannot activate/deactivate agitator for mashController with Id = %d", id);
            _server->send(400, "text/plain", "Failed to activate/deactivate agitator");
        }

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::getTemperatureControlState(int id) {
    LOG("Getting control state from mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot get state from mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::startTemperatureControlWithSetpoint(int id, float setpointC) {
    LOG("Starting temperature control for mashController id = %d: setpoint = %d", id, int(setpointC));
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        mashController->setSetpointC(setpointC);
        mashController->startTemperatureControl(ControlType::Setpoint);

        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot start temperature control for mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::startTemperatureControlWithProfile(int id) {
    LOG("Starting temperature control for mashController id = %d with profile", id);
    
    if (_server->hasArg("plain")) {
        String jsonText = _server->arg("plain");
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.parseObject(jsonText);

        if (json.success()) {
            MashController * mashController = MashControllerService::_()->get(id);
            if (mashController != NULL) {

                // Convert json to Temperature profile
                TemperatureProfile temperatureProfile(json);

                mashController->setTemperatureProfile(temperatureProfile);
                mashController->startTemperatureControl(ControlType::Profile);
        
                const TemperatureControlState state = mashController->getTemperatureControlState();
                _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
                
            } else {
                WARN("Cannot start temperature control for mashController : mashController with Id = %d does not exist", id);
                _server->send(404, "text/plain", "MashController not found");
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

void MashControllerEndpoint::stopTemperatureControl(int id) {
    LOG("Stopping temperature control for mashController id = %d", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        mashController->stopTemperatureControl();

        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot stop temperature control for mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::setAutoTuneActive(int id, bool active) {
    LOG("Setting auto tune active = %s for mashController id = %d", active ? "true" : "false", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        if (active) {
            mashController->startAutoTune();

        } else {
            mashController->stopAutoTune();
        }

        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot start/stop auto tune for mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::setTemperatureControlAutomatic(int id, bool automatic) {
    LOG("Setting temperature control automatic = %s for mashController id = %d", automatic ? "true" : "false", id);
    
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        mashController->setAutoTemperatureControl(automatic);

        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot start/stop automatic temperature control for  mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::getTemperatureControlSetpoint(int id) {
    LOG("Getting setpoint from mashController id = %d", id);

    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        float setpointC = mashController->getSetpointC();
        _server->send(200, "text/plain", String(setpointC).c_str());

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::setTemperatureControlSetpoint(int id, float setpointC) {
    LOG("Setting setpoint for mashController id = %d: setpoint = %d", id, (int)setpointC);

    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        mashController->setSetpointC(setpointC);
        setpointC = mashController->getSetpointC();
        _server->send(200, "text/plain", String(setpointC).c_str());

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::serveHistoryFile(int id) {
    LOG("Serving history file from mashController id = %d", id);

    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        const String & fileName = mashController->getHistoryFileName();

        _server->serveFile(fileName);

    } else {
        WARN("Cannot get mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::startTemperatureControlProfileLevel(int id) {
    LOG("start temperature profile level for mashController id = %d", id);
       
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        mashController->startTemperatureControlProfileLevel();

        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot start temperature control profile level for mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}

void MashControllerEndpoint::skipTemperatureControlProfileLevel(int id) {
    LOG("skip temperature profile level for mashController id = %d", id);
       
    MashController * mashController = MashControllerService::_()->get(id);
    if (mashController != NULL) {
        mashController->skipTemperatureControlProfileLevel();

        const TemperatureControlState state = mashController->getTemperatureControlState();
        _server->send(200, "application/json", JsonStringBuilder::jsonString(&state).c_str());
        
    } else {
        WARN("Cannot start temperature control profile level for mashController : mashController with Id = %d does not exist", id);
        _server->send(404, "text/plain", "MashController not found");
    }
}
