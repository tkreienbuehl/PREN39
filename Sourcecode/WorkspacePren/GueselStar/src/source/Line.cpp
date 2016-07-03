#include "../header/Line.hpp"

Line::Line(cv::Vec4i points) {
	m_StartPt = cv::Point(points[0], points[1]);
	m_EndPt = cv::Point(points[2], points[3]);
	m_Length = 0;
	calcSlope();
	calcLength();
}

Line::~Line() {

}

ushort Line::getLength() {
	if (m_Length == 0) {
		calcLength();
	}
	return m_Length;
}

float Line::getSlope() {
	return m_Slope;
}

void Line::adjustLineLength(ushort rows) {
	corrLineDirection();
	adjustPoint(rows, true);
	adjustPoint((rows >> 1), false);
	calcLength();
	calcSlope();
}

cv::Point Line::getStartPoint() {
	return m_StartPt;
}

cv::Point Line::getEndPoint() {
	return m_EndPt;
}

short Line::getDistX() {
	return m_DistX;
}

short Line::getDistY() {
	return m_DistY;
}

string Line::getLineInfoString() {
	char str[256];
	sprintf(str,"Line: lenght %d, slope %.2f,\n start point X: %d, Y: %d,\n end point X: %d, Y: %d",
			m_Length, m_Slope, m_StartPt.x, m_StartPt.y, m_EndPt.x, m_EndPt.y);
	string rstr = str;
	return rstr;
}

void Line::calcLength() {
	m_Length = sqrt(pow(m_DistX,2.0) + pow(m_DistY,2.0));
}

void Line::calcSlope() {
	calcDistXnY();
	if (abs(m_DistX) < 2) {
		m_Slope = UNDEFINED;
		return;
	}
	m_Slope = static_cast<float>(m_DistY) / m_DistX;
}

void Line::corrLineDirection() {
	cv::Point tmp;
	if (m_StartPt.y < m_EndPt.y) {
		tmp = m_EndPt;
		m_EndPt = m_StartPt;
		m_StartPt = tmp;
	}
	calcDistXnY();
}

void Line::adjustPoint(ushort refLinePosY, bool startPoint) {
	short newDistX, newDistY;
	cv::Point refPoint;
	if (startPoint) {
		refPoint = m_StartPt;
	}
	else {
		refPoint = m_EndPt;
	}

	if (abs(m_DistY) <= 2 || abs(refPoint.y - refLinePosY) < 2 ) {
		return;
	}

	newDistY = refLinePosY - refPoint.y;
	newDistX = -(newDistY * m_DistX) / m_DistY;

	if (startPoint) {
		m_StartPt.x += newDistX;
		m_StartPt.y += newDistY;
		calcSlope();
	}
	else {
		m_EndPt.x += newDistX;
		m_EndPt.y += newDistY;
		calcSlope();
	}
}

void Line::calcDistXnY() {
	m_DistX = m_EndPt.x - m_StartPt.x;
	m_DistY = m_StartPt.y - m_EndPt.y;
}
