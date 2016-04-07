#pragma once

#include "UARTHandler.hpp"

using namespace std;

class UARTSender {
public:

	UARTSender(UARTHandler* handler);
	~UARTSender();

	enum CameraStatesE {
		CAM_STRAIGHT = 0,
		CAM_CHECK_STREET = 1,
		CAM_TURN_RIGHT = 2,
		CAM_TURN_LEFT = 3
	};

	enum EngineModesE {
		SOFT = 0,
		HARD = 1
	};

	void sendStartCmd();
	void sendStopCmd();
	void setCameraPos(CameraStatesE pos);
	void setEngineSpeed(int speed, EngineModesE mode = SOFT);
	void setContainerFound(int distance);
	void setTargetFieldFound(int distance);

	//To test UART
	void blinkLed(int on);

private:
	bool writeStreamToPort(unsigned char* strPtr, int size);

	UARTHandler m_handler;
	int m_uart0_filestream;

};
