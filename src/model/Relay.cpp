#include "Relay.h"
#include <utils/Log.h>

bool Relay::setActive(bool active) {
    if (this->isEnabled() && this->getPortIsValid()) {
        if (_config.active != active) {
            LOG("%s relay with port %u", active ? "Activating" : "Deactivating", this->getPort());
    
            digitalWrite(this->getPort(), active ? 1 : 0);
            _config.active = active;
        }
    
        return true;
    }

    LOG("Cannot activate/deactivate relay with port %u: enabled = %s, portValid = %s", this->getPort(), this->isEnabled() ? "true" : "false", this->getPortIsValid() ? "true" : "false");
    return false;
}
