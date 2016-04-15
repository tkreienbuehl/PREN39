#pragma once

#include <iostream>
#include "RouteFinder.hpp"
#include "ObjectFinder.hpp"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

class DebugServer {
public:
	DebugServer(RouteFinder* rtFinder, ObjectFinder* objectFinder);
	~DebugServer();

	static void* startDebugServer(void* threadId);

	void stopServer();

private:

	void runningServer();
	void requestHandler(int newSocketfd);
	void prepareAndSendImage(int socketfd, cv::Mat& image);

	typedef struct sockaddr_in sockaddressIn_t;
	typedef struct sockaddr socketAddress_t;

	static const short PORT_TO_PI = 14001;
	bool m_running;
	RouteFinder* m_rtFinder;
	ObjectFinder* m_objectFinder;

};
