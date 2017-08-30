#ifndef TEMPERATUREREADERSERVICE_H
#define TEMPERATUREREADERSERVICE_H

#include <model/TemperatureReader.h>
#include <map>

class TemperatureReaderService {
public:
    virtual ~TemperatureReaderService();

    static TemperatureReaderService * _();
    
    void add(const TemperatureReaderConfig & readerConfig);
    void update(const TemperatureReaderConfig & readerConfig);
    const TemperatureReaderConfig & get(unsigned int id) const;
    void erase(unsigned int id);

    float getTemperature(unsigned int readerId) const;
    float getMeanTemperature() const;

    void readTemperatures();
    
private:
    TemperatureReaderService();
    static TemperatureReaderService * instance;

private:
    std::map<unsigned int, TemperatureReader *> _temperatureReaders;
    
};

#endif /*TEMPERATUREREADERSERVICE_H*/
