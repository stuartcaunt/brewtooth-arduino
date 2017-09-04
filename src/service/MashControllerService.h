#ifndef MASHCONTROLLERSERVICE_H
#define MASHCONTROLLERSERVICE_H

#include <model/MashController.h>
#include <map>

class MashControllerService {
public:
    virtual ~MashControllerService();

    static void init();
    static MashControllerService * _();

    void add(const MashControllerConfig & mashControllerConfig, bool save = true);
    void update(const MashControllerConfig & mashControllerConfig);
    MashController * get(unsigned int id) const;
    void erase(unsigned int id);

private:
    MashControllerService();
    static MashControllerService * instance;

    void addTemperatureReaders(MashController * mashController, const std::vector<unsigned int> & temperatureReaderIds);
    void createDefaultMashController();
    void save();

private:
    std::map<unsigned int, MashController *> _mashControllers;
};

#endif /*MASHCONTROLLERSERVICE_H*/