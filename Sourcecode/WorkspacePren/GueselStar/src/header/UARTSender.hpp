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

	bool sendStartCmd();
	bool sendStopCmd();
	bool setCameraPos(CameraStatesE pos);
	bool setEngineSpeed(uint8_t speed, EngineModesE mode = SOFT);
	bool setContainerFound(uint16_t distance);
	bool setTargetFieldFound(uint16_t distance);
	bool setSteering(uint8_t steeringAng);
	bool stillThereResponse(void);

	//To test UART
	void blinkLed(int on);

private:
	bool writeStreamToPort(const char* strPtr, unsigned int size);

	UARTHandler m_handler;
	int m_uart0_filestream;
	pthread_mutex_t m_mutex;

};
