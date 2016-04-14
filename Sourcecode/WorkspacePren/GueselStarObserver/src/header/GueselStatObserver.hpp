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

	int connectToServer(string hostname);
	int sendMessageRecieveImage(string* message);
	void getFilteredImageFromServer();
	void getGrayImageFromServer();

	static const short PORT_TO_PI = 4001;
	int m_socketForward;

};
