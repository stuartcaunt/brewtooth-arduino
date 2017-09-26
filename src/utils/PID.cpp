
#include "PID.h"
#include <arduino.h>

PID::PID(float * input, float * output, float * setpoint, float Kp, float Ki, float Kd) :
    _output(output),
    _input(input),
    _setpoint(setpoint),
    _inAuto(false),
    _sampleTime(100) {
	
	PID::setOutputLimits(0, 255); //default output limit corresponds to the arduino pwm limits
    PID::setTunings(Kp, Ki, Kd);

    _lastTime = millis() - _sampleTime;				
}
 
bool PID::compute() {
    if (!_inAuto) {
        return false;
    }

    unsigned long now = millis();
    unsigned long timeChange = (now - _lastTime);

    if (timeChange >= _sampleTime) {
        float input = * _input;
        float error = * _setpoint - input;
        _iTerm += (_ki * error);

        if (_iTerm > _outMax) {
            _iTerm = _outMax;
        
        } else if (_iTerm < _outMin) {
            _iTerm = _outMin;
        }

        _lastInputs.push_back(input);
        while (_lastInputs.size() > 5) {
            _lastInputs.pop_front();
        }
        float dInput = this->calculateDerivative();
    
        Serial.println("kp = " + String(_kp * error) + ", ki = " + String(_iTerm) + ", kd = " + String(-_kd * dInput));

        float output = _kp * error + _iTerm - _kd * dInput;
        
        if(output > _outMax) {
            output = _outMax;
        
        } else if (output < _outMin) {
            output = _outMin;
        }
        
        *_output = output;
    

        _lastTime = now;

        return true;
   
    } else {
       return false;
   }
}

float PID::calculateDerivative() const {
    int nPoints = _lastInputs.size();
    if (nPoints == 2) {
        return _lastInputs[1] - _lastInputs[0];
    
    } else if (nPoints == 3) {
        return 1.5 * _lastInputs[2] - 2.0 * _lastInputs[1] + 0.5 * _lastInputs[0];
    
    } else if (nPoints == 4) {
        return 1.833333 * _lastInputs[3] - 3.0 * _lastInputs[2] + 1.5 * _lastInputs[1] - 0.333333 * _lastInputs[0];
    
    } else if (nPoints == 5) {
        return 2.083333 * _lastInputs[4] - 4.0 * _lastInputs[3] + 3.0 * _lastInputs[2] - 1.333333 * _lastInputs[1] + 0.25 * _lastInputs[0];

    } else {
        return 0.0;
    }
}

void PID::setTunings(float kp, float ki, float kd) {
    if (kp < 0.0 || ki < 0.0 || kd < 0.0) {
        return;
    }
 
    _dispKp = kp; 
    _dispKi = ki; 
    _dispKd = kd;
   
    float sampleTimeInSec = ((float)_sampleTime) / 1000.0;  
   
    _kp = kp;
    _ki = ki * sampleTimeInSec;
    _kd = kd / sampleTimeInSec;
}
  
void PID::setSampleTime(int newSampleTime) {
    if (newSampleTime > 0) {
        float ratio  = (float)newSampleTime / (float)_sampleTime;
        _ki *= ratio;
        _kd /= ratio;
        _sampleTime = (unsigned long)newSampleTime;
    }
}
 
void PID::setOutputLimits(float min, float max) {
    if (min >= max) {
        return;
    }
    _outMin = min;
    _outMax = max;
 
    if (_inAuto) {
	    if (*_output > _outMax) {
            *_output = _outMax;
        
        } else if (*_output < _outMin) {
            *_output = _outMin;
        }
	 
	    if (_iTerm > _outMax) {
            _iTerm= _outMax;
        } else if (_iTerm < _outMin) {
            _iTerm= _outMin;
        }
    }
}

void PID::setMode(int Mode) {
    bool newAuto = (Mode == AUTOMATIC);
    
    if (newAuto == !_inAuto) {
        this->initialize();
    }

    _inAuto = newAuto;
}
 
void PID::initialize() {
   _iTerm = *_output;
   _lastInputs.clear();
   _lastInputs.push_back(*_input);
   
   if (_iTerm > _outMax) {
       _iTerm = _outMax;
   } else if (_iTerm < _outMin) {
       _iTerm = _outMin;
   }
}

