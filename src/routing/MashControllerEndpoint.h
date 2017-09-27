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

    void getPIDParams(int id);
    void updatePIDParams(int id);
        
    void getTemperature(int id);
    
    void getHeater(int id);
    void updateHeater(int id);
    void setHeaterActive(int id, bool active);
    
    void getAgitator(int id);
    void updateAgitator(int id);
    void setAgitatorActive(int id, bool active);

    void getTemperatureControlState(int id);
    void startTemperatureControlWithSetpoint(int id, float setpointC);
    void startTemperatureControlWithProfile(int id);
    void stopTemperatureControl(int id);
        
    void setAutoTuneActive(int id, bool active);
    void setTemperatureControlAutomatic(int id, bool automatic);
    void getTemperatureControlSetpoint(int id);
    void setTemperatureControlSetpoint(int id, float setpointC);

};

#endif /*MASHCONTROLLERENDPOINT_H*/
