#ifndef PID_H
#define PID_H

#include <deque>

#define AUTOMATIC	1
#define MANUAL	0

class PID {
public:
    PID(float*, float*, float*,float, float, float);
    
    void setMode(int Mode); 
    bool compute();
    void setOutputLimits(float, float); 
    void setTunings(float, float, float);
    void setSampleTime(int);
										  
    float getKp() const {
        return _dispKp;
    }
    float getKi() const {
        return _dispKi;
    }
    float getKd() const {
        return _dispKd;
    }

    int getMode() const {
        return _inAuto ? AUTOMATIC : MANUAL;
    }

private:
    void initialize();
    float calculateDerivative() const;
      
    float _kp;
    float _ki;
    float _kd;

    float _dispKp;
    float _dispKi;
    float _dispKd;

    float * _input;
    float * _output;
    float * _setpoint;
			  
  	unsigned long _lastTime;
    float _iTerm;
    std::deque<float> _lastInputs;

  	unsigned long _sampleTime;
    float _outMin;
    float _outMax;
	  bool _inAuto;
};

#endif

