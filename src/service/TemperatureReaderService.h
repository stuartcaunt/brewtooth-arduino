class TemperatureReaderService {
public:
    virtual ~TemperatureReaderService();

    static TemperatureReaderService * _();
    
    float getTemperature(unsigned int readerId);
    void addReader(const TemperatureReaderConfig & readerConfig);
    const TemperatureReaderConfig & getReaderConfig() const;

private:
    TemperatureReaderService();
    static TemperatureReaderService * instance;

private:
    
};

