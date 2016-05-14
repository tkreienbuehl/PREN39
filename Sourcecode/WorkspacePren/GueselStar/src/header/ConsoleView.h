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

	void setRouteFinderText(string text, unsigned int line = 0);
	void setObjectFinderText(string text, unsigned int line = 0);
	void setUARTStateText(string text, unsigned int line = 0);
	void setControllerText(string text, unsigned int line = 0);

	void stopProcess();

private:
	void runProcess();
	WINDOW* createNewWindow(int height, int width, int startY, int startX);
	void destroyWindowin(WINDOW* window);

	WINDOW* m_RtFinderView, *m_UARTState, *m_ObjFinderView, *m_ControllerView;
	unsigned int m_width, m_height;
	bool m_State;

};
