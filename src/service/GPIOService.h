#ifndef GPIOSERVICE_H
#define GPIOSERVICE_H

#include <model/GPIOConfig.h>

class GPIOService {
public:
    virtual ~GPIOService() {}

    static GPIOService * _();

    const GPIOConfig & getConfig() const {
        return _config;
    }

    bool acquire(uint8_t port);
    void release(uint8_t port);

    uint8_t getPinMode(uint8_t port);
    void setPinMode(uint8_t port, uint8_t mode);

private:
    GPIOService() {}
    static GPIOService * instance;

    GPIOConfig _config;
};

#endif /*GPIOSERVICE_H*/