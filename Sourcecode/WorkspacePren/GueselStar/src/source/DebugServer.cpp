#include "../header/DebugServer.hpp"

DebugServer::DebugServer(RouteFinder* rtFinder, ObjectFinder* objectFinder) {
	m_running = true;
	m_rtFinder = rtFinder;
	m_objectFinder = objectFinder;
	pthread_mutex_init(&m_mutex, NULL);
}

DebugServer::~DebugServer() {

}

void* DebugServer::startDebugServer(void* threadId) {
	((DebugServer*)threadId)->runningServer();
	cout << "DebugServer thread ended " << endl;
	return threadId;
}

void DebugServer::stopServer() {
	m_running = false;
}

void DebugServer::runningServer() {
	int socketfd, newSocketfd, portNr;
	socklen_t clientLength;

	sockaddressIn_t serverAddress, clientAddress;

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

	listen(socketfd,5);

	clientLength = sizeof(clientAddress);
	newSocketfd = accept(socketfd, reinterpret_cast<socketAddress_t*>(&clientAddress), &clientLength);

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
		pthread_mutex_lock(&m_mutex);
		recv(newSocketfd, buffer, 50, 0);

		string message(buffer);
		if (message.find("end connection") != message.npos) {
			m_running = false;
		}
		else if (message.find("getFilteredImage") != message.npos) {
			cv::Mat image = m_rtFinder->getFilteredImage();
			if (!image.empty()) {
				prepareAndSendImage(newSocketfd, &image);
			}
		}
		else if (message.find("getObjectImage") != message.npos) {
			cv::Mat image = m_objectFinder->getImage();
			if (!image.empty()) {
				prepareAndSendColorImage(newSocketfd, &image);
			}
		}
		else if (message.find("getGrayImage") != message.npos) {
			cv::Mat image = m_rtFinder->getGrayImage();
			if (!image.empty()) {
				prepareAndSendImage(newSocketfd, &image);
			}
		}
		else {
			string msg = "I got your message, but i dont't understand";
			send(newSocketfd, msg.c_str(), msg.length(), 0);
			msg.clear();
		}

		message.clear();
		pthread_mutex_unlock(&m_mutex);
	}
}

void DebugServer::prepareAndSendImage(int socketfd, cv::Mat* image) {

	*image = (image->reshape(0,1)); // to make it continuous

	int  imgSize = image->total()*image->elemSize();

	int bytes = send(socketfd, image->data, imgSize, 0);
	if (bytes < 0) {
		cout << "ERROR writing to socket" << endl;
	}

}

void DebugServer::prepareAndSendColorImage(int socketfd, cv::Mat* image) {

	cv::Mat lImg = image->clone();
	lImg = (lImg.reshape(0,1)); // to make it continuous

	int  imgSize = lImg.total()*lImg.elemSize();

	int bytes = send(socketfd, lImg.data, imgSize, 0);
	if (bytes < 0) {
		cout << "ERROR writing to socket" << endl;
	}

}
