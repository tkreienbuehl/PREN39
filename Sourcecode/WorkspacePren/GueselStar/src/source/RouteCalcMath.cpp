#include "../header/RouteCalcMath.hpp"

RouteCalcMath::RouteCalcMath(PrenController* controller, ushort rows, ushort cols) {
	m_Controller = controller;
	ROUTE_POS_CORR_VAL = m_Controller->getPrenConfig()->ROUTE_POS_CORR_VAL;

	m_Rows = rows;
	m_Cols = cols;

}

RouteCalcMath::~RouteCalcMath() {

}

short RouteCalcMath::calcLeftRefDistance(cv::Point pt) {
	short dist = (pt.y >> 1) + 40 + ROUTE_POS_CORR_VAL;
	short diff = ((m_Cols >> 1))-pt.x - dist;
	return diff;
}

short RouteCalcMath::calcRightRefDistance(cv::Point pt) {
	short dist = (pt.y >> 1) + 40 - ROUTE_POS_CORR_VAL;
	short diff = pt.x-((m_Cols >> 1)) - dist;
	return diff;
}

int RouteCalcMath::calcCorrAng(short distVal) {
	if (abs(distVal) <= 2) {
		return 0;
	}
	return static_cast<int>(acosf(static_cast<float>(distVal)/ 160.0f) * 180 / 3.1415926f) - 90;
}
