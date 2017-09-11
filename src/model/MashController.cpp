#include "MashController.h"
#include "ThermometerWire.h"
#include "Relay.h"
#include <service/GPIOService.h>
#include <utils/Log.h>


void MashController::setHeater(Relay * heater) {
    this->deleteHeater();
    _heater = heater;

    if (_heater != NULL) {
        
        // Try to get GPIO 
        if (_heater->isEnabled()) {
            _heater->setPortIsValid(GPIOService::_()->acquire(_heater->getPort()));
            if (_heater->getPortIsValid()) {
                GPIOService::_()->setPinMode(_heater->getPort(), OUTPUT);
        
                // Make inactive initially
                _heater->setActive(false);
                
            } else {
                WARN("Heater for Mash Controller \"%s\" has an invalid GPIO port %d", this->getName().c_str(), _heater->getPort());
            }
        }

        // Update config
        _config.heater = *(_heater->getConfig());
    }
}

void MashController::deleteHeater() {
    if (_heater != NULL) {
        // Release GPIO
        if (_heater->isEnabled()) {
            _heater->setActive(false);
            GPIOService::_()->release(_heater->getPort());
        }

        delete _heater;
        _heater = NULL;
    }
}

void MashController::setAgitator(Relay * agitator) {
    this->deleteAgitator();
    _agitator = agitator;

    if (_agitator != NULL) {
        
        // Try to get GPIO 
        if (_agitator->isEnabled()) {
            _agitator->setPortIsValid(GPIOService::_()->acquire(_agitator->getPort()));
            if (_agitator->getPortIsValid()) {
                GPIOService::_()->setPinMode(_agitator->getPort(), OUTPUT);

                // Make inactive initially
                _agitator->setActive(false);

            } else {
                WARN("Agitator for Mash Controller \"%s\" has an invalid GPIO port %d", this->getName().c_str(), _agitator->getPort());
            }
        }

        // Update config
        _config.agitator = *(_agitator->getConfig());
    }
}

void MashController::deleteAgitator() {
    if (this->_agitator != NULL) {
        // Release GPIO
        if (_agitator->isEnabled()) {
            _agitator->setActive(false);
            GPIOService::_()->release(_agitator->getPort());
        }

        delete _agitator;
        _agitator = NULL;
    }
}

float MashController::getTemperatureC() const {
    float temperature = 0.0;
    int count = 0;
    for (std::vector<ThermometerWire *>::const_iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        ThermometerWire * thermometerWire = *it;
        if (thermometerWire->isValid()) {
            temperature += thermometerWire->getTemperatureC();
            count++;
        }
    }

    if (count > 0) {
        temperature /= count;
    }
    LOG("Got average temperature of %d with %d valid thermometerWires", (int)temperature, count);

    return temperature;
}
