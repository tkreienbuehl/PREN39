#include "PrenController.hpp"
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

class RouteCalcMath {
public:
	RouteCalcMath(PrenController* controller, ushort rows, ushort cols);
	~RouteCalcMath();

	void setLineDirection(cv::Vec4i& line);
	void adjustLineLength(cv::Vec4i& line);
	short calcLeftRefDistance(cv::Point pt);
	short calcRightRefDistance(cv::Point pt);
	int calcCorrAng(short distVal);
	void countPositiveNegativeSlopes(ushort& posCnt, ushort& negCnt, int slope);
	uint calcLineLength(const cv::Vec4i& line);

private:

	void adjustPoint(ushort refLinePosY, cv::Point* refPoint, cv::Vec4i& outputLine);

	PrenController* m_Controller;

	ushort m_Rows, m_Cols;
	int ROUTE_POS_CORR_VAL;

};
