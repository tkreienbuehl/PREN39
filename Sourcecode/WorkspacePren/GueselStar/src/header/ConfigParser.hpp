#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

class ConfigParser {
public:
	ConfigParser(string configFileName);
	~ConfigParser();

	int readIntParam(string paraName);
	ushort readUShortParam(string paraName);
	string readStringParam(string paraName);
	bool readBoolParam(string paraName);
	float readFloatParam(string paraName);

private:
	void parseParaVal(string& paraVal, string& strLine);

	string m_paraFile;


};
