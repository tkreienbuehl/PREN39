//============================================================================
// Name        : UARTTestes.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <gtest/gtest.h>

#include "./header/UARTTestes.hpp"

using namespace std;

int main() {

	pthread_t threads;
	XInitThreads();
	int rc;

	cout << "!!!Hello UART tester!!!" << endl;

	UARTHandler* handler = new UARTHandler();
	char ifName[] = "/dev/ttyACM3";
	handler->openSerialIF(ifName);
	handler->setUartConfig(handler->FULL_SPEED);

	UARTReciever* reciever = new UARTReciever(handler);

	rc = pthread_create(&threads, NULL, UARTReciever::staticEntryPoint, reciever);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		return(-1);
	}
	usleep(10000);

	int cnt = 0;
	UARTSender* sender = new UARTSender(handler);
	while (cnt<=100) {
		sender->blinkLed(cnt % 2);
		cnt++;
	}

	reciever->stopReading();
	usleep(100000);

	delete reciever;
	delete sender;
	delete handler;

	cout <<"Bye bye" << endl;

	return 0;
}
