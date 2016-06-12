#include "../header/RouteCalcMath.hpp"

RouteCalcMath::RouteCalcMath(PrenController* controller, ushort rows, ushort cols) {
	m_Controller = controller;
	ROUTE_POS_CORR_VAL = m_Controller->getPrenConfig()->ROUTE_POS_CORR_VAL;

	m_Rows = rows;
	m_Cols = cols;

}

RouteCalcMath::~RouteCalcMath() {

}

void RouteCalcMath::setLineDirection(cv::Vec4i& line) {
	cv::Point p1(line[0], line[1]);
	cv::Point p2(line[2], line[3]);
	cv::Point tmp;
	if (p1.y < p2.y) {
		tmp = p2;
		p2 = p1;
		p1 = tmp;
	}
}

void RouteCalcMath::adjustLineLength(cv::Vec4i& line) {
	cv::Point pt1(line[0], line[1]);
	cv::Point pt2(line[2], line[3]);

	adjustPoint(m_Rows, &pt1, line);
	adjustPoint((m_Rows >> 1), &pt2, line);
}

short RouteCalcMath::calcLeftRefDistance(cv::Point pt) {
	short dist = (pt.y >> 1) + 40;
	short diff = ((m_Cols >> 1) + ROUTE_POS_CORR_VAL)-pt.x - dist;
	return diff;
}

short RouteCalcMath::calcRightRefDistance(cv::Point pt) {
	short dist = (pt.y >> 1) + 40;
	short diff = pt.x-((m_Cols >> 1) - ROUTE_POS_CORR_VAL) - dist;
	return diff;
}

int RouteCalcMath::calcCorrAng(short distVal) {
	if (abs(distVal) <= 2) {
		return 0;
	}
	return static_cast<int>(acosf(static_cast<float>(distVal)/ 160.0f) * 180 / 3.1415926f) - 90;
}

void RouteCalcMath::countPositiveNegativeSlopes(ushort& posCnt, ushort& negCnt, int slope) {
	if (slope < 0) {
		negCnt++;
	}
	else {
		posCnt++;
	}
}


void RouteCalcMath::adjustPoint(ushort refLinePosY, cv::Point* refPoint, cv::Vec4i& outputLine) {
	cv::Point pt1(outputLine[0], outputLine[1]);
	cv::Point pt2(outputLine[2], outputLine[3]);
	short distX, distY, newDistX, newDistY;

	if ( abs(refPoint->y - refLinePosY) < 2 ) {
		return;
	}
	distX = pt1.x - pt2.x;
	distY = pt1.y - pt2.y;
	if ( distY <= 2 ) {
		return;
	}
	newDistY = refLinePosY - refPoint->y;
	newDistX = (newDistY * distX) / distY;

	if (*refPoint == pt1) {
		outputLine[0] += newDistX;
		outputLine[1] += newDistY;
	}
	else {
		outputLine[2] += newDistX;
		outputLine[3] += newDistY;
	}
}

uint RouteCalcMath::calcLineLength(const cv::Vec4i& line) {
	cv::Point pt1(line[0], line[1]);
	cv::Point pt2(line[2], line[3]);
	short distX, distY;

	distX = pt1.x - pt2.x;
	distY = pt1.y - pt2.y;

	return sqrt(distX * distX + distY * distY);
}
