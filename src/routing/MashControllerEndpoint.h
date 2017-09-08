#ifndef MASHCONTROLLERENDPOINT_H
#define MASHCONTROLLERENDPOINT_H

#include "WebServerEndpoint.h"

class MashControllerEndpoint : public WebServerEndpoint {
public:
    MashControllerEndpoint() {}
    virtual ~MashControllerEndpoint() {}

    virtual void buildPaths();
    
private:
    void addMashController();
    void getMashControllers();

    void getMashController(int id);
    void deleteMashController(int id);
    void updateMashController(int id);
        
    void getTemperature(int id);
};

#endif /*MASHCONTROLLERENDPOINT_H*/
