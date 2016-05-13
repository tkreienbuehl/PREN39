#include "../header/ConsoleView.h"

ConsoleView::ConsoleView() {
	m_State = true;
	initscr();
	raw();
	keypad(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	getmaxyx(stdscr, m_height, m_width);
	attron(A_BOLD);
	printw("Welcome");
	mvchgat(0, 0, -1, A_BLINK, 1, NULL);
	attroff(A_BOLD);
	move(1,0);
	printw("Drücke 'F1' um die View zu schliessen");
	move(2,0);
	printw("Das Programm startet in wenigen Augenblicken");
	refresh();
	usleep(1000000);
	clear();
	refresh();
	m_UARTState = createNewWindow(4, m_width,0,0);
	m_RtFinderView = createNewWindow(m_height-5,m_width,5,0);
	usleep(3000000);
	refresh();
}

ConsoleView::~ConsoleView() {
	destroyWindowin(m_RtFinderView);
	destroyWindowin(m_UARTState);
}

void* ConsoleView::startThread(void* threadId) {
	reinterpret_cast<ConsoleView*>(threadId)->runProcess();
	cout << "Thread ended " << endl;
	return NULL;
}

void ConsoleView::setRouteFinderText(string text, unsigned int line) {

	//wclear(m_RtFinderView);
	wborder(m_RtFinderView, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	mvwprintw(m_RtFinderView, 1 + line, 1, text.c_str());
	wrefresh(m_RtFinderView);
}

void ConsoleView::stopProcess() {
	m_State = false;
}

void ConsoleView::runProcess() {
	usleep(10000);
	wclear(m_RtFinderView);
	while (m_State) {
		usleep(500);
	}
	destroyWindowin(m_RtFinderView);
	destroyWindowin(m_UARTState);
	endwin();
}

WINDOW* ConsoleView::createNewWindow(int height, int width, int startY, int startX) {
	WINDOW* win;
	win = newwin(height, width, startY, startX);
	box(win, 0 , 0);
	wrefresh(win);

	return win;
}

void ConsoleView::destroyWindowin(WINDOW* window) {
	wclear(window);
	wborder(window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(window);
	delwin(window);
}
