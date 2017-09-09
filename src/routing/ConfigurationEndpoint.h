#ifndef CONFIGURATIONENDPOINT_H
#define CONFIGURATIONENDPOINT_H

#include "WebServerEndpoint.h"

class ConfigurationEndpoint : public WebServerEndpoint {
public:
    ConfigurationEndpoint() {}
    virtual ~ConfigurationEndpoint() {}

    virtual void buildPaths();
    
private:
    void getProperties();
};

#endif /*CONFIGURATIONENDPOINT_H*/
