#ifndef GPIOCONFIG_H
#define GPIOCONFIG_H

typedef enum GPIOPinState {
    Input,
    Output,
    Unknown
};

struct GPIOPinConfig {
    int port;
    bool available;
    bool used;
    GPIOPinState state;
};

#define NUMBER_OF_GPIO_PINS 17

struct GPIOConfig {
    GPIOConfig() {
        for (int i = 0; i < NUMBER_OF_GPIO_PINS; i++) {
            pins[i].port = i;
            pins[i].available = (i != 6 && i != 7 && i != 8 && i != 11);
            pins[i].used = false;
            pins[i].state = GPIOPinState::Unknown;
        }
    }
    GPIOPinConfig pins[17];
};

#endif /*GPIOCONFIG_H*/