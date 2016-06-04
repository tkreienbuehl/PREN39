#pragma once

#include <vector>
#include <exception>
#include <iostream>
#include <math.h>

using namespace std;

class Gradient {
public:
	Gradient(short partX, short partY);
	Gradient();
	~Gradient();

	void setValue(short partX, short partY);
	short getXValue();
	short getYValue();
	short getLength();

private:
	short m_xVal, m_yVal;
};
