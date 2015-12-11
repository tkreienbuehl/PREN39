#pragma once

#include <iostream>

#include "RouteFinder.hpp"
#include "ObjectFinder.hpp"

class PictureViewer {
public:
	PictureViewer(RouteFinder* rtFinder, ObjectFinder* objFinder);
	~PictureViewer();

	static void* startThread(void* threadId);

	void stopViewer();

private:

	void runProcess();

	RouteFinder* m_rtFinder;
	ObjectFinder* m_objectFinder;

	bool m_state;

};
