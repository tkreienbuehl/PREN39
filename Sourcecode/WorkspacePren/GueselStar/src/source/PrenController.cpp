#include "../header/PrenController.hpp"

using namespace std;


PrenController::PrenController() {
	m_State = STOPPED;
}

PrenController::~PrenController() {

}

void PrenController::Start() {
	// Start thread PictureCreator
	m_State = RUNNING;
	RunProgram();
}

void PrenController::SetState(const states state) {
	switch (state) {
	case END:
		StopProgram();
		break;
	default:
		cout << "illegal state used" << endl;
	}
}

void PrenController::RunProgram() {

	while (m_State != END) {
		//@TODO Implement Controller code here
	}

}

int PrenController::StopProgram() {
	m_State = END;
	return m_State;
}
