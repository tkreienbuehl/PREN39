#include <gtest/gtest.h>
#include "../header/GueselStar.hpp"
#include "../test/PictureCreator_test.hpp"
#include "../header/DebugServer.hpp"

int main(int argc, char** argv) {

	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	usleep(1000000);

	pthread_t threads[6];
	XInitThreads();
	int rc;

	PrenController* controller = new PrenController();

	PictureCreator* picCreator = new PictureCreator(controller);
	RouteFinder* rtFinder = new RouteFinder(controller, picCreator);
	ObjectFinder* objectFinder = new ObjectFinder(controller, picCreator);
	PictureViewer* picViewer = new PictureViewer(rtFinder, objectFinder);
	DebugServer* debugServer = new DebugServer(rtFinder, objectFinder);

	rc = pthread_create(&threads[0], NULL, PictureCreator::staticEntryPoint, picCreator);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	usleep(1000000);	//wait a moment to let the camera come up

	rc = pthread_create(&threads[1], NULL, RouteFinder::staticEntryPoint, rtFinder);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	usleep(100);


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

	controller->start();

	debugServer->stopServer();
	usleep(1000000);
	picViewer->stopViewer();
	usleep(1000000);
	objectFinder->stopProcess();
	usleep(1000000);

	delete picViewer;
	delete rtFinder;
	delete objectFinder;

	picCreator->StopRecording();

	usleep(5000);

	delete picCreator;
	delete controller;
	delete debugServer;

	cout << "done :)" << endl;

	return 0;
}
