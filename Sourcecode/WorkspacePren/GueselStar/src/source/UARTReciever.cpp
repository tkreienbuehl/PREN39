#include "../header/UARTReciever.hpp"

UARTReciever::UARTReciever(UARTHandler* handler, PrenController* controller) {
	m_EngineSpeed = 0;
	m_FlexDistance = 0;
	m_UltraDist = 0;
	m_uart0_filestream = handler->getFileStream();
	m_active = false;
	m_startCmd = "StartFrd";
	m_ultraCmd = "Ul";
	m_camCmd = "CamP";
	m_debug = "DBG";
	m_flex1Cmd = "Fld1";
	m_ContDoneCmd = "StAf";
	m_unloadDoneCmd = "StEf";
	m_DCengCmd = "DCDr";
	m_Controller = controller;
	m_Logger = UARTReceiverLogger::getInstance("uartReceiveLog.log");
}

UARTReciever::~UARTReciever() {
	UARTReceiverLogger::releaseInstance();
}

void UARTReciever::startRecording() {
	m_active = true;
	readAndPlotData();
}

void UARTReciever::stopReading() {
	m_active = false;
	cout << "Stop command received" << endl;
}

void UARTReciever::readAndPlotData() {

	unsigned char rx_buffer[512];
	while(m_active) {
			//----- CHECK FOR ANY RX BYTES -----
		if (m_uart0_filestream != -1) {
			bzero(rx_buffer,sizeof(rx_buffer));
			int rx_length = read(m_uart0_filestream, rx_buffer, sizeof(rx_buffer));
			if (rx_length > 0 ) {
				string str(reinterpret_cast<const char*>(rx_buffer));
				if (str.length() > 0) {
					m_Logger->logMessage(str, m_Logger->RECEIVER);
					decodeRecievedString(str);
				}
			}
		}
	}
	m_Controller->printString("UART-Recorder stopped", m_Controller->UART_COMM, 1);
}

void UARTReciever::decodeRecievedString(std::string message) {
	if (message.find("*** Failed") != std::string::npos) {
		decodeUnknownCommandError(message);
	} else if (message.find("Ul") != std::string::npos) {
		decodeUltraValue(message);
	} else if(message.find("Fld1") != std::string::npos) {
		decodeFlexValue(message);
	} else if(message.find("StAf") != std::string::npos) {
		m_Controller->setContainerLoadingFinished(true);
	} else if(message.find("There") != std::string::npos) {
		decodeStillThere(message);
	} else if(message.find("Stop") != std::string::npos) {
		m_Controller->setState(m_Controller->END);
		m_active = false;
	}
}

void UARTReciever::decodeUnknownCommandError(std::string message) {
	int start = message.find("*** Failed");
	message.replace(start, 4 , "");
	m_Controller->printString(message, m_Controller->UART_COMM, 1);
}

void UARTReciever::decodeUltraValue(std::string message) {
	int start = message.find("Ul");
	message.replace(start, 3, "");
	int value = atoi(message.c_str());
	m_UltraDist = value;
	m_Controller->checkUltraDist(value);
}

void UARTReciever::decodeFlexValue(std::string message) {
	int start = message.find("Fld1");
	message.replace(start, 5, "");
	int value = atoi(message.c_str());
	m_FlexDistance = value;
	m_Controller->setFlexValue(value);
}

void UARTReciever::decodeStillThere(std::string message) {
	m_Controller->responseStillThere();
}

void* UARTReciever::staticEntryPoint(void* threadId) {
	((UARTReciever*)threadId)->startRecording();
	cout << "Thread UARTReceiver ended" << endl;
	pthread_exit(threadId);
}

uint8_t UARTReciever::getFlexDistance() {
	return m_FlexDistance;
}
uint8_t UARTReciever::getEngineSpeed() {
	return 0;
}
uint16_t UARTReciever::getUltraDist() {
	return m_UltraDist;
}
