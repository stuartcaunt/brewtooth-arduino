#include "ThermometerWire.h"
#include <utils/Log.h>
#include <DallasTemperature.h>

void ThermometerWire::init() {
    if (_oneWire != NULL) {
        delete _oneWire;
        _oneWire = NULL;

        delete _sensors;
        _sensors = NULL;
    }

    if (this->getPortIsValid()) {
        // Create one wire
        _oneWire = new OneWire(this->getPort());

        // Create Dallas sensors
        _sensors = new DallasTemperature(_oneWire);

        // Start the library
        _sensors->begin();
        uint8_t deviceCount = _sensors->getDeviceCount();
        LOG("Found %d temperature sensors", deviceCount);

        _devicesAvailable = (deviceCount > 0);
        if (_devicesAvailable) {
            // Get all device addresses
            for (uint8_t i = 0; i < deviceCount; i++) {
                Thermometer thermometer;
                _sensors->getAddress(thermometer.deviceAddress, i);

                _data.thermometers.push_back(thermometer);
            }

        } else {
            WARN("No devices available");
        }

        // Set the resolution
        _sensors->setResolution(12);

        // Set non-blocking
        _sensors->setWaitForConversion(false);
    }
}

void ThermometerWire::readTemperature() {
    if (_devicesAvailable) {
        if (_temperatureReading) {
            if (_sensors->isConversionComplete()) {
                _temperatureReading = false;
                // Read all temperatures
                _data.meanTemperatureC = 0.0;
                for (std::vector<Thermometer>::iterator it = _data.thermometers.begin(); it != _data.thermometers.end(); it++) {
                    Thermometer & thermometer = *it;
                    thermometer.temperatureC = _sensors->getTempC(thermometer.deviceAddress);
                    _data.meanTemperatureC += thermometer.temperatureC ;
                }

                // get average temperature
                _data.meanTemperatureC = _data.meanTemperatureC / _data.thermometers.size();
                DEBUG("Temperature conversion complete. Got temperature %d", (int)_meanTemperatureC);
            
            } else {
                DEBUG("Waiting for temperature conversion");
            }
        
        } else {
            _sensors->requestTemperatures();
            DEBUG("Requesting temperature");
            _temperatureReading = true;
        }
    }
}
