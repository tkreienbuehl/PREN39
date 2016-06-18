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

	usleep(1000000);

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
	usleep(1000);
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
	usleep(1000000);	//wait a moment to let the camera come up

	rc = pthread_create(&threads[2], NULL, ObjectFinder::staticEntryPoint, objectFinder);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	usleep(100);

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

	usleep(100);

	rc = pthread_create(&threads[5], NULL, UARTReciever::staticEntryPoint,
			uartReceiver);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
	}

	usleep(100);

	rc = pthread_create(&threads[1], NULL, RouteFinder::staticEntryPoint, rtFinder);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	usleep(100);

	usleep(1000);

	controller->start();

	usleep(50 * 1000);
	uartReceiver->stopReading();
	pthread_cancel(threads[5]);

	usleep(500 * 1000);
	debugServer->stopServer();
	usleep(500 * 1000);
	picViewer->stopViewer();
	usleep(500 * 1000);
	objectFinder->stopProcess();
	usleep(500 * 1000);
	pthread_cancel(threads[1]);

	delete picViewer;
	delete rtFinder;
	delete objectFinder;

	picCreator->StopRecording();

	usleep(5000);

	delete picCreator;
	delete controller;
	delete debugServer;
	delete uartReceiver;
	delete uartSender;
	delete handler;

	usleep(1000);

	ConsoleView::freeInstance();

	cout << "done :)" << endl;

	return 0;
}
