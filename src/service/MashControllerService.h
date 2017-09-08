#ifndef MASHCONTROLLERSERVICE_H
#define MASHCONTROLLERSERVICE_H

#include <model/MashController.h>
#include <vector>

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

private:
    MashControllerService();
    static MashControllerService * instance;

    void addThermometers(MashController * mashController, const std::vector<unsigned int> & thermometerIds);
    void createDefaultMashController();
    void save();

private:
    std::vector<MashController *> _mashControllers;
};

#endif /*MASHCONTROLLERSERVICE_H*/