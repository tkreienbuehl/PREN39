#include "../header/GueselStarObserver.hpp"

int main(int argc, char** argv) {

	GueselStarObserver observer;
	return 0;
}

GueselStarObserver::GueselStarObserver() {
	connectToServer("10.10.0.1");
	//connectToServer("127.0.0.1");
}

GueselStarObserver::~GueselStarObserver() {

}

int GueselStarObserver::connectToServer(std::string ipAddr) {
	int portNr;
	socketAddressIn_t serverAddress;
	hostEnt_t* server;
	ipAddress_t ipAddress;

	portNr = PORT_TO_PI;
	m_socketForward = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socketForward < 0 ) {
		cout << "Error opening socket" << endl;
		return -1;
	}

	cout << "Connecting to server..." << endl;
	inet_pton(AF_INET, ipAddr.c_str(), &ipAddress);
	server = gethostbyaddr(&ipAddress,sizeof(ipAddress), AF_INET);
	if (server == NULL) {
		cout << "ERROR no such host or can't resolve name to IP" << endl;
		return -1;
	}
	bzero(reinterpret_cast<char*>(&serverAddress), sizeof serverAddress);
	serverAddress.sin_family = AF_INET;
	bcopy(reinterpret_cast<char*>(server->h_addr),
			reinterpret_cast<char*>(&serverAddress.sin_addr.s_addr),
			server->h_length );
	serverAddress.sin_port = htons(portNr);
	if (connect(m_socketForward, reinterpret_cast<socketAddress_t*>(&serverAddress), sizeof(serverAddress)) < 0) {
		cout << "ERROR connecting" << endl;
	}
	cout << "connected to server" << endl;
	//usleep(100 * 1000);

	string message;

	bool loop = true;
	int selector = 0 ;

	cv::namedWindow( "The filtered Image", CV_WINDOW_AUTOSIZE );

	while (loop) {
		//usleep(1000);
		switch (selector) {
		case 0:
			message = "getFilteredImage";
			break;
		case 1:
			message = "getGrayImage";
			break;
		case 2:
			message = "getObjectImage";
			break;
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
		usleep(15 * 100);
	}

	close(m_socketForward);
	return 0;


}


int GueselStarObserver::sendMessageRecieveImage(string* message) {

	cv::Mat grayImg, fltImg, objImg;

	int n = write(m_socketForward, message->c_str(), message->length());
	if (n < 0) {
		cout << "ERROR writing to socket" << endl;
		return -1;
	}
	if (message->find("dummy") != message->npos) {

	}
	else if (message->find("getFilteredImage") != message->npos) {
		fltImg = getFilteredImageFromServer().clone();
		cv::resize(fltImg,m_FltImg,m_FltImg.size(),2.0,2.0,cv::INTER_LANCZOS4);
		if (!fltImg.empty()) {
			cv::imshow("The filtered Image", m_FltImg);
			cv::waitKey(1);
		}
	}
	else if (message->find("getObjectImage") != message->npos) {
		objImg = getObjectImageFromServer().clone();
		cv::resize(objImg,m_ObjectImg,m_ObjectImg.size(),2.0,2.0,cv::INTER_LANCZOS4);
		if (!objImg.empty()) {
			cv::imshow("The object Image", m_ObjectImg);
			cv::waitKey(1);
		}
	}
	else if (message->find("getGrayImage") != message->npos) {
		grayImg = getGrayImageFromServer().clone();
		cv::resize(grayImg,m_GrayImg,m_GrayImg.size(),2.0,2.0,cv::INTER_LANCZOS4);
		if (!grayImg.empty()) {
			cv::imshow("The gray Image", m_GrayImg);
			cv::waitKey(1);
		}
	}
	message->clear();
	return 0;
}

cv::Mat GueselStarObserver::getFilteredImageFromServer() {

	int height = 240, width = 320;
	cv::Mat img = cv::Mat::zeros( height, width, CV_8UC1);
	int  imgSize = img.total()*img.elemSize();
	//cout << imgSize << endl;
	uchar sockData[imgSize];

	if ((imgSize = recv(m_socketForward, sockData, imgSize, MSG_WAITALL)) == -1) {
		cout << "ERROR Filtered Image" << endl;
	}

	int ptr=0;
	for (int i = 0;  i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i,j) = sockData[ptr];
			ptr++;
		}
	}

	return img;
}

cv::Mat GueselStarObserver::getGrayImageFromServer() {

	int height = 240, width = 320;
	cv::Mat img = cv::Mat::zeros( height, width, CV_8UC1);
	int  imgSize = img.total()*img.elemSize();
	//cout << imgSize << endl;
	uchar sockData[imgSize];

	if ((imgSize = recv(m_socketForward, sockData, imgSize, MSG_WAITALL)) == -1) {
		cout << "ERROR Gray Image" << endl;
	}

	int ptr=0;
	for (int i = 0;  i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<uchar>(i,j) = sockData[ptr];
			ptr++;
		}
	}

	return img;

}

cv::Mat GueselStarObserver::getObjectImageFromServer() {

	int height = 240, width = 320 >> 1;
	cv::Mat img = cv::Mat::zeros( height, width, CV_8UC3);
	int  imgSize = img.total()*img.elemSize();
	//cout << imgSize << endl;
	uchar sockData[imgSize];

	if ((imgSize = recv(m_socketForward, sockData, imgSize, MSG_WAITALL)) == -1) {
		cout << "ERROR Object Image" << endl;
	}

	int ptr=0;
	for (int i = 0;  i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);;
			ptr += 3;
		}
	}

	char buffer[256];
	recv(m_socketForward,buffer, sizeof(buffer), MSG_DONTWAIT);

	return img;

}


