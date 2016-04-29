#pragma once

#include <iostream>
#include <stdlib.h>
#include "ConfigParser.hpp"

using namespace std;
class PrenConfiguration {
public:
	PrenConfiguration();
	~PrenConfiguration();
	bool START_LOCAL_VIEW;
	bool IS_DEBUG;
	bool IS_ON_PI;
	string TEST_IMG_DIR;
	int TEST_IMG_FIRST;
	int TEST_IMG_LAST;
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
	int P_PART;
	int I_PART;
	int D_PART;

private:
};
