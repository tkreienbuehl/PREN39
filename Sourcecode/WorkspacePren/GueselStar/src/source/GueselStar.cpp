#include "../header/GueselStar.hpp"

int main() {

	pthread_t threads[3];
	XInitThreads();
	int rc;

	PrenController* controller = new PrenController();

	PictureCreator* picCreator = new PictureCreator(controller);
	RouteFinder* rtFinder = new RouteFinder(controller, picCreator);
	ObjectFinder* objectFinder = new ObjectFinder(controller, picCreator);

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

	controller->start();

	delete rtFinder;
	delete objectFinder;

	picCreator->StopRecording();

	usleep(1000);

	delete picCreator;
	delete controller;

	cout << "done :)" << endl;

	return 0;
}
