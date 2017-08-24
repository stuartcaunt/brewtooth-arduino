#include "WifiConfigurationStation.h"
#include "../utils/Configuration.h"

void WifiConfigurationStation::setup() {
    Configuration::properties.wifiData.isConfigured = true;
    Configuration::save();

}

void WifiConfigurationStation::loop() {
    
}