#pragma once

#include "GradientMat.hpp"

class PrenController {
public:

	enum states {
		RUNNING = 1,
		ERROR = -1,
		CAMERA_LOST = 99,
		END = 0,
		STOPPED = -99
	};

	PrenController();
	~PrenController();

	void Start();
	void SetState(const states state);

private:
	void RunProgram();
	int StopProgram();
	states m_State;

};
