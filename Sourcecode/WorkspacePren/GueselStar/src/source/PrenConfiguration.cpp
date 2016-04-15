#include "../header/PrenConfiguration.hpp"

using namespace std;

PrenConfiguration::PrenConfiguration() {

	config4cpp::Configuration *  cfg = config4cpp::Configuration::create();
	    const char *     scope = "";
	    const char *     configFile = "conf/PrenConfig.conf";

	    try {
	        cfg->parse(configFile);
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
	    } catch(const config4cpp::ConfigurationException & ex) {
	        cerr << ex.c_str() << endl;
	        cfg->destroy();
	    }


}

PrenConfiguration::~PrenConfiguration() {
	// TODO Auto-generated destructor stub
}
