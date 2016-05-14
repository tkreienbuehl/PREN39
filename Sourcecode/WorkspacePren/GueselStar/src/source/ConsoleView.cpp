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
	m_RtFinderView = createNewWindow(m_height-10,(m_width >> 1)-1,5,0);
	m_ObjFinderView = createNewWindow(m_height-10,(m_width >> 1)-1,5,(m_width >> 1)+1);
	m_ControllerView = createNewWindow(4,m_width,m_height-5,0);
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
	wclrtoeol(m_RtFinderView);
	wborder(m_RtFinderView, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	box(m_RtFinderView, 0 , 0);
	mvwprintw(m_RtFinderView, 1 + line, 1, text.c_str());
	wrefresh(m_RtFinderView);
}

void ConsoleView::setObjectFinderText(string text, unsigned int line) {
	wclrtoeol(m_ObjFinderView);
	wborder(m_ObjFinderView, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	box(m_ObjFinderView, 0 , 0);
	mvwprintw(m_ObjFinderView, 1 + line, 1, text.c_str());
	wrefresh(m_ObjFinderView);
}

void ConsoleView::setUARTStateText(string text, unsigned int line) {
	wclrtoeol(m_UARTState);
	wborder(m_UARTState, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	box(m_UARTState, 0 , 0);
	mvwprintw(m_UARTState, 1 + line, 1, text.c_str());
	wrefresh(m_UARTState);
}

void ConsoleView::setControllerText(string text, unsigned int line) {
	wclrtoeol(m_ControllerView);
	wborder(m_ControllerView, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	box(m_ControllerView, 0 , 0);
	mvwprintw(m_ControllerView, 1 + line, 1, text.c_str());
	wrefresh(m_ControllerView);
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
