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
	if (!handler->setUartConfig(handler->FULL_SPEED)) {
		cout << "configuration failed" << endl;
		delete handler;
		return -1;
	}

	UARTReciever* reciever = new UARTReciever(handler);

	rc = pthread_create(&threads, NULL, UARTReciever::staticEntryPoint, reciever);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		return(-1);
	}
	usleep(1000);

	int cnt = 0;
	UARTSender* sender = new UARTSender(handler);
	sender->sendStartCmd();
	while (cnt<=100) {
		sender->blinkLed(cnt % 2);
		sender->setCameraPos(sender->CAM_TURN_LEFT);
		sender->setEngineSpeed(160, sender->SOFT);
		sender->setSteering(127);
		sender->setContainerFound(213);
		sender->setTargetFieldFound(343);
		sender->stillThereResponse();
		cnt++;
	}
	sender->sendStopCmd();

	reciever->stopReading();
	usleep(1000);

	delete reciever;
	delete sender;
	delete handler;

	cout <<"Bye bye" << endl;

	return 0;
}
