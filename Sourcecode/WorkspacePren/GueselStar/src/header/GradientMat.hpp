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


class GradientMat {
private:
	GradientMat(short rows, short cols);
public:
	static GradientMat* getInstance(short rows, short cols);
	~GradientMat();

	void setValue(short row, short col, Gradient val);
	Gradient getValue(short row, short col);

private:
	static GradientMat* m_TheInstance;
	Gradient** m_matPtr;
	short m_Rows, m_Cols;
};
