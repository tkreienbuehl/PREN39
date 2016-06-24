#pragma once

#include <fstream>
#include <iostream>
#include <time.h>

using namespace std;

class UARTSenderLogger {
private:

	UARTSenderLogger(string fileName);
	UARTSenderLogger(UARTSenderLogger const&);				// Copy Constructor private
	UARTSenderLogger& operator=(UARTSenderLogger const&);	// assignment operator private
	~UARTSenderLogger();

public:

	enum sender {
		RECEIVER,
		SENDER
	};

	static UARTSenderLogger* getInstance(string filename);
	static void releaseInstance();

	void logMessage(string message, sender who);

private:

	static UARTSenderLogger* m_TheInstance;
	string m_FileName;

};
