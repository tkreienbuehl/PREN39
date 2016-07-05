#include "PrenController.hpp"
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "Line.hpp"

using namespace std;

class RouteCalcMath {
public:
	RouteCalcMath(PrenController* controller, ushort rows, ushort cols);
	~RouteCalcMath();

	short calcLeftRefDistance(cv::Point pt);
	short calcRightRefDistance(cv::Point pt);
	int calcCorrAng(short distVal);
private:
	PrenController* m_Controller;

	ushort m_Rows, m_Cols;
	int ROUTE_POS_CORR_VAL;

};
