#include "MashController.h"
#include "ThermometerWire.h"
#include "ThermometerWireData.h"
#include "StateHistory.h"
#include "Relay.h"
#include <service/GPIOService.h>
#include <utils/Log.h>
#include <utils/PID.h>
#include <utils/PIDAutoTune.h>

MashController::MashController(const MashControllerConfig & config) :
    _config(config),
    _heater(NULL),
    _agitator(NULL),
    _temperatureController(NULL),
    _autoTune(NULL),
    _isAutoTuning(false),
    _lastHistoryWriteTimeS(0.0),
    _historyWritePeriodS(10.0),
    _historyFileName("StateHistory-" + String(config.id) + ".json"),
    _isFirstWrite(true) {

    _windowStartTimeMs = _lastTimeMs = millis();
    _state.currentTimeS = 0.0;
    _state.runTimeS = 0.0;
    _state.outputMax = _config.pidParams.outputMax;
    _state.kp = _config.pidParams.kp;
    _state.ki = _config.pidParams.ki;
    _state.kd = _config.pidParams.kd;
    _state.windowSizeMs = _config.windowSizeMs;
    _state.sampleTimeMs = WINDOW_SAMPLE_TIME_RATION * _config.windowSizeMs;

    _historyFile = SPIFFS.open(_historyFileName, "w");
}

MashController::~MashController() {
    this->deleteHeater();
    this->deleteAgitator();

    if (_temperatureController != NULL) {
        delete _temperatureController;
        _temperatureController = NULL;
    }

    if (_autoTune != NULL) {
        delete _autoTune;
        _autoTune = NULL;
    }

    _historyFile.close();
}

void MashController::setHeater(Relay * heater) {
    this->deleteHeater();
    _heater = heater;

    if (_heater != NULL) {
        
        // Try to get GPIO 
        _heater->setPortIsValid(GPIOService::_()->acquire(_heater->getPort()));
        if (_heater->getPortIsValid()) {
            GPIOService::_()->setPinMode(_heater->getPort(), OUTPUT);
    
            // Make inactive initially
            _heater->setActive(false);
            
        } else {
            WARN("Heater for Mash Controller \"%s\" has an invalid GPIO port %u", this->getName().c_str(), _heater->getPort());
        }

        // Update config
        _config.heater = *(_heater->getConfig());

        // Update state
        _state.heaterActive = _heater->isActive();
    }
}

void MashController::deleteHeater() {
    if (_heater != NULL) {
        // Release GPIO
        _heater->setActive(false);
        GPIOService::_()->release(_heater->getPort());

        delete _heater;
        _heater = NULL;
    }
}

bool MashController::setHeaterActive(bool active) {
    if (_heater != NULL && _heater->isActive() != active) {
        LOG("Setting heater active %s", active ? "true" : "false");
        _state.heaterActive = active;
        return _heater->setActive(active);
    }

    return false;
}

void MashController::setAgitator(Relay * agitator) {
    this->deleteAgitator();
    _agitator = agitator;

    if (_agitator != NULL) {
        
        // Try to get GPIO 
        _agitator->setPortIsValid(GPIOService::_()->acquire(_agitator->getPort()));
        if (_agitator->getPortIsValid()) {
            GPIOService::_()->setPinMode(_agitator->getPort(), OUTPUT);

            // Make inactive initially
            _agitator->setActive(false);

        } else {
            WARN("Agitator for Mash Controller \"%s\" has an invalid GPIO port %u", this->getName().c_str(), _agitator->getPort());
        }

        // Update config
        _config.agitator = *(_agitator->getConfig());

        // Update state
        _state.agitatorActive = _agitator->isActive();
    }
}

void MashController::deleteAgitator() {
    if (this->_agitator != NULL) {
        // Release GPIO
        _agitator->setActive(false);
        GPIOService::_()->release(_agitator->getPort());

        delete _agitator;
        _agitator = NULL;
    }
}

bool MashController::setAgitatorActive(bool active) {
    if (_agitator != NULL && _agitator->isActive() != active) {
        LOG("Setting agitator active %s", active ? "true" : "false");
        _state.agitatorActive = active;
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
        _temperatureController->setTunings(kp, ki, kd);
    }
}

