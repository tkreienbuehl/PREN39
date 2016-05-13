#include "../header/PrenController.hpp"

PrenController::PrenController() {
	prenConfig = new PrenConfiguration();
	m_State = STOPPED;

	handler = new UARTHandler();
	consoleView = NULL;
	handler->openSerialIF(prenConfig->IF_NAME.c_str());
	usleep(1000);
	if (!handler->setUartConfig(handler->FULL_SPEED)) {
		cout << "configuration failed" << endl;
		delete handler;
	}

	uartReceiver = new UARTReciever(handler);
	uartSender = new UARTSender(handler);
}

PrenController::~PrenController() {

	delete uartReceiver;
	delete uartSender;
	if (consoleView!= NULL) {
		delete consoleView;
	}
	delete handler;
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

	rc = pthread_create(&threads[0], NULL, UARTReciever::staticEntryPoint,
			uartReceiver);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
	}
	usleep(1000);

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
	uartReceiver->stopReading();
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

	//cout << "set new angle" << angle << endl;
	uartSender->setSteering(angle);
}

void PrenController::setVehicleInCrossing() {

	//cout << "Vehicle in Crossing";
}

bool PrenController::checkObjectOnLane(void) {

	bool objectOnLane = false;
	int distance = uartReceiver->getUltraDist();

	if (distance < 10) {
		objectOnLane = true;
	}

	return objectOnLane;
}

int PrenController::getFlexDistance(void) {

	return uartReceiver->getFlexDistance();
}

int PrenController::getEngineSpeed(void) {

	return uartReceiver->getEngineSpeed();
}
