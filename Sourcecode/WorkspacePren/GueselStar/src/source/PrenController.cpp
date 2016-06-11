#include "../header/PrenController.hpp"

PrenController::PrenController(UARTSender* sender, ConsoleView* viewer) {
	prenConfig = new PrenConfiguration();
	m_State = STOPPED;
	uartSender = sender;
	consoleView = viewer;
	objectStateObserver = NULL;
	m_FlexValue = 0;
	ultraValueIndex = 0;
	m_objectOnLane = false;
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
	sprintf(str, "Distance to Container: %d", distance);
	printString(str, OBJECT_FINDER, 1);
	// inform MC-Board
	uartSender->setContainerFound(distance);
}
void PrenController::setCrossingFound(int distance) {
	char str[50];
	sprintf(str, "Crossing ahead: Distance to Crossing: %d", distance);
	printString(str, CONTROLLER, 1);
	// inform MC-Board
	// inform ObjectFinder
	objectStateObserver->updateCrossingState(true);
}

void PrenController::setTargetFieldFound(int distance) {
	char str[50];
	sprintf(str, "Targetfield ahead: Distance to Targetfield: %d", distance);
	printString(str, CONTROLLER, 1);
	// inform MC-Board
	uartSender->setTargetFieldFound(distance);
}

void PrenController::setLaneLost() {
	// inform MC-Board
	printString("RouteFinder lost Lane", CONTROLLER, 1);
	uartSender->sendStopCmd();
	if(prenConfig->IS_ON_PI) {
		usleep(3000 * 1000);
		this->setState(END);
	}
}

void PrenController::setSteeringAngle(int angle) {
	char str[20];
	sprintf(str, "set new angle %d", angle);
	printString(str, CONTROLLER, 0);
	// inform MC-Board
	uartSender->setSteering(angle);
}

void PrenController::setEngineSpeed(uint8_t speed) {
	// inform MC-Board
	uartSender->setEngineSpeed(speed);
}

void PrenController::setVehicleInCrossing(bool found) {
	if (found) {
		printString("Vehicle in Crossing", OBJECT_FINDER, 10);
	} else {
		uartSender->setEngineSpeed(prenConfig->MAX_SPEED);
	}
}

void PrenController::checkUltraDist(int ultraDistance) {
/*
	bool objectOnLane = false;

	lastUltraValues[ultraValueIndex] = ultraDistance;

	if (ultraValueIndex == 5) {
		int counter = 0;
		for (int i = 0; i < ultraValueIndex; i++) {
			if (lastUltraValues[i] < prenConfig->MAX_DISTANCE_TO_OBJECT) {
				counter++;
			}
		}
		if (counter >= 3) {
			objectOnLane = true;
			ultraValueIndex = 0;
		} else {
			objectOnLane = false;
			ultraValueIndex = 0;
		}
	}

	ultraValueIndex++;

	if (m_objectOnLane != objectOnLane) {
		m_objectOnLane = objectOnLane;
		objectStateObserver->updateObjectOnLaneState(m_objectOnLane);

		if (m_objectOnLane) {
			uartSender->setEngineSpeed(0);
			printString("Object on Lane", OBJECT_FINDER, 5);
		} else {
			uartSender->setEngineSpeed(prenConfig->MAX_SPEED);
			printString("lane free", OBJECT_FINDER, 5);
		}
	}
	*/
}

void PrenController::setFlexValue(int flexValue) {
	m_FlexValue = flexValue;
}

int PrenController::getFlexDistance(void) {

	return m_FlexValue;
}

int PrenController::getEngineSpeed(void) {

	return 7; //uartReceiver->getEngineSpeed();
}

void PrenController::responseStillThere() {
	uartSender->stillThereResponse();
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

void PrenController::setContainerLoadingFinished(bool finished) {
	setEngineSpeed(prenConfig->MAX_SPEED);
	printString("Container aufgeladen", OBJECT_FINDER, 11);
}

void PrenController::setCameraPos(CameraStatesE pos) {
	char str[40];
	sprintf(str, "Cam-Pos changed to: %d", pos);
	consoleView->setControllerText(str, 1);
	uartSender->setCameraPos(pos);
}
