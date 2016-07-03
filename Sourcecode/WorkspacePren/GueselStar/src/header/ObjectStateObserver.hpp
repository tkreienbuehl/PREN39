#pragma once

using namespace std;

class ObjectStateObserver {
	public:
		ObjectStateObserver() {};
		virtual void updateCrossingState(bool crossingAhead) = 0;
		virtual void updateObjectOnLaneState(bool objectOnLane) = 0;
		virtual ~ObjectStateObserver() {};
};
