#include "../header/UARTSenderLogger.hpp"

UARTSenderLogger* UARTSenderLogger::m_TheInstance = NULL;

UARTSenderLogger::UARTSenderLogger(string fileName) {
	m_FileName = fileName;
}

UARTSenderLogger::~UARTSenderLogger() {

}

UARTSenderLogger* UARTSenderLogger::getInstance(string filename) {
	if (m_TheInstance == NULL) {
		m_TheInstance = new UARTSenderLogger(filename);
	}
	return m_TheInstance;
}

void UARTSenderLogger::releaseInstance() {
	if (m_TheInstance != NULL) {
		delete m_TheInstance;
	}
}

void UARTSenderLogger::logMessage(string message, sender who) {
	char timeStamp[100];
	time_t clk = time(NULL);
	sprintf(timeStamp, "Log_time: %s, sender: %i, Message: ", ctime(&clk), who);
	string prestr(timeStamp);
	message = prestr + message;
    ofstream log_file(m_FileName.c_str(), ios_base::out | ios_base::app );
    log_file << message << endl;
}

