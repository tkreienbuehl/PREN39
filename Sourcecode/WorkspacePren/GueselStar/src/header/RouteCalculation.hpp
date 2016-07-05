#include "RouteCalcMath.hpp"

using namespace std;

class RouteCalculation {
public:
	RouteCalculation(PrenController* controller);
	~RouteCalculation();

	int lineDetection(cv::Mat* edgeImg);
	void startCheckForBend();
	void stopCheckForBend();
	bool startCheckForCrossing();
	bool getRouteFoundState();
	ushort getLineLostCnt();
	int getCamCorrVal();
	ushort getCrossingCnt();

private:

	enum routeVals {
		NOTHING,
		BEND_LEFT,
		BEND_RIGHT,
		STRAIGHT
	};

	void lineFilter(cv::Mat* changesMat, vector<Line>& leftLines, vector<Line>& rightLines);
	void routeLocker(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines);
	void checkRouteDirection(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines);
	routeVals checkBendLeft(cv::Mat* edgeImg, vector<Line>& lines);
	routeVals checkBendRight(cv::Mat* edgeImg, vector<Line>& lines);
	routeVals checkBackStraight(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines);
	void analyzeHorizonalLines(vector<Line>* lines);

	PrenController* m_Controller;
	RouteCalcMath* m_RouteCalc;
	int m_CorrAng;
	bool m_rightSidePositiveSlope;
	bool m_leftSidePositiveSlope;
	PrenController::classes me;
	ushort m_Rows, m_Cols;
	short m_DistCorrAng;
	ushort m_CamPos;
	routeVals m_RouteState;
	ushort m_LineLostCnt;
	ushort m_CamPosCorrCnt;
	ushort m_CamPosChangeWaitCnt;
	bool m_CheckBend;
	bool m_RouteFound;
	bool m_CrossingFound;
	bool m_SearchCrossing;
	ushort m_CrossingCnt, m_CrossingLinesCnt;

	ushort MAX_PIX_DIFF;
	ushort CAM_ANG_CORR_VAL;
	ushort CAM_POS_CHANGE_LIMIT;
	float SLOPE_VAL_FOR_BEND_RIGHT;
	float SLOPE_VAL_FOR_BEND_LEFT;
	float SLOPE_UPPER_VAL_FOR_STRAIGHT;
	float SLOPE_LOWER_VAL_FOR_STRAIGHT;

};
