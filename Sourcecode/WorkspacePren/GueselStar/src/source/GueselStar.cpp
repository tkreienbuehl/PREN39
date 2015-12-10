#include "../header/GueselStar.hpp"

int main() {

	PrenController* controller = new PrenController();

	PictureCreator* PicCreator = new PictureCreator(controller);
	RouteFinder* RtFinder = new RouteFinder(controller);
	ObjectFinder* objectFinder = new ObjectFinder(controller);

	RtFinder->FindRoute(PicCreator);
	objectFinder->findObjects(PicCreator);

	controller->Start();

	PicCreator->StopRecording();
	delete RtFinder;
	delete objectFinder;
	delete PicCreator;
	delete controller;
	return 0;
}
