#ifndef WIFICONFIGURATIONSTATION_H
#define WIFICONFIGURATIONSTATION_H

#include "ApplicationState.h"

class WifiConfigurationStation : public ApplicationState {
public:
    WifiConfigurationStation() {}
    virtual ~WifiConfigurationStation() {}

    virtual void setup();
    virtual void loop();

private:
};

#endif /*WIFICONFIGURATIONSTATION_H*/