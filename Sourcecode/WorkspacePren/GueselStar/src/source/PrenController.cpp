#include "../header/PrenController.hpp"


PrenController::PrenController() {
	m_State = STOPPED;
}

PrenController::~PrenController() {

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

void PrenController::stopCar() {

}

void PrenController::setContainerFound(int distance) {

	cout << "Container detected" << endl;
}
