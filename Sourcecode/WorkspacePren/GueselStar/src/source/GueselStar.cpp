#include "../header/GueselStar.hpp"

int main() {

	PrenController* controller = new PrenController();

	PictureCreator* PicCreator = new PictureCreator(controller);
	RouteFinder* RtFinder = new RouteFinder(controller);
	RtFinder->FindRoute(PicCreator);

	controller->Start();

	PicCreator->StopRecording();
	delete RtFinder;
	delete PicCreator;
	delete controller;
	return 0;
}
