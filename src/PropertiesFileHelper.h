#ifndef PROPERTIES_H
#define PROPERTIES_H

class Properties {
public:
    virtual ~Properties() {}

    static Properties * getInstance();
    void init();

private:
    Properties() {}
    static Properties * _instance;
};

#endif /*PROPERTIES_H*/