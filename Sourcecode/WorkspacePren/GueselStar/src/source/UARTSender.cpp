#include "../header/UARTSender.hpp"

UARTSender::UARTSender(UARTHandler* handler) {
	m_uart0_filestream = handler->getFileStream();
	pthread_mutex_init(&m_mutex, NULL);
	m_Logger = UARTLogger::getInstance("uartLog.log");
}

UARTSender::~UARTSender() {
	UARTLogger::releaseInstance();
}

bool UARTSender::sendStartCmd() {
	char stream[] = "StartFrd a\r\n";
	return writeStreamToPort(stream, sizeof(stream));
	//return writeStreamToPort("\rStartFrd a\r\n\0", 14);
}

bool UARTSender::sendStopCmd() {
	char stream[] = "Stop a\r\n";
	return writeStreamToPort(stream, sizeof(stream));
	//return writeStreamToPort("\rStop a\r\n\0", 10);
}

bool UARTSender::setCameraPos(ushort pos) {
	char stream[16];
	int length = 0;
	length = sprintf(stream,"CamP p %d\r\n", pos);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setEngineSpeed(uint8_t speed, EngineModesE mode) {
	char stream[30];
	int length = 0;
	//length = sprintf(stream,"\rDCDr d %d %d", speed, mode);
	length = sprintf(stream,"DCDr d %d\r\n", speed);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setContainerFound(uint16_t distance) {
	char stream[20];
	int length = 0;
	length = sprintf(stream,"StA d %d\r\n", distance);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setTargetFieldFound(uint16_t distance) {
	char stream[20];
	int length = 0;
	length = sprintf(stream,"StE d %d\r\n", distance);
	return writeStreamToPort(stream, length);
}

bool UARTSender::setSteering(uint8_t steeringAng) {
	char stream[20];
	int length = 0;
	length = sprintf(stream,"LeS p %d\r\n", steeringAng);
	return writeStreamToPort(stream, length);
}

bool UARTSender::stillThereResponse(void) {
	return writeStreamToPort("Ja a\r\n\0", 8);
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
	string msg(strPtr);
	m_Logger->logMessage(msg, m_Logger->SENDER);
	bool isOk = true;
	int count = write(m_uart0_filestream, "\0" , 1);
	count = write(m_uart0_filestream, strPtr , size);
	if (count < 0)
	{
		//cout << "UART TX error\n" << endl;
		isOk = false;
	}
	count = write(m_uart0_filestream, "\0" , 1);
	pthread_mutex_unlock(&m_mutex);
	return isOk;
}
