#pragma once

#include <cv.h>
#include <memory>
#include <iostream>

using namespace std;

class Line {
public:
	Line(cv::Vec4i points);
	~Line();

	ushort getLength();
	float getSlope();
	void adjustLineLength(ushort rows);
	cv::Point getStartPoint();
	cv::Point getEndPoint();
	short getDistX();
	short getDistY();
	string getLineInfoString();

	enum SLOPE {
		UNDEFINED = 100
	};

private:

	void calcLength();
	void calcSlope();
	void corrLineDirection();
	void adjustPoint(ushort refLinePosY, bool startPoint);
	void calcDistXnY();

	cv::Point m_StartPt;
	cv::Point m_EndPt;
	float m_Slope;
	ushort m_Length;
	short m_DistX, m_DistY;

};
