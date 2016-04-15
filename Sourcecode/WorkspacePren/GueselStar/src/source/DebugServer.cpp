#include "../header/DebugServer.hpp"

DebugServer::DebugServer(RouteFinder* rtFinder, ObjectFinder* objectFinder) {
	m_running = true;
	m_rtFinder = rtFinder;
	m_objectFinder = objectFinder;
}

DebugServer::~DebugServer() {

}

void* DebugServer::startDebugServer(void* threadId) {
	((DebugServer*)threadId)->runningServer();
	cout << "DebugServer thread ended " << endl;
	return NULL;
}

void DebugServer::stopServer() {
	m_running = false;
}

void DebugServer::runningServer() {
	int socketfd, newSocketfd, portNr;
	socklen_t clientLength;

	sockaddressIn_t serverAddress, clientAddress;

	cout << "Hallo! " << endl;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0 ) {
		cout << "Error opening socket" << endl;
	}
	bzero((char*) &serverAddress, sizeof(serverAddress));
	portNr = PORT_TO_PI;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(portNr);

	if (bind(socketfd, reinterpret_cast<socketAddress_t*>(&serverAddress), sizeof(serverAddress)) < 0 ) {
		cout << "ERROR on binding" << endl;
	}

	cout << "server listens" << endl;
	listen(socketfd,5);

	clientLength = sizeof(clientAddress);
	newSocketfd = accept(socketfd, reinterpret_cast<socketAddress_t*>(&clientAddress), &clientLength);

	cout << "server connected" << endl;

	if (newSocketfd < 0) {
		cout << "ERROR on accept" << endl;
	}

	requestHandler(newSocketfd);

	close(newSocketfd);
	close(socketfd);

}

void DebugServer::requestHandler(int newSocketfd) {

	char buffer[50];

	while (m_running) {
		bzero(buffer,50);
		int n = read(newSocketfd, buffer, 49);
		if (n < 0) {
			cout << "ERROR reading from socket" << endl;
		}
		string message(buffer);
		if (message.find("end connection") != message.npos) {
			m_running = false;
		}
		else if (message.find("getFilteredImage") != message.npos) {
			cv::Mat image = m_rtFinder->getFilteredImage();
			if (!image.empty()) {
				prepareAndSendImage(newSocketfd, image);
			}
		}
		else if (message.find("getObjectImage") != message.npos) {
			cv::Mat image = m_objectFinder->getImage();
			if (!image.empty()) {
				//prepareAndSendImage(newSocketfd, image);
			}
		}
		else if (message.find("getGrayImage") != message.npos) {
			cv::Mat image = m_rtFinder->getGrayImage();
			if (!image.empty()) {
				prepareAndSendImage(newSocketfd, image);
			}
		}
		else {
			string msg = "I got your message, but i dont't understand";
			n = write(newSocketfd, msg.c_str(), msg.length());
			if (n < 0) {
				cout << "ERROR writing to socket" << endl;
			}
			msg.clear();
		}

		//cout << message << endl;
		message.clear();
	}
}

void DebugServer::prepareAndSendImage(int socketfd, cv::Mat& image) {

	image = (image.reshape(0,1)); // to make it continuous

	int  imgSize = image.total()*image.elemSize();

	//cout << imgSize << endl;

	int bytes = send(socketfd, image.data, imgSize, 0);
	if (bytes < 0) {
		cout << "ERROR writing to socket" << endl;
	}

}
