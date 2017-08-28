#ifndef GPIOSERVICE_H
#define GPIOSERVICE_H

#include <model/GPIOConfig.h>

class GPIOService {
public:
    virtual ~GPIOService() {}

    static GPIOService * _();

private:
    GPIOService() {}
    static GPIOService * instance;

    GPIOConfig _config;
};

#endif /*GPIOSERVICE_H*/