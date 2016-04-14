#include "../header/GueselStatObserver.hpp"

int main(int argc, char** argv) {

	GueselStarObserver observer;
	return 0;
}

GueselStarObserver::GueselStarObserver() {
	connectToServer("localhost");
}

GueselStarObserver::~GueselStarObserver() {

}

int GueselStarObserver::connectToServer(std::string hostName) {
	int portNr;
	socketAddressIn_t serverAddress;
	hostEnt_t* server;
	struct in_addr ipAddr;


	portNr = PORT_TO_PI;
	m_socketForward = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socketForward < 0 ) {
		cout << "Error opening socket" << endl;
		return -1;
	}

	inet_pton(AF_INET, "127.0.0.1", &ipAddr);
	server = gethostbyaddr(&ipAddr,sizeof(ipAddr), AF_INET);
	if (server == NULL) {
		cout << "ERROR no such host or can't resolve name to IP" << endl;
		return -1;
	}
	bzero(reinterpret_cast<char*>(&serverAddress), sizeof serverAddress);
	serverAddress.sin_family = AF_INET;
	bcopy(reinterpret_cast<char*>(server->h_addr), reinterpret_cast<char*>(&serverAddress.sin_addr.s_addr), server->h_length );
	serverAddress.sin_port = htons(portNr);
	if (connect(m_socketForward, reinterpret_cast<socketAddress_t*>(&serverAddress), sizeof(serverAddress)) < 0) {
		cout << "ERROR connecting" << endl;
	}

	string message;

	bool loop = true;
	int selector = 0 ;

	cv::namedWindow( "The Image", CV_WINDOW_AUTOSIZE );

	while (loop) {
		switch (selector) {
		case 0:
			message = "getFilteredImage";
			break;
		case 1:
			message = "getObjectImage";
			break;
		case 2:
			message = "getGrayImage";
		}
		if (selector == 2) {
			selector = 0;
		}
		else {
			selector++;
		}

		if (sendMessageRecieveImage(&message) < 0) {
			cout << "Connection aborted" << endl;
			close(m_socketForward);
			loop = false;
		}
		usleep(10000);
	}

	close(m_socketForward);
	return 0;


}


int GueselStarObserver::sendMessageRecieveImage(string* message) {

	char buffer[256];
	bzero(buffer,256);

	int n = write(m_socketForward, message->c_str(), message->length());
	if (n < 0) {
		cout << "ERROR writing to socket" << endl;
		return -1;
	}
	usleep(800);
	if (message->find("getFilteredImage") != message->npos) {
		getFilteredImageFromServer();
	}
	else if (message->find("getObjectImage") != message->npos) {
	}
	else if (message->find("getGrayImage") != message->npos) {
		getGrayImageFromServer();
	}
	message->clear();
	return 0;
}

void GueselStarObserver::getFilteredImageFromServer() {

	int height = 240, width = 320;
	cv::Mat img = cv::Mat::zeros( height, width, CV_8UC1);
	int  imgSize = img.total()*img.elemSize();
	//cout << imgSize << endl;
	uchar sockData[imgSize];

	for (int i = 0; i < imgSize; i += imgSize) {
		if ((imgSize = recv(m_socketForward, sockData+i, imgSize-i, 0)) == -1) {
			cout << "ERROR" << endl;
		}
	}

	int ptr=0;
	for (int i = 0;  i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i,j) = sockData[ptr];
			ptr++;
		}
	}

	if (!img.empty()) {
		cv::imshow("The filtered Image", img);
		cv::waitKey(10);
	}

}

void GueselStarObserver::getGrayImageFromServer() {

	int height = 240, width = 320;
	cv::Mat img = cv::Mat::zeros( height, width, CV_8UC1);
	int  imgSize = img.total()*img.elemSize();
	//cout << imgSize << endl;
	uchar sockData[imgSize];

	for (int i = 0; i < imgSize; i += imgSize) {
		if ((imgSize = recv(m_socketForward, sockData+i, imgSize-i, 0)) == -1) {
			cout << "ERROR" << endl;
		}
	}

	int ptr=0;
	for (int i = 0;  i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i,j) = sockData[ptr];
			ptr++;
		}
	}

	if (!img.empty()) {
		cv::imshow("The gray Image", img);
		cv::waitKey(10);
	}

}


