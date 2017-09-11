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
        if (!_devicesAvailable) {
            WARN("No devices available");
        }

        // Get address for index 0
        _sensors->getAddress(_deviceAddress, 0);

        // Set the resolution
        _sensors->setResolution(12);

        // Set non-blocking
        _sensors->setWaitForConversion(false);
    }
}

void ThermometerWire::readTemperature() {
    if (_devicesAvailable) {
        if (_temperatureReading) {
            if (_sensors->isConversionAvailable(_deviceAddress)) {
                _temperatureReading = false;
                _temperatureC = _sensors->getTempC(_deviceAddress);
                DEBUG("Temperature conversion complete. Got temperature %d", (int)_temperatureC);
            
            } else {
                DEBUG("Waiting for temperature conversion");
            }
        
        } else {
            if (_sensors->requestTemperaturesByAddress(_deviceAddress)) {
                DEBUG("Requesting temperature");
                _temperatureReading = true;
            
            } else {
                ERROR("Cannot read temperature as device is not connected: disabling thermometer");
                _devicesAvailable = false;
            }
        }
    }
}
