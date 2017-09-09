#ifndef MASHCONTROLLERSERVICE_H
#define MASHCONTROLLERSERVICE_H

#include <model/MashControllerConfig.h>
#include <vector>

class MashController;
class Relay;

class MashControllerService {
public:
    virtual ~MashControllerService();

    static void init();
    static MashControllerService * _();

    MashController * add(const MashControllerConfig & mashControllerConfig, bool save = true);
    MashController * update(const MashControllerConfig & mashControllerConfig);
    MashController * get(unsigned int id) const;
    const std::vector<MashController *> & getAll() const;
    bool erase(unsigned int id);

    Relay * updateHeater(unsigned int mashControllerId, const RelayConfig & relayConfig);
    Relay * updateAgitator(unsigned int mashControllerId, const RelayConfig & relayConfig);
    
private:
    MashControllerService();
    static MashControllerService * instance;

    void addThermometers(MashController * mashController, const std::vector<unsigned int> & thermometerIds);

    void setHeater(MashController * mashController, const RelayConfig & heater);
    void setAgitator(MashController * mashController, const RelayConfig & agitator);

    void createDefaultMashController();

    void save();

private:
    std::vector<MashController *> _mashControllers;
};

#endif /*MASHCONTROLLERSERVICE_H*/