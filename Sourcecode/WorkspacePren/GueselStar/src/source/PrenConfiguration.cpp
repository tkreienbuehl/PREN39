#include "../header/PrenConfiguration.hpp"

using namespace std;

PrenConfiguration::PrenConfiguration() {

	    string    configFile = "conf/PrenConfig.conf";
	    ConfigParser parser(configFile);

	    START_LOCAL_VIEW = parser.readBoolParam("START_LOCAL_VIEW");
	    IS_ON_PI = parser.readBoolParam("IS_ON_PI");
	    IS_DEBUG = parser.readBoolParam("IS_DEBUG");
	    IS_ON_IDE = parser.readBoolParam("IS_ON_IDE");
	    TEST_IMG_DIR = parser.readStringParam("TEST_IMG_DIR");
	    TEST_IMG_FIRST = parser.readIntParam("TEST_IMG_FIRST");
	    TEST_IMG_LAST = parser.readIntParam("TEST_IMG_LAST");

	    BLUE_RANGE_H_LOW = parser.readIntParam("BLUE_RANGE_H_LOW");
        BLUE_RANGE_H_HIGH = parser.readIntParam("BLUE_RANGE_H_HIGH");
        BLUE_RANGE_S_LOW = parser.readIntParam("BLUE_RANGE_S_LOW");
		BLUE_RANGE_S_HIGH = parser.readIntParam("BLUE_RANGE_S_HIGH");
		BLUE_RANGE_V_LOW = parser.readIntParam("BLUE_RANGE_V_LOW");
		BLUE_RANGE_V_HIGH = parser.readIntParam("BLUE_RANGE_V_HIGH");
		GREEN_RANGE_H_LOW = parser.readIntParam("GREEN_RANGE_H_LOW");
		GREEN_RANGE_H_HIGH = parser.readIntParam("GREEN_RANGE_H_HIGH");
		GREEN_RANGE_S_LOW = parser.readIntParam("GREEN_RANGE_S_LOW");
		GREEN_RANGE_S_HIGH = parser.readIntParam("GREEN_RANGE_S_HIGH");
		GREEN_RANGE_V_LOW = parser.readIntParam("GREEN_RANGE_V_LOW");
		GREEN_RANGE_V_HIGH = parser.readIntParam("GREEN_RANGE_V_HIGH");
		MAX_DISTANCE_TO_OBJECT = parser.readIntParam("MAX_DISTANCE_TO_OBJECT");
		MAX_DISTANCE_TO_OBJECT_CROSSING = parser.readIntParam("MAX_DISTANCE_TO_OBJECT_CROSSING");
		NUM_VALUES_UNTIL_RELEVANT = parser.readIntParam("NUM_VALUES_UNTIL_RELEVANT");
		REFERENCE_HEIGHT = parser.readIntParam("REFERENCE_HEIGHT");
		REFERENCE_DISTANCE = parser.readIntParam("REFERENCE_DISTANCE");
		P_PART = parser.readUShortParam("P-PART");
		I_PART = parser.readUShortParam("I-PART");
		D_PART = parser.readUShortParam("D-PART");
		MINLENGTH = parser.readUShortParam("MINLENGHT");
		MINXDIFF = parser.readUShortParam("MINXDIFF");
		MINYDIFF = parser.readUShortParam("MINYDIFF");
		NROFLINES = parser.readUShortParam("NROFLINES");
		MAX_PIX_DIFF = parser.readUShortParam("MAX_PIX_DIFF");
		MIN_RT_WIDTH = parser.readUShortParam("MIN_RT_WIDTH");
		IF_NAME = parser.readStringParam("IF_NAME");
}

PrenConfiguration::~PrenConfiguration() {
}
