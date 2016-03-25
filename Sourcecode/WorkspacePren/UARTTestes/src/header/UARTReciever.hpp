#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;

class UARTReciever {
public:
	UARTReciever(int uart0_filestream);
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

	int m_uart0_filestream;
	bool m_active;
	int m_FlexDistance, m_EngineSpeed, m_UltraDist;

};
