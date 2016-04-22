#include "../header/ConfigParser.hpp"

ConfigParser::ConfigParser(string configFileName) {
	m_paraFile = configFileName;
}

ConfigParser::~ConfigParser() {

}

int ConfigParser::readIntParam(string paraName) {
    string str;
    ifstream file(m_paraFile.c_str());
    string paraVal;
    while (getline(file, str)) {
    	paraVal.clear();
    	if (str.find(paraName) != string::npos) {
    		parseParaVal(paraVal, str);
    		break;
    	}
    }
    return atoi(paraVal.c_str());
}

string ConfigParser::readStringParam(string paraName) {
    string str;
    ifstream file(m_paraFile.c_str());
    string paraVal;
    while (getline(file, str)) {
    	paraVal.clear();
    	if (str.find(paraName) != string::npos) {
    		parseParaVal(paraVal, str);
    		break;
    	}
    }
    return paraVal.c_str();
}

bool ConfigParser::readBoolParam(string paraName) {
    string str;
    ifstream file(m_paraFile.c_str());
    string paraVal;
    while (getline(file, str)) {
    	paraVal.clear();
    	if (str.find(paraName) != string::npos) {
    		parseParaVal(paraVal, str);
    		break;
    	}
    }
    if (paraVal.find("TRUE") != string::npos) {
    	return true;
    }
    return false;
}

void ConfigParser::parseParaVal(string& paraVal, string& strLine) {
	if (strLine.find("=") != string::npos) {
		int beg = strLine.find("\"");
		int end = strLine.find("\"",beg+1) - 1;
		paraVal = strLine.substr(beg+1,end-beg);
	}
}

