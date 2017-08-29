#include "GPIOService.h"
#include <Arduino.h>

GPIOService * GPIOService::instance = 0;

GPIOService * GPIOService::_() {
    if (instance == 0) {
        instance = new GPIOService();
    }

    return instance;
}


bool GPIOService::acquire(uint8_t port) {
    if (port < NUMBER_OF_GPIO_PINS) {
        GPIOPinConfig & config = _config.pins[port];

        if (config.available && !config.used) {
            config.used = true;
            Serial.println("Aquired GPIO port " + port);
        
            return true;

        } else {
           Serial.println("Refused aquire of GPIO port " + port + String(": available = ") + config.available + String(", used = ") + config.used); 
        }
    } else {
        Serial.println("Refused aquire of GPIO port " + port + String(": max available = ") + NUMBER_OF_GPIO_PINS) ;         
    }

    return false;
}

void GPIOService::release(uint8_t port) {
    if (port < NUMBER_OF_GPIO_PINS) {
        GPIOPinConfig & config = _config.pins[port];
        config.used = false;
    }
}


uint8_t GPIOService::getPinMode(uint8_t port) {
    if (port < NUMBER_OF_GPIO_PINS) {
        GPIOPinConfig & config = _config.pins[port];

        return config.state;
    }

    return GPIOPinState::Unknown;
}

void GPIOService::setPinMode(uint8_t port, uint8_t mode) {
    if (port < NUMBER_OF_GPIO_PINS) {
        pinMode(port, mode);

        GPIOPinConfig & config = _config.pins[port];
        config.state = (mode == OUTPUT) ? GPIOPinState::Output : GPIOPinState::Input;
    }
}
