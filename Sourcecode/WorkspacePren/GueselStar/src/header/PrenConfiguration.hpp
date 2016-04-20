#pragma once

#include <iostream>
#include <config4cpp/Configuration.h>
#include <stdlib.h>

using namespace std;
class PrenConfiguration {
public:
	PrenConfiguration();
	~PrenConfiguration();
	bool START_LOCAL_VIEW;
	int BLUE_RANGE_H_LOW;
	int BLUE_RANGE_H_HIGH;
	int BLUE_RANGE_S_LOW;
	int BLUE_RANGE_S_HIGH;
	int BLUE_RANGE_V_LOW;
	int BLUE_RANGE_V_HIGH;
	int GREEN_RANGE_H_LOW;
	int GREEN_RANGE_H_HIGH;
	int GREEN_RANGE_S_LOW;
	int GREEN_RANGE_S_HIGH;
	int GREEN_RANGE_V_LOW;
	int GREEN_RANGE_V_HIGH;
	int MAX_DISTANCE_TO_OBJECT;
	int MAX_DISTANCE_TO_OBJECT_CROSSING;
	int REFERENCE_HEIGHT;
	int REFERENCE_DISTANCE;

private:

};
