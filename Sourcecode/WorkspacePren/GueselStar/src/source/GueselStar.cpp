#include <gtest/gtest.h>
#include "../header/GueselStar.hpp"
#include "../test/PictureCreator_test.hpp"
#include "../header/DebugServer.hpp"
#include "../header/UARTReciever.hpp"
#include "../header/UARTSender.hpp"
#include "../header/PrenConfiguration.hpp"
#include "../header/ConsoleView.hpp"

int main(int argc, char** argv) {

	testing::InitGoogleTest(&argc, argv);
	//RUN_ALL_TESTS();

	pthread_t threads[7];
	XInitThreads();
	int rc;

	PrenConfiguration prenConfig;

	ConsoleView* viewer = NULL;
	if (!prenConfig.IS_ON_IDE) {
		viewer = ConsoleView::getInstance();
		rc = pthread_create(&threads[6], NULL, ConsoleView::startThread, viewer);
		if (rc) {
			cout << "Error:unable to create Console View thread," << rc << endl;
			exit(-1);
		}
	}

	UARTHandler* handler = new UARTHandler();

	handler->openSerialIF(prenConfig.IF_NAME.c_str());
	if (!handler->setUartConfig(handler->MEDIUM)) {
		cout << "configuration failed" << endl;
	}
	UARTSender* uartSender = new UARTSender(handler);
	PrenController* controller = new PrenController(uartSender, viewer);
	UARTReciever* uartReceiver = new UARTReciever(handler, controller);
	PictureCreator* picCreator = new PictureCreator(controller);
	RouteFinder* rtFinder = new RouteFinder(controller, picCreator);
	ObjectFinder* objectFinder = new ObjectFinder(controller, picCreator);
	PictureViewer* picViewer = new PictureViewer(rtFinder, objectFinder);
	DebugServer* debugServer = new DebugServer(rtFinder, objectFinder);
	controller->setObjectStateObserver(objectFinder);

	rc = pthread_create(&threads[0], NULL, PictureCreator::staticEntryPoint, picCreator);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	usleep(10 * 1000);

	rc = pthread_create(&threads[2], NULL, ObjectFinder::staticEntryPoint, objectFinder);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}

	if(controller->getPrenConfig()->START_LOCAL_VIEW) {
		rc = pthread_create(&threads[3], NULL, PictureViewer::startThread, picViewer);
		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}
	else {
		if(controller->getPrenConfig()->IS_DEBUG) {
			rc = pthread_create(&threads[4], NULL, DebugServer::startDebugServer, debugServer);
			if (rc) {
				cout << "Error:unable to create thread," << rc << endl;
				exit(-1);
			}
		}
	}

	rc = pthread_create(&threads[5], NULL, UARTReciever::staticEntryPoint,
			uartReceiver);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
	}

	rc = pthread_create(&threads[1], NULL, RouteFinder::staticEntryPoint, rtFinder);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}

	controller->start();

	uartReceiver->stopReading();

	debugServer->stopServer();

	picViewer->stopViewer();

	objectFinder->stopProcess();

	usleep (1000 * 1000);
	rtFinder->stopProcess();
	usleep (5000 * 1000);

	delete picViewer;
	delete rtFinder;
	delete objectFinder;

	picCreator->StopRecording();

	delete picCreator;
	delete debugServer;
	delete uartReceiver;
	delete uartSender;
	delete handler;
	delete controller;

	cout << "done :)" << endl;

	return 0;
}
