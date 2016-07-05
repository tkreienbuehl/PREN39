#include "../header/RouteCalculation.hpp"

RouteCalculation::RouteCalculation(PrenController* controller) {
	m_rightSidePositiveSlope = false;
	m_leftSidePositiveSlope = true;
	m_RouteFound = false;
	m_CheckBend = false;
	m_CrossingFound = false;
	m_SearchCrossing = false;
	m_CrossingCnt = m_CrossingLinesCnt = 0;
	m_DistCorrAng = 0;
	m_CamPos = m_Controller->CAM_STRAIGHT;
	m_Controller = controller;
	m_CorrAng = 0;
	me = m_Controller->ROUTE_FINDER;
	m_Rows = 0;
	m_Cols = 0;
	m_CamPosCorrCnt = m_CamPosChangeWaitCnt = m_DistCorrAng = m_LineLostCnt = 0;
	m_RouteState = STRAIGHT;
	//Konstante Stellwerte
	MAX_PIX_DIFF = m_Controller->getPrenConfig()->MAX_PIX_DIFF;
	CAM_ANG_CORR_VAL = m_Controller->getPrenConfig()->CAM_ANG_CORR_VAL;
	CAM_POS_CHANGE_LIMIT = m_Controller->getPrenConfig()->CAM_POS_CHANGE_LIMIT;
	SLOPE_VAL_FOR_BEND_RIGHT = m_Controller->getPrenConfig()->SLOPE_VAL_FOR_BEND_RIGHT;
	SLOPE_VAL_FOR_BEND_LEFT = m_Controller->getPrenConfig()->SLOPE_VAL_FOR_BEND_LEFT;
	SLOPE_UPPER_VAL_FOR_STRAIGHT = m_Controller->getPrenConfig()->SLOPE_UPPER_VAL_FOR_STRAIGHT;
	SLOPE_LOWER_VAL_FOR_STRAIGHT = m_Controller->getPrenConfig()->SLOPE_LOWER_VAL_FOR_STRAIGHT;
	//
	m_RouteCalc = NULL;
}

RouteCalculation::~RouteCalculation() {
	delete m_RouteCalc;
}

int RouteCalculation::lineDetection(cv::Mat* edgeImg) {

	if (m_Rows == 0) {
		m_Cols = edgeImg->cols;
		m_Rows = edgeImg->rows;
		m_RouteCalc = new RouteCalcMath(m_Controller, m_Rows, m_Cols);
	}

	ushort maxPixDiff;
	if (!m_CheckBend) {
		maxPixDiff = MAX_PIX_DIFF << 1;
	}
	else
		maxPixDiff = MAX_PIX_DIFF;

    std::vector<cv::Vec4i> lines;
    std::vector<Line> savedLinesLeft, savedLinesRight, horizonalLines;
    cv::HoughLinesP(*edgeImg, lines, 1, CV_PI/180, 20, 40, 20 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
		Line l(lines[i]);
		float slope = l.getSlope();
		if (fabs(slope) > 0.95) {
			if (!m_Controller->getPrenConfig()->IS_ON_PI) {
				usleep(10* 1000);
			}
			l.adjustLineLength(m_Rows);
			//leftSide
			if (slope > 0.0 && m_CheckBend) {	// left side only if not in start zone
				if (l.getStartPoint().x < ((m_Cols >> 1) + MAX_PIX_DIFF) && l.getEndPoint().x < ((m_Cols >> 1) + MAX_PIX_DIFF)) {
					m_Controller->printString(l.getLineInfoString(),me,10);
					savedLinesLeft.push_back(l);
				}
			}
			//right side
			else {
				if (l.getStartPoint().x > ((m_Cols >> 1) - maxPixDiff) && l.getEndPoint().x > ((m_Cols >> 1) - maxPixDiff)) {
					m_Controller->printString(l.getLineInfoString(),me,15);
					savedLinesRight.push_back(l);
				}
			}
		}
		else if (slope <= 0.25) {
			horizonalLines.push_back(l);
		}
    }
    m_Controller->printString("", me, 40);
    analyzeHorizonalLines(&horizonalLines);
    lineFilter(edgeImg, savedLinesLeft, savedLinesRight);

	return m_CorrAng;
}

