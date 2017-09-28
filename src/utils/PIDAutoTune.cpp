#include "PIDAutoTune.h"
#include <Arduino.h>

PIDAutoTune::PIDAutoTune(float * input, float * output) :
	_input(input),
	_output(output),
	_controlType(0), //default to PI
	_noiseBand(0.5),
	_running(false),
	_oStep(30),
	_lastTime(millis()) {

	setLookbackSec(10);
}

void PIDAutoTune::cancel() {
	_running = false;
} 
 
int PIDAutoTune::runtime() {
	_justevaled = false;
	if (_peakCount > 9 && _running) {
		_running = false;
		finishUp();
		return 1;
	}
	unsigned long now = millis();
	
	if ((now - _lastTime) < _sampleTime) {
		return false;
	}

	_lastTime = now;
	float refVal = * _input;
	_justevaled =true;
	if (!_running) { 
		//initialize working variables the first time around
		_peakType = 0;
		_peakCount = 0;
		_justchanged = false;
		_absMax = refVal;
		_absMin = refVal;
		_setpoint = refVal;
		_running = true;
		_outputStart = * _output;
		* _output = _outputStart + _oStep;
	} else {
		if (refVal < _absMin) {
			_absMin = refVal;
		}
		if (refVal > _absMax) {
			_absMax = refVal;
		}
	}
	
	//oscillate the output base on the input's relation to the setpoint
	
	if (refVal > _setpoint + _noiseBand) {
		*_output = _outputStart - _oStep;
	} else if (refVal < _setpoint - _noiseBand) {
		*_output = _outputStart + _oStep;
	}
	
  	//bool isMax=true, isMin=true;
	_isMax = true;
	_isMin = true;
	//id peaks
	for (int i = _nLookBack - 1; i >= 0; i--) {
		float val = _lastInputs[i];
		if (_isMax) {
			_isMax = refVal > val;
		}
		if (_isMin) {
			_isMin = refVal < val;
		}
		_lastInputs[i + 1] = _lastInputs[i];
	}
	_lastInputs[0] = refVal;  

	if (_nLookBack < 9) {  
		//we don't want to trust the maxes or mins until the inputs array has been filled
		return 0;
	}
	
	if (_isMax) {
		if (_peakType == 0) {
			_peakType = 1;
		}
		if (_peakType == -1) {
			_peakType = 1;
			_justchanged = true;
			_peak2 = _peak1;
		}
		_peak1 = now;
		_peaks[_peakCount] = refVal;
	
	} else if (_isMin) {
		if (_peakType == 0) {
			_peakType = -1;
		}
		if (_peakType == 1) {
			_peakType = -1;
			_peakCount++;
			_justchanged = true;
		}
		
		if (_peakCount < 10) {
			_peaks[_peakCount] = refVal;
		}
	}
	
	if (_justchanged && _peakCount > 2) { 
		//we've transitioned.  check if we can autotune based on the last peaks
		float avgSeparation = (abs(_peaks[_peakCount - 1] - _peaks[_peakCount - 2]) + abs(_peaks[_peakCount - 2] - _peaks[_peakCount - 3])) / 2;
		if (avgSeparation < 0.05 * (_absMax - _absMin)) {
			finishUp();
			_running = false;
			return 1;
		}
	}
	   
	_justchanged = false;
	return 0;
}

void PIDAutoTune::finishUp() {
	  *_output = _outputStart;
      //we can generate tuning parameters!
      _ku = 4 * (2 * _oStep) / ((_absMax - _absMin) * 3.14159);
      _pu = (float)(_peak1 - _peak2) / 1000;
}

void PIDAutoTune::setLookbackSec(int value) {
    if (value < 1) {
		value = 1;
	}
	
	if (value < 25) {
		_nLookBack = value * 4;
		_sampleTime = 250;
	
	} else {
		_nLookBack = 100;
		_sampleTime = value * 10;
	}
}

