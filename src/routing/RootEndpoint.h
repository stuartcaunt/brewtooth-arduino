#ifndef ROOTENDPOINT_H
#define ROOTENDPOINT_H

#include "WebServerEndpoint.h"

class RootEndpoint : public WebServerEndpoint {
public:
    RootEndpoint() {}
    virtual ~RootEndpoint() {}

    virtual void buildPaths();
    
private:
    void getMessage();
};

#endif /*ROOTENDPOINT_H*/
