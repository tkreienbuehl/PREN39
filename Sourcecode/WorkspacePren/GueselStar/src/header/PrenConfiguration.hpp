#pragma once

#include <iostream>
#include <stdlib.h>
#include "ConfigParser.hpp"
#include <stdint.h>

using namespace std;
class PrenConfiguration {
public:
	PrenConfiguration();
	~PrenConfiguration();
	bool START_LOCAL_VIEW;
	bool IS_DEBUG;
	bool IS_ON_PI;
	bool IS_ON_IDE;
	string TEST_IMG_DIR;
	string IF_NAME;
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
	int NUM_VALUES_UNTIL_RELEVANT;
	int REFERENCE_HEIGHT;
	int REFERENCE_DISTANCE;
	ushort P_PART;
	ushort I_PART;
	ushort D_PART;
	ushort MINLENGTH;
	ushort MINXDIFF;
	ushort MINYDIFF;
	ushort NROFLINES;
	ushort MAX_PIX_DIFF;
	ushort MIN_RT_WIDTH;
	ushort MAX_SPEED;
	ushort MAX_NR_OF_IMAGES;
	ushort LINE_LOST_LIMIT;
	ushort CAM_POS_CHANGE_LIMIT;
	int ROUTE_POS_CORR_VAL;
	ushort CAM_ANG_CORR_VAL;
	ushort NR_OF_IMS_FOR_CHECK_BEND;
	float SLOPE_VAL_FOR_BEND;
	float SLOPE_VAL_FOR_STRAIGHT;

private:
};
