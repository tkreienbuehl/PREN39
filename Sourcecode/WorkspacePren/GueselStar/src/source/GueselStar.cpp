#include "../header/GueselStar.hpp"

int main() {

	PrenController* controller = new PrenController();

	PictureCreator* picCreator = new PictureCreator(controller);
	RouteFinder* rtFinder = new RouteFinder(controller);
	ObjectFinder* objectFinder = new ObjectFinder(controller);

	rtFinder->findRoute(picCreator);
	objectFinder->findObjects(picCreator);

	controller->Start();

	delete rtFinder;
	delete objectFinder;
	picCreator->StopRecording();
	delete picCreator;
	delete controller;
	return 0;
}
