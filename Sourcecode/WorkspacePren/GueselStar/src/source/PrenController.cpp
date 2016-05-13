#include "../header/PrenController.hpp"

PrenController::PrenController(UARTSender* sender) {
	prenConfig = new PrenConfiguration();
	m_State = STOPPED;

	uartSender = sender;
	consoleView = NULL;

}

PrenController::~PrenController() {

	delete uartSender;
	if (consoleView!= NULL) {
		delete consoleView;
	}
}

PrenConfiguration* PrenController::getPrenConfig(void) {

	return prenConfig;
}

ConsoleView* PrenController::getConsoleView() {
	return consoleView;
}

void PrenController::start() {
	// Start thread PictureCreator
	m_State = RUNNING;
	runProgram();
}

void PrenController::setState(const states state) {
	cout << state << endl;
	switch (state) {
	case END:
		stopProgram();
		break;
	default:
		cout << "illegal state used" << endl;
	}
}

void PrenController::runProgram() {

	pthread_t threads[2];
	int rc;

	if (!prenConfig->IS_ON_IDE) {
		consoleView = new ConsoleView();
		rc = pthread_create(&threads[1], NULL, ConsoleView::startThread, consoleView);
		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}

	uartSender->sendStartCmd();

	while (m_State != END) {

		usleep(300);
	}

	if (consoleView != NULL) {
		consoleView->stopProcess();
	}
	uartSender->sendStopCmd();
	cout << "exiting Controller" << endl;
}

int PrenController::stopProgram() {
	m_State = END;
	cout << "Controller stopped " << endl;
	return m_State;
}

void PrenController::setContainerFound(int distance) {

	// inform MC-Board
	uartSender->setContainerFound(distance);
	//cout << "Container ahead: Distance to Container: " << distance << endl;
}
void PrenController::setCrossingFound(int distance) {

	// inform MC-Board
	//cout << "Crossing ahead: Distance to Crossing: " << distance << endl;
}

void PrenController::setTargetFieldFound(int distance) {

	//cout << "Targetfield ahead: Distance to Tagetfield: " << distance << endl;
	uartSender->setTargetFieldFound(distance);
}

void PrenController::setLaneLost() {

	// inform MC-Board
	cout << "RouterFinder lost Lane" << endl;
}

void PrenController::setSteeringAngle(int angle) {

	char str[20];
	bzero(str,sizeof(str));
	sprintf(str, "set new angle % d",angle);
	if (!prenConfig->IS_ON_IDE) {
		if (consoleView != NULL ) {
			consoleView->setUARTStateText(str);
		}
	}
	else {
		cout << str << endl;
	}
	uartSender->setSteering(angle);
}

void PrenController::setEngineSpeed(uint8_t speed) {
	uartSender->setEngineSpeed(speed);
}

void PrenController::setVehicleInCrossing() {

	if (!prenConfig->IS_ON_IDE) {
		if (consoleView != NULL ) {
			consoleView->setUARTStateText("Vehicle in Crossing");
		}
	}
}

bool PrenController::checkObjectOnLane(void) {

	bool objectOnLane = false;
	int distance = 5; //uartReceiver->getUltraDist();

	if (distance < 10) {
		objectOnLane = true;
	}

	return objectOnLane;
}

int PrenController::getFlexDistance(void) {

	return 7; //uartReceiver->getFlexDistance();
}

int PrenController::getEngineSpeed(void) {

	return 7; //uartReceiver->getEngineSpeed();
}