void MashController::setOutputMax(int outputMax) {
    _config.pidParams.outputMax = _state.outputMax = outputMax;

    // Modify output limits in temperature controller
    if (_temperatureController != NULL) {
        _temperatureController->setOutputLimits(0.0, outputMax);
    }
}

void MashController::setPIDParams(const PIDParams & pidParams) {
    this->setOutputMax(pidParams.outputMax);
    this->setTunings(pidParams.kp, pidParams.ki, pidParams.kd);
}

void MashController::startTemperatureControl(ControlType controlType) {
    if (_state.autoTuning) {
        LOG("Cannot start temperature control when autotuning is in progress");
        return;
    }

    if (_temperatureController != NULL) {
        LOG("Temperature control is already active");
        return;
    }

    if (_heater != NULL) {
        // create temperature controller
        LOG("Starting temperature control with control type %s", toString(controlType).c_str());
        _state.setControlType(controlType);
        _state.controllerOutput = 0.0;
        _temperatureController = new PID(&_state.temperatureC, &_state.controllerOutput, &_state.setpointC, _config.pidParams.kp, _config.pidParams.ki, _config.pidParams.kd);
        _temperatureController->setOutputLimits(0.0, _config.pidParams.outputMax);
        this->setAutoTemperatureControl(true);

        _state.running = true;
        _startTimeMs = millis();
        _state.runTimeS = 0.0;

        // start the profile
        if (controlType == ControlType::Profile) {
            _state.temperatureC = _state.temperatureProfile.start(0.001 * _startTimeMs, _state.temperatureC);
        }

        _temperatureController->setSampleTime(_state.sampleTimeMs);

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

    this->setHeaterActive(false);
    this->setAutoTemperatureControl(false);
    _state.running = false;
    _state.runTimeS = 0.0;

    // stop the profile
    if (_state.controlType == ControlType::Profile) {
        _state.temperatureProfile.stop();
    }
}

void MashController::setAutoTemperatureControl(bool isAuto) {
    _state.autoControl = isAuto;

    // Set temperature controller to auto or manual
    if (_temperatureController != NULL) {
        _temperatureController->setMode(isAuto ? AUTOMATIC : MANUAL);
    }
}

void MashController::startAutoTune() {
    if (_state.running) {
        LOG("Cannot run autotune when temperature control is active");
    }

    if (_temperatureController != NULL) {
        LOG("Auto tunining is already active");
        return;
    }

    if (_heater != NULL) {

        // create temperature controller
        LOG("Creating new temperature controller for autotuning");
        // Set output to half max
        _state.controllerOutput = 0.5 * _config.pidParams.outputMax;
        
        this->setTunings(2.0, 0.5, 2.0);

        _temperatureController = new PID(&_state.temperatureC, &_state.controllerOutput, &_state.setpointC, _config.pidParams.kp, _config.pidParams.ki, _config.pidParams.kd);
        _temperatureController->setOutputLimits(0.0, _config.pidParams.outputMax);
        _temperatureController->setMode(AUTOMATIC);
        
        _state.autoTuning = true;
        _startTimeMs = millis();
        _state.runTimeS = 0.0;
        _state.controlType = ControlType::Setpoint;
        _temperatureController->setSampleTime(_state.sampleTimeMs);

        // Create auto tuner
        _autoTune = new PIDAutoTune(&_state.temperatureC, &_state.controllerOutput);
        _autoTune->setControlType(1); // PID (not PI)
        _autoTune->setNoiseBand(0.1); // Noise on line : ignore noise less than this
        _autoTune->setOutputStep(0.5 * _config.pidParams.outputMax); // Step above and below initital output value
        _autoTune->setLookbackSec(30); // How far to look back

    } else {
        LOG("Cannot start autotune since the heater is not configured");
    }
}

void MashController::stopAutoTune() {
    // delete autotuner and temperature controller
    if (_autoTune != NULL) {
        LOG("Stopping/deleting autotuner and temperature controller");
        _state.controllerOutput = 0.0;
    
        delete _autoTune;
        _autoTune = NULL;
        delete _temperatureController;
        _temperatureController = NULL;
    }

    this->setHeaterActive(false);
    _state.autoTuning = false;
    _state.runTimeS = 0.0;
}

void MashController::startTemperatureControlProfileLevel() {
    if (_state.running && _state.controlType == ControlType::Profile) {
        unsigned long timeMs = millis();
        float currentTimeS = 0.001 * timeMs;
    
        _state.temperatureProfile.startPendingLevel(currentTimeS, _state.temperatureC);
    }
}

void MashController::skipTemperatureControlProfileLevel() {
    if (_state.running && _state.controlType == ControlType::Profile) {
        unsigned long timeMs = millis();
        float currentTimeS = 0.001 * timeMs;

        _state.temperatureProfile.terminateCurrentLevel(currentTimeS, _state.temperatureC);
    }
}

void MashController::update() {
    // Update loop timing
    unsigned long timeMs = millis();
    _state.loopMs = timeMs - _lastTimeMs;
    _lastTimeMs = timeMs;

    _state.currentTimeS = 0.001 * timeMs;

    // Get mean temperature from thermometer wires
    _state.temperatureC = this->getMeanTemperatureC();
    DEBUG("Got average temperature of %d with %d valid thermometerWires", (int)_state.temperatureC, count);

    // Update temperate control
    if (_temperatureController != NULL) {

        // Shift relay window
        while (timeMs - _windowStartTimeMs > _config.windowSizeMs) {
            _windowStartTimeMs += _config.windowSizeMs;
        }
        
        if (_state.running) {
            // Update the profile
            if (_state.controlType == ControlType::Profile) {
                _state.setpointC = _state.temperatureProfile.update(0.001 * timeMs, _state.temperatureC);
            }

            _temperatureController->compute();

        } else if (_state.autoTuning) {
            // Update the autotune
            int  retVal = _autoTune->runtime();
            if (retVal != 0) {
                // Finished
                float kp = _autoTune->getKp();
                float ki = _autoTune->getKi();
                float kd = _autoTune->getKd();

                LOG("Finished autotuning");
                
                this->setTunings(kp, ki, kd);

                // Stop autotune
                _autoTune->cancel();

                // And let`s kill it too
                this->stopAutoTune();
            }
        }

        // Update runtime
        _state.runTimeS = float(timeMs - _startTimeMs) * 0.001;
        _state.windowSizeMs = _config.windowSizeMs;
    
        // Activate heater depending on controller output
        float windowFactor = float(timeMs - _windowStartTimeMs) / _config.windowSizeMs;
        float outputFactor = float(_state.controllerOutput) / _config.pidParams.outputMax;

        bool activeHeater = outputFactor >= windowFactor;
        if (this->isHeaterActive() != activeHeater && _state.autoControl) {
            LOG("%d : Changing heater state to %s, wf = %d, of = %d", (int)_state.runTimeS, activeHeater ? "active" : "inactive", (int)(windowFactor * 100), (int)(outputFactor * 100));
            this->setHeaterActive(activeHeater);
        }
    }

    // Write data to file
    this->writeHistoryToFile();
}

void MashController::writeHistoryToFile() {

    if (_lastHistoryWriteTimeS + _historyWritePeriodS < _state.currentTimeS && (int)_state.currentTimeS % (int)_historyWritePeriodS == 0) {
        // Create history
        StateHistory history(_state);

        if (_isFirstWrite) {
            _historyFile.println("[");
            _isFirstWrite = false;
        
        } else {
            // Go back one character
            _historyFile.seek(-1, SeekCur);
            _historyFile.println(",");
        }
        // Convert to json
        DynamicJsonBuffer jsonBuffer;
        JsonObject & json = jsonBuffer.createObject();
        history.convertToJson(json);
    
        // Write to file
        json.printTo(_historyFile);
        
        _historyFile.print("\n]");
        
        // Update write time
        _lastHistoryWriteTimeS = _state.currentTimeS;
    }
}

