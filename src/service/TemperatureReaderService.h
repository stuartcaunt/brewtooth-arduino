#ifndef TEMPERATUREREADERSERVICE_H
#define TEMPERATUREREADERSERVICE_H

#include <model/TemperatureReader.h>
#include <map>
#include <vector>

class TemperatureReaderService {
public:
    virtual ~TemperatureReaderService();

    static void init();
    static TemperatureReaderService * _();
    
    void add(const TemperatureReaderConfig & readerConfig, bool save = true);
    void update(const TemperatureReaderConfig & readerConfig);
    const TemperatureReaderConfig * get(unsigned int id) const;
    void erase(unsigned int id);

    float getTemperature(unsigned int id) const;
    float getMeanTemperature() const;

    void readTemperatures();
    
private:
    TemperatureReaderService();
    static TemperatureReaderService * instance;

    void createDefaultTemperatureReader();
    void save();

private:
    std::map<unsigned int, TemperatureReader *> _temperatureReaders;
    
};

#endif /*TEMPERATUREREADERSERVICE_H*/