void RouteCalculation::startCheckForBend() {
	m_Controller->printString("Start checking for bend",me, 20);
	m_CheckBend = true;
}

void RouteCalculation::stopCheckForBend() {
	m_Controller->printString("Stop checking for bend",me, 20);
	m_CheckBend = false;
}

bool RouteCalculation::startCheckForCrossing() {
	if (!m_SearchCrossing) {
		m_Controller->printString("Start checking for crossings",me, 21);
		m_SearchCrossing = true;
		return true;
	}
	return false;
}

bool RouteCalculation::getRouteFoundState() {
	return m_RouteFound;
}

ushort RouteCalculation::getLineLostCnt() {
	return m_LineLostCnt;
}

int RouteCalculation::getCamCorrVal() {
	return m_CorrAng;
}

ushort RouteCalculation::getCrossingCnt() {
	return m_CrossingCnt;
}

void RouteCalculation::lineFilter(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines) {
    std::vector<Line> usedLinesLeft, usedLinesRight;
    short upperLeftMax, lowerLeftMax, upperRightMin, lowerRightMin;
    upperRightMin = lowerRightMin = m_Cols + 100;
	upperLeftMax = lowerLeftMax = -100;
    // left side
	for( size_t i = 0; i < leftLines.size(); i++ )
    {
		Line l = leftLines[i];
		float slope = l.getSlope();
		if ( (slope > 0 )) {
			if (l.getEndPoint().x >= upperLeftMax) {
				upperLeftMax = l.getEndPoint().x;
				usedLinesLeft.push_back(l);
			}
		}
    }

    // right side
    for( size_t i = 0; i < rightLines.size(); i++ )
    {
		Line l = rightLines[i];
		float slope = l.getSlope();;
		if ( (slope < 0 ) ) {
			if (l.getEndPoint().x <= upperRightMin) {
				upperRightMin = l.getEndPoint().x;
				usedLinesRight.push_back(l);
			}
		}
    }

    routeLocker(edgeImg, usedLinesLeft, usedLinesRight);
}

void RouteCalculation::routeLocker(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines) {
    // Fahrtrichtungsvektor
    cv::line(*edgeImg,cv::Point((edgeImg->cols >> 1),edgeImg->rows),
    		cv::Point((edgeImg->cols >> 1),(edgeImg->rows >> 1)),cv::Scalar(255), 2);

	if (leftLines.size() == 0 && rightLines.size() == 0) {
		m_Controller->printString("*** No lines found, keeping old direction ***", me, 7);
		m_LineLostCnt++;
		return;
	}
	if (leftLines.size() == 0) {
		m_Controller->printString("*** Right side found only ***", me, 7);
	}
	else if (rightLines.size() == 0) {
		m_Controller->printString("*** Left side found only ***", me, 7);
	}
	else {
		m_Controller->printString("", me, 7);
	}
	short max = -100;
	short min = m_Cols + 100;
	cv::Point lpt, rpt, lpt2, rpt2;
	for (unsigned short i = 0; i< leftLines.size() ; i++) {
		if (leftLines[i].getStartPoint().x > max) {
			lpt = leftLines[i].getStartPoint();
			lpt2 = leftLines[i].getEndPoint();
			max = lpt.y;
		}
	}
	max = 0;
	for (unsigned short i = 0; i< rightLines.size() ; i++) {
		if (rightLines[i].getStartPoint().x < min) {
			rpt = rightLines[i].getStartPoint();
			rpt2 = rightLines[i].getEndPoint();
			min = rpt.y;
		}
	}

	short lVal, rVal, med;
	lVal = rVal = med = 0;

	if (leftLines.size() > 0) {
		lVal = m_RouteCalc->calcLeftRefDistance(lpt);
		line(*edgeImg, lpt, lpt2, cv::Scalar(255), 3, CV_AA);
	}
	if (rightLines.size() > 0) {
		rVal = m_RouteCalc->calcRightRefDistance(rpt);
		line(*edgeImg, rpt, rpt2, cv::Scalar(255), 3, CV_AA);
	}
	if (leftLines.size() > 0 && rightLines.size() > 0) {
		med = (lVal-rVal)/2;
	}
	else if (rightLines.size() > 0) {
		med = -rVal;
	}
	else if (leftLines.size() > 0) {
		med = lVal;
	}
	m_CorrAng = m_RouteCalc->calcCorrAng(med);
	m_CorrAng += m_DistCorrAng;
	char numstr[128];

	if (!m_Controller->getPrenConfig()->IS_ON_PI) {
		sprintf(numstr, "Left side dist: %d ", lVal);
		m_Controller->printString(numstr, me, 2);

		sprintf(numstr, "Right side dist: %d ", rVal);
		m_Controller->printString(numstr, me, 3);

		sprintf(numstr, "Input angle to PID: %d ", m_CorrAng);
		m_Controller->printString(numstr, me, 4);
	}

	if (m_RouteFound && m_CheckBend) {
		checkRouteDirection(edgeImg, leftLines, rightLines);
	}

	m_RouteFound = true;
	m_LineLostCnt = 0;
}

