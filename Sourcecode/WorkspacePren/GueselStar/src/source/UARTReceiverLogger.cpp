#include "../header/UARTReceiverLogger.hpp"

UARTReceiverLogger* UARTReceiverLogger::m_TheInstance = NULL;

UARTReceiverLogger::UARTReceiverLogger(string fileName) {
	m_FileName = fileName;
}

UARTReceiverLogger::~UARTReceiverLogger() {

}

UARTReceiverLogger* UARTReceiverLogger::getInstance(string filename) {
	if (m_TheInstance == NULL) {
		m_TheInstance = new UARTReceiverLogger(filename);
	}
	return m_TheInstance;
}

void UARTReceiverLogger::releaseInstance() {
	if (m_TheInstance != NULL) {
		delete m_TheInstance;
	}
}

void UARTReceiverLogger::logMessage(string message, sender who) {
	char timeStamp[100];
	time_t clk = time(NULL);
	sprintf(timeStamp, "Log_time: %s, sender: %i, Message: ", ctime(&clk), who);
	string prestr(timeStamp);
	message = prestr + message;
    ofstream log_file(m_FileName.c_str(), ios_base::out | ios_base::app );
    log_file << message << endl;
}

