#include "GPIOService.h"

GPIOService * GPIOService::instance = 0;

GPIOService * GPIOService::_() {
    if (instance == 0) {
        instance = new GPIOService();
    }

    return instance;
}
