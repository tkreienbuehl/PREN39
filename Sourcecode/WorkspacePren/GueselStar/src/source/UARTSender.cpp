#include "../header/UARTSender.hpp"

UARTSender::UARTSender(UARTHandler* handler) {
	m_uart0_filestream = handler->getFileStream();
	pthread_mutex_init(&m_mutex, NULL);
}

UARTSender::~UARTSender() {

}

bool UARTSender::sendStartCmd() {
	return writeStreamToPort("\rStartFrd a\r\n\0", 14);
}

bool UARTSender::sendStopCmd() {
	return writeStreamToPort("\rStop a\r\n\0", 10);
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
	//length = sprintf(stream,"\rDCDr d %d %d", speed, mode);
	length = sprintf(stream,"\rDCDr d %d", speed);
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
	return writeStreamToPort("\rJa a\r\n\0", 8);
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

	pthread_mutex_lock(&m_mutex);
	bool isOk = true;
	int count = write(m_uart0_filestream, strPtr , size);
	if (count < 0)
	{
		//cout << "UART TX error\n" << endl;
		isOk = false;
	}
	pthread_mutex_unlock(&m_mutex);
	return isOk;
}
