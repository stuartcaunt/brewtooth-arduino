#ifndef APPLICATIONSTATE
#define APPLICATIONSTATE

class ApplicationState {
public:
    ApplicationState() {}
    virtual ~ApplicationState() {}

    virtual void setup() = 0;
    virtual void loop() = 0;
    
};

#endif /*APPLICATIONSTATE*/