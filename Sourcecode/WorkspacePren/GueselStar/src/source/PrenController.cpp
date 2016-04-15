#include "../header/PrenController.hpp"


PrenController::PrenController() {
	prenConfig = new PrenConfiguration();
	m_State = STOPPED;
}

PrenController::~PrenController() {

}

PrenConfiguration* PrenController::getPrenConfig(void) {

	return prenConfig;
}

void PrenController::start() {
	// Start thread PictureCreator
	m_State = RUNNING;
	runProgram();
}

void PrenController::setState(const states state) {
	switch (state) {
	case END:
		stopProgram();
		break;
	default:
		cout << "illegal state used" << endl;
	}
}

void PrenController::runProgram() {

	while (m_State != END) {
		//@TODO Implement Controller code here
		usleep(300);
	}
	cout << "exiting Controller" << endl;
}

int PrenController::stopProgram() {
	m_State = END;
	cout << "Controller stopped " << endl;
	return m_State;
}

void PrenController::setContainerFound(int distance) {

	// inform MC-Board
	cout << "Crossing ahead: Distance to Container: " << distance << endl;
}
void PrenController::setCrossingFound(int distance) {

	// inform MC-Board
	cout << "Crossing ahead: Distance to Crossing: " << distance << endl;
}

void PrenController::setTargetFieldFound(int distance) {

	// inform MC-Board
	cout << "Targetfield ahead: Distance to Tagetfield: " << distance << endl;
}


void PrenController::setLaneLost() {

	// inform MC-Board
	cout << "RouterFinder lost Lane" << endl;
}

void PrenController::setSteeringAngle(int angle) {

	// inform MC-Board: adjust
	cout << "set new angle" << angle << endl;
}

void PrenController::setVehicleInCrossing() {

	// inform MC-Board: wait
	cout << "Vehicle in Crossing";
}

bool PrenController:: checkObjectOnLane(void) {

	bool objectOnLane = false;
	int distance = 0;

	// ask MC-Board for distance

	if(distance < 10) {
		objectOnLane = true;
	}

	return objectOnLane;

}

int PrenController::getFlexDistance(void) {

	// ask MC-Board
	return 0;
}

int PrenController::getEngineSpeed(void) {

	// ask MC-Board
	return 0;
}
