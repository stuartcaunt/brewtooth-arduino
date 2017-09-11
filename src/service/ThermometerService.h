#ifndef THERMOMETERSERVICE_H
#define THERMOMETERSERVICE_H

#include <model/ThermometerWireConfig.h>
#include <vector>

class ThermometerWire;

class ThermometerService {
public:
    virtual ~ThermometerService();

    static void init();
    static ThermometerService * _();
    
    ThermometerWire * add(const ThermometerWireConfig & thermometerWireConfig, bool save = true);
    ThermometerWire * update(const ThermometerWireConfig & thermometerWireConfig);
    ThermometerWire * get(unsigned int id) const;
    const std::vector<ThermometerWire *> & getAll() const;
    bool erase(unsigned int id);

    float getTemperatureC(unsigned int id) const;
    float getMeanTemperatureC() const;

    void readTemperatures();
    
private:
    ThermometerService();
    static ThermometerService * instance;

    void createDefaultThermometer();
    void save();
        
private:
    std::vector<ThermometerWire *> _thermometerWires;
    
};

#endif /*THERMOMETERSERVICE_H*/
