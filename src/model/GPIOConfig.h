#ifndef GPIOCONFIG_H
#define GPIOCONFIG_H

#include <stdint.h>

enum GPIOPinState {
    Input,
    Output,
    Unknown
};

struct GPIOPinConfig {
    uint8_t port;
    bool available;
    bool used;
    GPIOPinState state;
};

#define NUMBER_OF_GPIO_PINS 17

struct GPIOConfig {
    GPIOConfig() {
        for (uint8_t i = 0; i < NUMBER_OF_GPIO_PINS; i++) {
            pins[i].port = i;
            pins[i].available = (i != 6 && i != 7 && i != 8 && i != 11);
            pins[i].used = false;
            pins[i].state = GPIOPinState::Unknown;
        }
    }
    GPIOPinConfig pins[NUMBER_OF_GPIO_PINS];
};

#endif /*GPIOCONFIG_H*/