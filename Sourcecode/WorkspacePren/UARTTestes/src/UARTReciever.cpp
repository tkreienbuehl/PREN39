#include "./header/UARTReciever.hpp"

UARTReciever::UARTReciever(UARTHandler* handler) {
	m_EngineSpeed = 0;
	m_FlexDistance = 0;
	m_UltraDist = 0;
	m_uart0_filestream = handler->getFileStream();
	m_active = false;
}

UARTReciever::~UARTReciever() {
}

void UARTReciever::startRecording() {
	m_active = true;
	readAndPlotData();
}

void UARTReciever::stopReading() {
	m_active = false;
}

void UARTReciever::readAndPlotData() {

	while(m_active) {
			//----- CHECK FOR ANY RX BYTES -----
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
				cout << rx_buffer << endl;
			}
		}
		usleep(2000);
	}

}

void* UARTReciever::staticEntryPoint(void* threadId) {
	((UARTReciever*)threadId)->startRecording();
	cout << "Thread ended " << endl;
	return NULL;
}
