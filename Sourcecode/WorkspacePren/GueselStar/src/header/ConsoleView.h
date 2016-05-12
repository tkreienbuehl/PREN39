#pragma once

#include <iostream>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

class ConsoleView {
public:
	ConsoleView();
	~ConsoleView();

	static void* startThread(void* threadId);

	void setRouteFinderText(string text);

private:
	void runProcess();

};
