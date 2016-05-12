#include "../header/ConsoleView.h"

ConsoleView::ConsoleView() {
	initscr();
	printw("Welcome");
	refresh();
	usleep(1000000);
	clear();
	printw("Gueselstar rocks: Tests passed %i <- WOOOHOOOO!!!!", 100);
	refresh();
}

ConsoleView::~ConsoleView() {
	endwin();
}

void* ConsoleView::startThread(void* threadId) {
	reinterpret_cast<ConsoleView*>(threadId)->runProcess();
	cout << "Thread ended " << endl;
	return NULL;
}

void ConsoleView::setRouteFinderText(string text) {
	printw(text.c_str());
}

void ConsoleView::runProcess() {

}
