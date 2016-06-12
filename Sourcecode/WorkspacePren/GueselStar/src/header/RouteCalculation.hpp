#include "RouteCalcMath.hpp"

using namespace std;

class RouteCalculation {
public:
	RouteCalculation(PrenController* controller);
	~RouteCalculation();

	int lineDetection(cv::Mat* edgeImg);
	void startCheckForBend();
	bool getRouteFoundState();
	ushort getLineLostCnt();
	int getCamCorrVal();

private:

	enum routeVals {
		NOTHING,
		BEND_LEFT,
		BEND_RIGHT,
		STRAIGHT
	};

	void lineFilter(cv::Mat* changesMat, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines);
	void routeLocker(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines);
	void checkRouteDirection(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines);
	routeVals checkLeftRouteLimit(cv::Mat* edgeImg, vector<cv::Vec4i>& lines, float& xDist, float& yDist, ushort textStartPos);
	routeVals checkRightRouteLimit(cv::Mat* edgeImg, vector<cv::Vec4i>& lines, float& xDist, float& yDist, ushort textStartPos);
	void analyzeHorizonalLines(vector<cv::Vec4i>* lines);

	PrenController* m_Controller;
	RouteCalcMath* m_RouteCalc;
	int m_CorrAng;
	bool m_rightSidePositiveSlope;
	bool m_leftSidePositiveSlope;
	PrenController::classes me;
	ushort m_Rows, m_Cols;
	short m_DistCorrAng;
	ushort m_CamPos;
	ushort m_LineLostCnt;
	ushort m_CamPosCorrCnt;
	bool m_CheckBend;
	bool m_RouteFound;
	bool m_CrossingFound;
	ushort m_CrossingCnt;

	ushort MAX_PIX_DIFF;
	ushort CAM_ANG_CORR_VAL;
	ushort CAM_POS_CHANGE_LIMIT;
	float SLOPE_VAL_FOR_BEND;
	float SLOPE_VAL_FOR_STRAIGHT;

};
