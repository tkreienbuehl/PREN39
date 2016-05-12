#include "../header/UARTReciever.hpp"

UARTReciever::UARTReciever(UARTHandler* handler) {
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
}

UARTReciever::~UARTReciever() {
}

void UARTReciever::startRecording() {
	m_active = true;
	readAndPlotData();
}

void UARTReciever::stopReading() {
	m_active = false;
	cout << "Stop command recieved" << endl;
}

void UARTReciever::readAndPlotData() {

	while(m_active) {
			//----- CHECK FOR ANY RX BYTES -----
		//cout << "reading..." << endl;
		if (m_uart0_filestream != -1) {
			unsigned char rx_buffer[256];
			unsigned char rchar[1];
			int i = 0;
			rchar[0] = 'x';
			for (int i = 0; i<256; i++) {
				rx_buffer[i] = 'x';
			}
			while (rchar[0] != '\n') {
				if (i== 255) {
					i=0;
				}
				int rx_length = read(m_uart0_filestream, (void*)rchar, 1);		//Filestream, buffer to store in, number of bytes to read (max)
				if (rx_length < 0) {
					//An error occured (will occur if there are no bytes)
				}
				else if (rx_length == 0) {
					//No data waiting
				}
				else {
					//Bytes received
					if (rchar[0] != '\r' && rchar[0] != '\n') {
						rx_buffer[i] = rchar[0];
						i++;
					}

				}
			}
			if (i > 0) {
				rx_buffer[i] = '\0';
				std::string str(reinterpret_cast<const char*>(rx_buffer));
				decodeRecievedString(str);
			}
		}
	}
	cout << "UART-Recorder stopped" << endl;
}

void UARTReciever::decodeRecievedString(std::string message) {
	if (message.find("*** Failed") != std::string::npos) {
		decodeUnknownCommandError(message);
	} else if (message.find("Ul") != std::string::npos) {
		decodeUltraValue(message);
	} else if(message.find("Fld1") != std::string::npos) {
		decodeFlexValue(message);
	}
	// TODO: EngineSpeed
}

void UARTReciever::decodeUnknownCommandError(std::string message) {
	int start = message.find("*** Failed");
	message.replace(start, 4 , "");
	cout << message << endl;
}

void UARTReciever::decodeUltraValue(std::string message) {
	int start = message.find("Ul");
	message.replace(start, 3, "");
	int value = atoi(message.c_str());
	m_UltraDist = value;
	cout << message << endl;
}

void UARTReciever::decodeFlexValue(std::string message) {
	int start = message.find("Fld1");
	message.replace(start, 5, "");
	int value = atoi(message.c_str());
	m_FlexDistance = value;
	cout << message << endl;
}

void* UARTReciever::staticEntryPoint(void* threadId) {
	((UARTReciever*)threadId)->startRecording();
	cout << "Thread ended " << endl;
	return NULL;
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
