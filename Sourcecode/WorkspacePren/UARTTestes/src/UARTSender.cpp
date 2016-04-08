#include "./header/UARTSender.hpp"

UARTSender::UARTSender(UARTHandler* handler) {
	m_uart0_filestream = handler->getFileStream();
}

UARTSender::~UARTSender() {

}

bool UARTSender::sendStartCmd() {
	return writeStreamToPort("\rStartFrd\r\n\0", 12);
}

bool UARTSender::sendStopCmd() {
	return writeStreamToPort("\rStop\r\n\0", 8);
}

bool UARTSender::setCameraPos(CameraStatesE pos) {
	char stream[10];
	int length = 0;
	length = sprintf(stream,"\rCamP p %d\r\n", pos);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setEngineSpeed(uint8_t speed, EngineModesE mode) {
	char stream[30];
	int length = 0;
	length = sprintf(stream,"\rDCDr d %d %d", speed, mode);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setContainerFound(uint16_t distance) {
	char stream[20];
	int length = 0;
	length = sprintf(stream,"\rStA d %d", distance);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setTargetFieldFound(uint16_t distance) {
	char stream[20];
	int length = 0;
	length = sprintf(stream,"\rStE d %d", distance);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setSteering(uint8_t steeringAng) {
	char stream[20];
	int length = 0;
	length = sprintf(stream,"\rLeS p %d", steeringAng);
	return writeStreamToPort(stream, length);
}

bool UARTSender::stillThereResponse(void) {
	return writeStreamToPort("\rJa\r\n\0", 6);
}

//To test UART
void UARTSender::blinkLed(int on) {

	if (m_uart0_filestream != -1)
	{

		if (on==1) {
			char str[] = "\rLED1 on\r\n\0";
			int check = sizeof(str) / sizeof(char);

			writeStreamToPort(str, check);

		}
		else {
			char str[] = "\rLED1 off\r\n\0";
			int check = sizeof str;

			writeStreamToPort(str, check);

		}
	}
}

bool UARTSender::writeStreamToPort(const char* strPtr, unsigned int size) {

	bool isOk = true;
	isOk = writeCharToPort("\r");
	for (unsigned int i = 0; i< size; i++) {
		isOk = writeCharToPort(&strPtr[i]);
	}
	isOk = writeCharToPort("\0");
	return isOk;
}

bool UARTSender::writeCharToPort(const char* ch) {

	usleep(waitTime);
	int count = write(m_uart0_filestream, ch , 1);
	if (count < 0)
	{
		cout << "UART TX error\n" << endl;
		return false;
	}
	return true;
}