void RouteCalculation::checkRouteDirection(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines) {

	if (m_CamPosChangeWaitCnt > 0 ) {
		m_CamPosChangeWaitCnt--;
		return;
	}
	char str[40];
	routeVals routeState = m_RouteState;
	if (m_CamPos == m_Controller->CAM_TURN_LEFT || m_CamPos == m_Controller->CAM_TURN_RIGHT) {
		routeState = checkBackStraight(edgeImg, leftLines, rightLines);
	}
	else if (m_CamPos == m_Controller->CAM_STRAIGHT) {
		routeState = checkBendLeft(edgeImg, rightLines);
		if (routeState != BEND_LEFT) {
			routeState = checkBendRight(edgeImg, leftLines);
		}
	}

	if (routeState == BEND_RIGHT) {
		m_Controller->setCameraPos(m_Controller->CAM_TURN_RIGHT);
		m_CamPos = m_Controller->CAM_TURN_RIGHT;
		m_Controller->printString("CAM_RIGHT", me, 24);
		m_DistCorrAng = CAM_ANG_CORR_VAL;
		m_CamPosChangeWaitCnt = m_Controller->getPrenConfig()->CAM_STATE_CHANGE_WAIT_LIMIT;
	}
	else if (routeState == BEND_LEFT) {
		m_Controller->setCameraPos(m_Controller->CAM_TURN_LEFT);
		m_CamPos = m_Controller->CAM_TURN_LEFT;
		m_Controller->printString("CAM_LEFT", me, 24);
		m_DistCorrAng = -CAM_ANG_CORR_VAL;
		m_CamPosChangeWaitCnt = m_Controller->getPrenConfig()->CAM_STATE_CHANGE_WAIT_LIMIT;
	}
	else if (routeState == STRAIGHT) {
		m_Controller->setCameraPos(m_Controller->CAM_STRAIGHT);
		m_CamPos = m_Controller->CAM_STRAIGHT;
		m_Controller->printString("CAM_STRAIGHT", me, 24);
		m_DistCorrAng = 0;
		m_CamPosChangeWaitCnt = m_Controller->getPrenConfig()->CAM_STATE_CHANGE_WAIT_LIMIT;
	}

	sprintf(str,"Cam position change counter: %d", m_CamPosCorrCnt);
	m_Controller->printString(str, me, 25);
	if (routeState != NOTHING) {
		m_RouteState = routeState;
	}
}

