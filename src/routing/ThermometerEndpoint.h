#ifndef THERMOMETERENDPOINT_H
#define THERMOMETERENDPOINT_H

#include "WebServerEndpoint.h"

class ThermometerEndpoint : public WebServerEndpoint {
public:
    ThermometerEndpoint() {}
    virtual ~ThermometerEndpoint() {}

    virtual void buildPaths();
    
private:
    void addThermometer();
    void getThermometers();

    void getThermometer(int id);
    void deleteThermometer(int id);
    void updateThermometer(int id);
        
    void getThermometerTemperature(int id);
    void getMeanTemperature();
};

#endif /*THERMOMETERENDPOINT_H*/
