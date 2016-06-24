#pragma once

#include <fstream>
#include <iostream>
#include <time.h>

using namespace std;

class UARTReceiverLogger {
private:

	UARTReceiverLogger(string fileName);
	UARTReceiverLogger(UARTReceiverLogger const&);				// Copy Constructor private
	UARTReceiverLogger& operator=(UARTReceiverLogger const&);	// assignment operator private
	~UARTReceiverLogger();

public:

	enum sender {
		RECEIVER,
		SENDER
	};

	static UARTReceiverLogger* getInstance(string filename);
	static void releaseInstance();

	void logMessage(string message, sender who);

private:

	static UARTReceiverLogger* m_TheInstance;
	string m_FileName;

};
