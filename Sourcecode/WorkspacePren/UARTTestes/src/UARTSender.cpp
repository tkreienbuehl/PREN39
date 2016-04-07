#include "./header/UARTSender.hpp"

UARTSender::UARTSender(UARTHandler* handler) {
	m_uart0_filestream = handler->getFileStream();
}

UARTSender::~UARTSender() {

}

void UARTSender::sendStartCmd() {

}
void UARTSender::sendStopCmd() {

}
void UARTSender::setCameraPos(CameraStatesE pos) {

}

void UARTSender::setEngineSpeed(int speed, EngineModesE mode) {

}

void UARTSender::setContainerFound(int distance) {

}

void UARTSender::setTargetFieldFound(int distance) {

}

//To test UART
void UARTSender::blinkLed(int on) {

	if (m_uart0_filestream != -1)
	{

		if (on==1) {
			unsigned char str[] = "LED1 on\r\n\0";;
			int check = sizeof(str) / sizeof(char);

			writeStreamToPort(str, check);

		}
		else {
			unsigned char str[] = "LED1 off\r\n\0";
			int check = sizeof str;

			writeStreamToPort(str, check);

		}
	}
}

bool UARTSender::writeStreamToPort(unsigned char* strPtr, int size) {

	usleep(50000);
	int count = write(m_uart0_filestream, "\r" , 1);
	if (count < 0)
	{
		cout << "UART TX error\n" << endl;
		return false;
	}

	for (int i = 0; i<= size; i++) {
		usleep(50000);
		count = write(m_uart0_filestream, &strPtr[i] , 1);
		if (count < 0)
		{
			cout << "UART TX error\n" << endl;
			return false;
		}
	}

	return true;
}
