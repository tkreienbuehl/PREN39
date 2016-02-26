#pragma once

#include <iostream>
#include <unistd.h>

using namespace std;

class PrenController {

public:
	enum states {
		RUNNING = 1,
		ERROR = -1,
		CAMERA_LOST = 99,
		END = 0,
		STOPPED = -99
	};

	PrenController(void);
	~PrenController();

	void start(void);
	void setState(states state);

private:
	void runProgram(void);
	int stopProgram(void);
	states m_State;

};
