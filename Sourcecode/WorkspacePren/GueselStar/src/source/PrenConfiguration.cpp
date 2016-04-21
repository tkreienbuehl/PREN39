#include "../header/PrenConfiguration.hpp"

using namespace std;

PrenConfiguration::PrenConfiguration() {

	config4cpp::Configuration*  cfg = config4cpp::Configuration::create();
	    const char*     scope = "";
	    const char*     configFile = "conf/PrenConfig.conf";

	    try {
	        cfg->parse(configFile);
	        START_LOCAL_VIEW = strcmp(cfg->lookupString(scope, "START_LOCAL_VIEW"),"TRUE") == 0 ? true : false;
	        IS_DEBUG = strcmp(cfg->lookupString(scope, "IS_DEBUG"),"TRUE") == 0 ? true : false;
	        IS_ON_PI = strcmp(cfg->lookupString(scope, "IS_ON_PI"),"TRUE") == 0 ? true : false;

	        cout << "Auf PI?: " << IS_ON_PI << endl;

	        BLUE_RANGE_H_LOW = atoi(cfg->lookupString(scope, "BLUE_RANGE_H_LOW"));
	        BLUE_RANGE_H_HIGH = atoi(cfg->lookupString(scope, "BLUE_RANGE_H_HIGH"));
	        BLUE_RANGE_S_LOW = atoi(cfg->lookupString(scope, "BLUE_RANGE_S_LOW"));
			BLUE_RANGE_S_HIGH = atoi(cfg->lookupString(scope, "BLUE_RANGE_S_HIGH"));
			BLUE_RANGE_V_LOW = atoi(cfg->lookupString(scope, "BLUE_RANGE_V_LOW"));
			BLUE_RANGE_V_HIGH = atoi(cfg->lookupString(scope, "BLUE_RANGE_V_HIGH"));
			GREEN_RANGE_H_LOW = atoi(cfg->lookupString(scope, "GREEN_RANGE_H_LOW"));
			GREEN_RANGE_H_HIGH = atoi(cfg->lookupString(scope, "GREEN_RANGE_H_HIGH"));
			GREEN_RANGE_S_LOW = atoi(cfg->lookupString(scope, "GREEN_RANGE_S_LOW"));
			GREEN_RANGE_S_HIGH = atoi(cfg->lookupString(scope, "GREEN_RANGE_S_HIGH"));
			GREEN_RANGE_V_LOW = atoi(cfg->lookupString(scope, "GREEN_RANGE_V_LOW"));
			GREEN_RANGE_V_HIGH = atoi(cfg->lookupString(scope, "GREEN_RANGE_V_HIGH"));
			MAX_DISTANCE_TO_OBJECT = atoi(cfg->lookupString(scope, "MAX_DISTANCE_TO_OBJECT"));
			MAX_DISTANCE_TO_OBJECT_CROSSING = atoi(cfg->lookupString(scope, "MAX_DISTANCE_TO_OBJECT_CROSSING"));
			REFERENCE_HEIGHT = atoi(cfg->lookupString(scope, "REFERENCE_HEIGHT"));
			REFERENCE_DISTANCE = atoi(cfg->lookupString(scope, "REFERENCE_DISTANCE"));
			cfg->destroy();
	    } catch(const config4cpp::ConfigurationException & ex) {
	        cerr << ex.c_str() << endl;
	        cfg->destroy();
	    }
}

PrenConfiguration::~PrenConfiguration() {
	// TODO Auto-generated destructor stub
}
