#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <cv.h>
#include <highgui.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <arpa/inet.h>

using namespace std;

class GueselStarObserver {
public:
	GueselStarObserver();
	~GueselStarObserver();

private:

	typedef struct sockaddr_in socketAddressIn_t;
	typedef struct sockaddr socketAddress_t;
	typedef struct hostent hostEnt_t;
	typedef struct in_addr ipAddress_t;

	int connectToServer(string ipAddr);
	int sendMessageRecieveImage(string* message);

	cv::Mat getFilteredImageFromServer();
	cv::Mat getGrayImageFromServer();
	cv::Mat getObjectImageFromServer();

	static const short PORT_TO_PI = 14001;
	int m_socketForward;

	cv::Mat m_GrayImg, m_FltImg, m_ObjectImg;

};
