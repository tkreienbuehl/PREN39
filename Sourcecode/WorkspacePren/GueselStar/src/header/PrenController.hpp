#pragma once

#include <iostream>
#include <unistd.h>
#include <list>
#include "PrenConfiguration.hpp"
#include "UARTSender.hpp"
#include <string.h>
#include "ConsoleView.hpp"
#include "ObjectStateObserver.hpp"

using namespace std;

class UARTHandler;

class PrenController {

public:
	enum states {
		RUNNING = 1,
		ERROR = -1,
		CAMERA_LOST = 99,
		END = 0,
		STOPPED = -99
	};

	enum classes {
		ROUTE_FINDER,
		OBJECT_FINDER,
		UART_COMM,
		CONTROLLER
	};

	enum CameraStatesE {
		CAM_STRAIGHT = 120,
		CAM_TURN_LEFT = 170,
		CAM_TURN_RIGHT = 70,
		CAM_CHECK_STREET = 50
	};

	PrenController(UARTSender* sender, ConsoleView* viewer);
	~PrenController();

	void start(void);
	void stop(void);
	void setState(states state);
	void setContainerFound(int distance);
	void setCrossingFound(int distance);
	void setTargetFieldFound(int distance);
	void setLaneLost();
	void setSteeringAngle(int angle);
	void setEngineSpeed(uint8_t speed);
	void setVehicleInCrossing();
	bool checkObjectOnLane(void);
	int getFlexDistance(void);
	int getEngineSpeed(void);
	PrenConfiguration* getPrenConfig(void);
	ConsoleView* getConsoleView();
	void printString(string str, classes cl, uint line = 0);
	void setObjectStateObserver(ObjectStateObserver* observer);
	void setCameraPos(CameraStatesE pos);
	void responseStillThere();

private:
	void runProgram(void);
	int stopProgram(void);

	PrenConfiguration* prenConfig;
	states m_State;
	UARTSender* uartSender;
	ConsoleView* consoleView;
	ObjectStateObserver* objectStateObserver;
};
