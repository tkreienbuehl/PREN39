#pragma once

#include "UARTHandler.hpp"
#include "UARTLogger.hpp"

using namespace std;

class UARTSender {
public:

	UARTSender(UARTHandler* handler);
	~UARTSender();

	enum EngineModesE {
		SOFT = 0,
		HARD = 1
	};

	bool sendStartCmd();
	bool sendStopCmd();
	bool setCameraPos(ushort pos);
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
	UARTLogger* m_Logger;

};
