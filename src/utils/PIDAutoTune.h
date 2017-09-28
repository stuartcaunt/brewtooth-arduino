#ifndef PIDAUTOTUNE_H
#define PIDAUTOTUNE_H

class PIDAutoTune {
public:
	//commonly used functions **************************************************************************
    PIDAutoTune(float *, float *);                       	// * Constructor.  links the Autotune to a given PID
    int runtime();						   			   	// * Similar to the PID Compue function, returns non 0 when done
	void cancel();									   	// * Stops the AutoTune	
	
	// how far above and below the starting value will the output step?	
	void setOutputStep(float step) {
		_oStep = step;
	}
	
	float GetOutputStep() const {
		return _oStep;
	}
	
	// Determies if the tuning parameters returned will be PI (D=0) or PID.  (0=PI, 1=PID)	
	void setControlType(int type) {
		//0=PI, 1=PID
		_controlType = type;
	}

	int getControlType() const {
		return _controlType;
	}
	// the autotune will ignore signal chatter smaller than this value this should be acurately set
	void setNoiseBand(float band) {
		_noiseBand = band;
	}
	
	float getNoiseBand() const {
		return _noiseBand;
	}
	
	// how far back are we looking to identify peaks
	void setLookbackSec(int);

	int getLookbackSec() const {
		return _nLookBack * _sampleTime / 1000;
	}

	float getKp() const {
		return _controlType == 1 ? 0.6 * _ku : 0.4 * _ku;
	}
	
	float getKi() const {
		return _controlType == 1 ? 1.2 * _ku / _pu : 0.48 * _ku / _pu;  // Ki = Kc/Ti
	}
	
	float getKd() const {
		return _controlType == 1 ? 0.075 * _ku * _pu : 0;  //Kd = Kc * Td
	}
	
private:
	void finishUp();
	
private:
	bool _isMax;
	bool _isMin;
	float * _input;
	float * _output;
	float _setpoint;
	float _noiseBand;
	int _controlType;
	bool _running;
	unsigned long _peak1;
	unsigned long _peak2;
	unsigned long _lastTime;
	int _sampleTime;
	int _nLookBack;
	int _peakType;
	float _lastInputs[101];
    float _peaks[10];
	int _peakCount;
	bool _justchanged;
	bool _justevaled;
	float _absMax;
	float _absMin;
	float _oStep;
	float _outputStart;
	float _ku;
	float _pu;
};

#endif

