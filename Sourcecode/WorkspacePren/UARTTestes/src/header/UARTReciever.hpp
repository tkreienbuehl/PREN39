#pragma once

#include "UARTHandler.hpp"

using namespace std;

class UARTReciever {
public:
	UARTReciever(UARTHandler* handler);
	~UARTReciever();

	int getFlexDistance();
	int getEngineSpeed();
	int getUltraDist();
	void stopReading();

	static void* staticEntryPoint(void* threadId);

private:
	void readAndPlotData();
	void startRecording();
	void sendDebugAlert(std::string message);

	UARTHandler m_handler;
	int m_uart0_filestream;
	bool m_active;
	int m_FlexDistance, m_EngineSpeed, m_UltraDist;

};
