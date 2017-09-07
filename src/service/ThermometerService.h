#ifndef THERMOMETERSERVICE_H
#define THERMOMETERSERVICE_H

#include <model/Thermometer.h>
#include <vector>

class ThermometerService {
public:
    virtual ~ThermometerService();

    static void init();
    static ThermometerService * _();
    
    Thermometer * add(const ThermometerConfig & thermometerConfig, bool save = true);
    Thermometer * update(const ThermometerConfig & thermometerConfig);
    Thermometer * get(unsigned int id) const;
    const std::vector<Thermometer *> & getAll() const;
    bool erase(unsigned int id);

    float getTemperature(unsigned int id) const;
    float getMeanTemperature() const;

    void readTemperatures();
    
private:
    ThermometerService();
    static ThermometerService * instance;

    void createDefaultThermometer();
    void save();
        
private:
    std::vector<Thermometer *> _thermometers;
    
};

#endif /*THERMOMETERSERVICE_H*/
