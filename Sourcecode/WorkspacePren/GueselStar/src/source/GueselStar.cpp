#include "../header/GueselStar.hpp"

int main() {

	pthread_t threads[4];
	XInitThreads();
	int rc;

	PrenController* controller = new PrenController();

	PictureCreator* picCreator = new PictureCreator(controller);
	RouteFinder* rtFinder = new RouteFinder(controller, picCreator);
	ObjectFinder* objectFinder = new ObjectFinder(controller, picCreator);
	PictureViewer* picViewer = new PictureViewer(rtFinder, objectFinder);

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

	rc = pthread_create(&threads[3], NULL, PictureViewer::startThread, picViewer);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
	usleep(100);

	controller->start();

	picViewer->stopViewer();
	usleep(5000);
	objectFinder->stopProcess();
	usleep(5000);

	delete picViewer;
	delete rtFinder;
	delete objectFinder;

	picCreator->StopRecording();

	usleep(5000);

	delete picCreator;
	delete controller;

	cout << "done :)" << endl;

	return 0;
}
