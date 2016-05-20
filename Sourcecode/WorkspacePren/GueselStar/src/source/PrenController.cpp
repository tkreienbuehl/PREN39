#include "../header/PrenController.hpp"

PrenController::PrenController(UARTSender* sender, ConsoleView* viewer) {
	prenConfig = new PrenConfiguration();
	m_State = STOPPED;

	uartSender = sender;
	consoleView = viewer;

	objectStateObserver = NULL;

}

PrenController::~PrenController() {

	delete uartSender;
}

PrenConfiguration* PrenController::getPrenConfig(void) {

	return prenConfig;
}

ConsoleView* PrenController::getConsoleView() {
	return consoleView;
}

void PrenController::start() {
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

	uartSender->sendStopCmd();
	usleep(10000);
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
	char str[30];
	bzero(str, sizeof(str));
	sprintf(str, "Distance to Container: %d", distance);
	printString(str, OBJECT_FINDER, 1);
	// inform MC-Board
	uartSender->setContainerFound(distance);
}
void PrenController::setCrossingFound(int distance) {
	char str[50];
	bzero(str, sizeof(str));
	sprintf(str, "Crossing ahead: Distance to Crossing: %d", distance);
	printString(str, CONTROLLER, 1);
	// inform MC-Board
	// inform ObjectFinder
	/*list<ObjectStateObserver*>::iterator iter = objectStateObserverList.begin();
	for (; iter != objectStateObserverList.end(); iter++) {
		(*iter)->updateCrossingState(true);
	}*/
	objectStateObserver->updateCrossingState(true);
}

void PrenController::setTargetFieldFound(int distance) {
	char str[50];
	bzero(str, sizeof(str));
	sprintf(str, "Targetfield ahead: Distance to Tagetfield: %d", distance);
	printString(str, CONTROLLER, 1);
	// inform MC-Board
	uartSender->setTargetFieldFound(distance);
}

void PrenController::setLaneLost() {
	// inform MC-Board
	printString("RouterFinder lost Lane", CONTROLLER, 1);
	uartSender->sendStopCmd();
}

void PrenController::setSteeringAngle(int angle) {
	char str[20];
	bzero(str, sizeof(str));
	sprintf(str, "set new angle %d", angle);
	printString(str, CONTROLLER, 0);
	// inform MC-Board
	uartSender->setSteering(angle);
}

void PrenController::setEngineSpeed(uint8_t speed) {
	// inform MC-Board
	uartSender->setEngineSpeed(speed);
}

void PrenController::setVehicleInCrossing() {
	printString("Vehicle in Crossing", CONTROLLER, 1);
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

void PrenController::printString(string str, classes cl, uint line) {
	if (prenConfig->IS_ON_IDE) {
		cout << str.c_str() << endl;
	} else {
		if (consoleView != NULL) {
			switch (cl) {
			case ROUTE_FINDER:
				consoleView->setRouteFinderText(str, line);
				break;
			case OBJECT_FINDER:
				consoleView->setObjectFinderText(str, line);
				break;
			case UART_COMM:
				consoleView->setUARTStateText(str, line);
				break;
			case CONTROLLER:
				consoleView->setControllerText(str, line);
				break;
			}
		}
	}
}

void PrenController::setObjectStateObserver(ObjectStateObserver* observer) {
	//objectStateObserverList.push_back(observer);
	objectStateObserver = observer;
}

void PrenController::setCameraPos(CameraStatesE pos) {
	char str[40];
	sprintf(str, "Cam-Pos changed to: %d", pos);
	consoleView->setControllerText(str, 1);
	uartSender->setCameraPos(pos);
}
