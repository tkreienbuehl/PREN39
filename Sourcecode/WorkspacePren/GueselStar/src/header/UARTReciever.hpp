#pragma once

#include "UARTHandler.hpp"
#include "PrenController.hpp"
#include "UARTReceiverLogger.hpp"

using namespace std;

class UARTReciever {
public:
	UARTReciever(UARTHandler* handler, PrenController* controller);
	~UARTReciever();

	uint8_t getFlexDistance();
	uint8_t getEngineSpeed();
	uint16_t getUltraDist();
	void stopReading();

	static void* staticEntryPoint(void* threadId);

private:
	void readAndPlotData();
	void startRecording();
	void sendDebugAlert(std::string message);
	void decodeRecievedString(std::string message);
	void decodeUnknownCommandError(std::string message);
	void decodeUltraValue(std::string message);
	void decodeFlexValue(std::string message);
	void decodeStillThere(std::string message);


	UARTHandler m_handler;
	int m_uart0_filestream;
	bool m_active;
	uint8_t m_FlexDistance, m_EngineSpeed, m_UltraDist;
	std::string m_startCmd, m_ultraCmd, m_camCmd, m_debug, m_flex1Cmd, m_ContDoneCmd;
	std::string m_unloadDoneCmd, m_DCengCmd;
	PrenController* m_Controller;
	UARTReceiverLogger* m_Logger;

};