RouteCalculation::routeVals RouteCalculation::checkBendLeft(cv::Mat* edgeImg, vector<Line>& lines) {
	char str[80];
	float slope = -100;
	if (lines.size()>0) {
		slope = lines[0].getSlope();
		if (slope == lines[0].UNDEFINED) {
			return NOTHING;
		}
		sprintf(str,"Check for bend left: right side slope: %.2f", slope);
		m_Controller->printString(str, me, 22);
		if (abs(slope) < SLOPE_VAL_FOR_BEND_LEFT) {
			if (m_CamPosCorrCnt >= CAM_POS_CHANGE_LIMIT) {
				m_CamPosCorrCnt = 0;
				return BEND_LEFT;
			}
			m_CamPosCorrCnt++;
		}
		else {
			m_CamPosCorrCnt = 0;
			return NOTHING;
		}
	}
	return NOTHING;
}

RouteCalculation::routeVals RouteCalculation::checkBendRight(cv::Mat* edgeImg, vector<Line>& lines) {
	char str[80];
	float slope = -100;
	if (lines.size()>0) {
		slope = lines[0].getSlope();
		if (slope == lines[0].UNDEFINED) {
			return NOTHING;
		}
		sprintf(str,"Check for bend right: left side line slope: %.2f", slope);
		m_Controller->printString(str, me, 23);
		if (fabs(slope) < SLOPE_VAL_FOR_BEND_RIGHT) {
			if (m_CamPosCorrCnt >= CAM_POS_CHANGE_LIMIT) {
				m_CamPosCorrCnt = 0;
				return BEND_RIGHT;
			}
			m_CamPosCorrCnt++;
		}
		else {
			m_CamPosCorrCnt = 0;
			return NOTHING;
		}
	}
	return NOTHING;
}

RouteCalculation::routeVals RouteCalculation::checkBackStraight(cv::Mat* edgeImg, vector<Line>& leftLines, vector<Line>& rightLines) {
	float leftSlope = -100, rightSlope = -100;

	m_Controller->printString("checking for straight", me, 27);
	if (rightLines.size() == 0 || leftLines.size() == 0) {
		m_Controller->printString("Empty lines vector found", me, 27);
		return NOTHING;
	}

	leftSlope = leftLines[0].getSlope();
	rightSlope = rightLines[0].getSlope();

	if (m_RouteState == BEND_LEFT) {
		if (fabs(leftSlope) > SLOPE_LOWER_VAL_FOR_STRAIGHT && fabs(rightSlope) > SLOPE_UPPER_VAL_FOR_STRAIGHT) {
			if (m_CamPosCorrCnt >= CAM_POS_CHANGE_LIMIT) {
				m_CamPosCorrCnt = 0;
				return STRAIGHT;
			}
			m_CamPosCorrCnt++;
		}
	}
	else if (m_RouteState == BEND_RIGHT) {
		if (fabs(leftSlope) > SLOPE_UPPER_VAL_FOR_STRAIGHT && fabs(rightSlope) > SLOPE_LOWER_VAL_FOR_STRAIGHT) {
			if (m_CamPosCorrCnt >= CAM_POS_CHANGE_LIMIT) {
				m_CamPosCorrCnt = 0;
				return STRAIGHT;
			}
			m_CamPosCorrCnt++;
		}
	}

	return NOTHING;
}

void RouteCalculation::analyzeHorizonalLines(vector<Line>* lines) {
	if (lines->size() < 4 || !m_SearchCrossing) {
		return;
	}

	char str[70];
	sprintf(str,"Nr. of lines %d", static_cast<int>(lines->size()));
	m_Controller->printString(str, me, 34);
	ushort maxLength = 0;
	ushort length;
	float slope = 0.0;
	for (size_t i = 0; i< lines->size(); i++) {
		length = lines->at(i).getLength();
		slope = lines->at(i).getSlope();
		if ( length > maxLength) {
			maxLength = length;
		}
	}

	if (maxLength > 100 ) {
		m_Controller->printString("Crossing line found", me, 35);
		if (m_CrossingLinesCnt >= 5 && m_SearchCrossing) {
			m_CrossingCnt++;
			sprintf(str,"Nr. of crossings found: %d, length: %d ,\n slope: %1.2f", m_CrossingCnt, maxLength, slope);
			m_Controller->printString(str, me, 36);
			m_CrossingLinesCnt = 0;
			m_SearchCrossing = false;
		}
		else {
			m_CrossingLinesCnt++;
		}
	}
}
