#include "MashController.h"
#include "ThermometerWire.h"
#include "ThermometerWireData.h"
#include "Relay.h"
#include <service/GPIOService.h>
#include <utils/Log.h>
#include <PID_v1.h>

MashController::MashController(const MashControllerConfig & config) :
    _config(config),
    _heater(NULL),
    _agitator(NULL),
    _temperatureController(NULL) {

    _windowStartTimeMs = _lastTimeMs = millis();
    _state.runTimeMs = 0;
    _state.windowSizeMs = _config.pidParams.windowSizeMs;
    _state.kp = _config.pidParams.kp;
    _state.ki = _config.pidParams.ki;
    _state.kd = _config.pidParams.kd;
    _state.sampleTimeMs = 100;
}

MashController::~MashController() {
    this->deleteHeater();
    this->deleteAgitator();

    if (_temperatureController != NULL) {
        delete _temperatureController;
        _temperatureController = NULL;
    }
}

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

bool MashController::setHeaterActive(bool active) {
    if (_heater != NULL && _heater->isActive() != active) {
        LOG("Setting heater active %s", active ? "true" : "false");
        return _heater->setActive(active);
    }

    return false;
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

bool MashController::setAgitatorActive(bool active) {
    if (_agitator != NULL && _agitator->isActive() != active) {
        LOG("Setting agitator active %s", active ? "true" : "false");
        return _agitator->setActive(active);
    }

    return false;
}

ThermometerWireData MashController::getThermometerData() const {
    ThermometerWireData data;

    data.meanTemperatureC = 0.0;
    int count = 0;
    for (std::vector<ThermometerWire *>::const_iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        ThermometerWire * thermometerWire = *it;
        if (thermometerWire->isValid()) {
            const ThermometerWireData wireData = thermometerWire->getData();

            data.meanTemperatureC += wireData.meanTemperatureC;

            for (std::vector<Thermometer>::const_iterator it2 = wireData.thermometers.begin(); it2 != wireData.thermometers.end(); it2++) {
                data.thermometers.push_back(*it2);
            }

            count++;
        }
    }

    data.meanTemperatureC /= count;

    return data;
}

float MashController::getMeanTemperatureC() const {
    float temperatureC = 0.0;
    int count = 0;
    for (std::vector<ThermometerWire *>::const_iterator it = _thermometerWires.begin(); it != _thermometerWires.end(); it++) {
        ThermometerWire * thermometerWire = *it;
        if (thermometerWire->isValid()) {
            temperatureC += thermometerWire->getMeanTemperatureC();
            count++;
        }
    }

    if (count > 0) {
        temperatureC /= count;
    }

    return temperatureC;
}

void MashController::setTunings(double kp, double ki, double kd) {
    _config.pidParams.kp = _state.kp = kp;
    _config.pidParams.ki = _state.ki = ki;
    _config.pidParams.kd = _state.kd = kd;

    // Set PIDs in temperatureController
    if (_temperatureController != NULL) {
        _temperatureController->SetTunings(kp, ki, kd);
    }
}

void MashController::setWindowSizeMs(int windowSizeMs) {
    _config.pidParams.windowSizeMs = _state.windowSizeMs = windowSizeMs;

    // Modify output limits in temperature controller
    if (_temperatureController != NULL) {
        _temperatureController->SetOutputLimits(0.0, windowSizeMs);
    }
}

void MashController::setPIDParams(const PIDParams & pidParams) {
    this->setWindowSizeMs(pidParams.windowSizeMs);
    this->setTunings(pidParams.kp, pidParams.ki, pidParams.kd);
}

void MashController::startTemperatureControl() {
    if (_temperatureController != NULL) {
        LOG("Temperature control is already active");
        return;
    }

    if (_heater != NULL) {
        // create temperature controller
        LOG("Creating new temperature controller with control mode = %s", _config.autoControl ? "AUTOMATIC" : "MANUAL");
        _state.controllerOutput = 0.0;
        _temperatureController = new PID(&_state.temperatureC, &_state.controllerOutput, &_state.setpointC, _config.pidParams.kp, _config.pidParams.ki, _config.pidParams.kd, DIRECT);
        _temperatureController->SetOutputLimits(0.0, _config.pidParams.windowSizeMs);
        this->setAutoTemperatureControl(_config.autoControl);

        _state.running = true;
        _startTimeMs = millis();
        _state.runTimeMs = 0;
        _temperatureController->SetSampleTime(_state.sampleTimeMs);

    } else {
        LOG("Cannot start temperature control since the heater is not configured");
    }
}

void MashController::stopTemperatureControl() {
    // delete temperature controller
    if (_temperatureController != NULL) {
        LOG("Stopping/deleting temperature controller");
        _state.controllerOutput = 0.0;
        
        delete _temperatureController;
        _temperatureController = NULL;
    }

    _state.running = false;
}

void MashController::setAutoTemperatureControl(bool isAuto) {
    _config.autoControl = _state.autoControl = isAuto;

    // Set temperature controller to auto or manual
    if (_temperatureController != NULL) {
        _temperatureController->SetMode(isAuto ? AUTOMATIC : MANUAL);
    }
}

void MashController::update() {
    // Update loop timing
    unsigned long timeMs = millis();
    _state.loopMs = timeMs - _lastTimeMs;
    _lastTimeMs = timeMs;

    // Get mean temperature from thermometer wires
    _state.temperatureC = this->getMeanTemperatureC();
    DEBUG("Got average temperature of %d with %d valid thermometerWires", (int)_state.temperatureC, count);

    // Update temperate control
    if (_temperatureController != NULL) {
        _temperatureController->Compute();

        // Shift relay window
        if (timeMs - _windowStartTimeMs > _config.pidParams.windowSizeMs) {
            _windowStartTimeMs += _config.pidParams.windowSizeMs;
        }
        if (_config.autoControl) {
            // Activate heater depending on controller output
            bool activeHeater = (_state.controllerOutput < (timeMs - _windowStartTimeMs));
            this->setHeaterActive(activeHeater);
        }

        // Update runtime
        _state.runTimeMs = timeMs - _startTimeMs;
    }
}

