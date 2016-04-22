#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

class ConfigParser {
public:
	ConfigParser(string configFileName);
	~ConfigParser();

	int readIntParam(string paraName);

	bool readBoolParam(string paraName);

private:
	void parseParaVal(string& paraVal, string& strLine);

	string m_paraFile;


};
