#include "../header/RouteCalculation.hpp"

RouteCalculation::RouteCalculation(PrenController* controller) {
	m_rightSidePositiveSlope = true;
	m_leftSidePositiveSlope = false;
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
	m_CamPosCorrCnt = m_DistCorrAng = m_LineLostCnt = 0;
	//Konstante Stellwerte
	MAX_PIX_DIFF = m_Controller->getPrenConfig()->MAX_PIX_DIFF;
	CAM_ANG_CORR_VAL = m_Controller->getPrenConfig()->CAM_ANG_CORR_VAL;
	CAM_POS_CHANGE_LIMIT = m_Controller->getPrenConfig()->CAM_POS_CHANGE_LIMIT;
	SLOPE_VAL_FOR_BEND_LEFT = m_Controller->getPrenConfig()->SLOPE_VAL_FOR_BEND_LEFT;
	SLOPE_VAL_FOR_STRAIGHT_LEFT = m_Controller->getPrenConfig()->SLOPE_VAL_FOR_STRAIGHT_LEFT;
	SLOPE_VAL_FOR_BEND_RIGHT = m_Controller->getPrenConfig()->SLOPE_VAL_FOR_BEND_RIGHT;
	SLOPE_VAL_FOR_STRAIGHT_RIGHT = m_Controller->getPrenConfig()->SLOPE_VAL_FOR_STRAIGHT_RIGHT;
	//
	m_RouteCalc = NULL;
}

RouteCalculation::~RouteCalculation() {

}

int RouteCalculation::lineDetection(cv::Mat* edgeImg) {

	if (m_Rows == 0) {
		m_Cols = edgeImg->cols;
		m_Rows = edgeImg->rows;
		m_RouteCalc = new RouteCalcMath(m_Controller, m_Rows, m_Cols);
	}

    ushort lNegSignCnt, lPosSignCnt, rNegSignCnt, rPosSignCnt;
    lNegSignCnt = lPosSignCnt = rNegSignCnt = rPosSignCnt = 0;

    std::vector<cv::Vec4i> lines;
    std::vector<cv::Vec4i> savedLinesLeft, savedLinesRight, horizonalLines;
    cv::HoughLinesP(*edgeImg, lines, 1, CV_PI/180, 15, 40, 20 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
		cv::Vec4i l = lines[i];
		m_RouteCalc->setLineDirection(l);
		m_RouteCalc->adjustLineLength(l);
		if ((l[2] - l[0]) != 0) {
			int slope = (l[3] - l[1]) / (l[2] - l[0]);
			char str[20];
			bzero(str, sizeof(str));
			sprintf(str,"slope of Line. %d", slope);
			m_Controller->printString(str,me,9);
			if (abs(slope) >= 1) {
				//leftSide
				if (l[2] < ((m_Cols >> 1) + MAX_PIX_DIFF) && l[0] < ((m_Cols >> 1) + MAX_PIX_DIFF)) {
					m_RouteCalc->countPositiveNegativeSlopes(lPosSignCnt, lNegSignCnt, slope);
					savedLinesLeft.push_back(l);
				}
				//right side
				else if (l[2] > ((m_Cols >> 1) - MAX_PIX_DIFF) && l[0] > ((m_Cols >> 1) - MAX_PIX_DIFF)) {
					m_RouteCalc->countPositiveNegativeSlopes(rPosSignCnt, lNegSignCnt, slope);
					savedLinesRight.push_back(l);
				}
			}
			else if (slope == 0) {
				horizonalLines.push_back(l);
			}
		}
    }
    m_leftSidePositiveSlope = true;
    if (lPosSignCnt < lNegSignCnt) {
    	m_leftSidePositiveSlope = false;
    }
    m_Controller->printString("", me, 40);
    analyzeHorizonalLines(&horizonalLines);
    lineFilter(edgeImg, savedLinesLeft, savedLinesRight);

	return m_CorrAng;
}

void RouteCalculation::startCheckForBend() {
	m_Controller->printString("Start checking for bend",me, 18);
	m_CheckBend = true;
}

bool RouteCalculation::startCheckForCrossing() {
	if (!m_CheckBend) {
		return false;
	}
	if (!m_SearchCrossing) {
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

void RouteCalculation::lineFilter(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines) {
    std::vector<cv::Vec4i> usedLinesLeft, usedLinesRight;
    short upperLeftMax, lowerLeftMax, upperRightMin, lowerRightMin;
    upperRightMin = lowerRightMin = m_Cols + 100;
	upperLeftMax = lowerLeftMax = -100;
    // left side
    for( size_t i = 0; i < leftLines.size(); i++ )
    {
		cv::Vec4i l = leftLines[i];
		int slope = (l[3] - l[1]) / (l[2] - l[0]);
		if ( (slope > 0 && m_leftSidePositiveSlope) || (slope < 0 && !m_leftSidePositiveSlope)) {
			//if (l[0] >= lowerLeftMax && l[2] >= upperLeftMax) {
			if (l[2] >= upperLeftMax) {
				lowerLeftMax = l[0];
				upperLeftMax = l[2];
				usedLinesLeft.push_back(l);
			}
		}
    }

    // right side
    for( size_t i = 0; i < rightLines.size(); i++ )
    {
		cv::Vec4i l = rightLines[i];
		int slope = (l[3] - l[1]) / (l[2] - l[0]);
		if ( (slope > 0 && m_rightSidePositiveSlope) || (slope < 0 && !m_rightSidePositiveSlope)) {
			if (l[0] <= lowerRightMin && l[2] <= upperRightMin ) {
				lowerRightMin = l[0];
				upperRightMin = l[2];
				usedLinesRight.push_back(l);
			}
		}
    }

    routeLocker(edgeImg, usedLinesLeft, usedLinesRight);
}

void RouteCalculation::routeLocker(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines) {
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
		if (leftLines[i][0] > max) {
			lpt = cv::Point(leftLines[i][0],leftLines[i][1]);
			lpt2 = cv::Point(leftLines[i][2],leftLines[i][3]);
			max = leftLines[i][1];
		}
	}
	max = 0;
	for (unsigned short i = 0; i< rightLines.size() ; i++) {
		if (rightLines[i][0] < min) {
			rpt = cv::Point(rightLines[i][0],rightLines[i][1]);
			rpt2 = cv::Point(rightLines[i][2],rightLines[i][3]);
			min = rightLines[i][1];
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

void RouteCalculation::checkRouteDirection(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines) {
	float xDistRight, xDistLeft, yDistRight, yDistLeft;
	xDistRight = xDistLeft = yDistRight = yDistLeft = 0;
	bool isLimit = false;
	char str[40];
	routeVals lRtState, rRtState;
	lRtState = checkLeftRouteLimit(edgeImg, leftLines, xDistLeft, yDistLeft, 5);
	if (leftLines.size() == 0) {
		rRtState = checkRightRouteLimit(edgeImg, rightLines, xDistRight, yDistRight, m_Cols >> 1);
	}
	else {
		rRtState = NOTHING;
	}
	xDistRight = - xDistRight;

	if (m_CamPosCorrCnt >= CAM_POS_CHANGE_LIMIT) {
		isLimit = true;
	}
	if (isLimit) {
		m_CamPosCorrCnt = 0;
		if (m_CamPos == m_Controller->CAM_STRAIGHT && lRtState == BEND_RIGHT) {
			m_Controller->setCameraPos(m_Controller->CAM_TURN_RIGHT);
			m_CamPos = m_Controller->CAM_TURN_RIGHT;
		}
		else if (m_CamPos == m_Controller->CAM_STRAIGHT && rRtState == BEND_LEFT) {
			m_Controller->setCameraPos(m_Controller->CAM_TURN_LEFT);
			m_CamPos = m_Controller->CAM_TURN_LEFT;
		}
		else if (m_CamPos == m_Controller->CAM_TURN_RIGHT && lRtState == STRAIGHT) {
			m_Controller->setCameraPos(m_Controller->CAM_STRAIGHT);
			m_CamPos = m_Controller->CAM_STRAIGHT;
		}
		else if (m_CamPos == m_Controller->CAM_TURN_LEFT && rRtState == STRAIGHT) {
			m_Controller->setCameraPos(m_Controller->CAM_STRAIGHT);
			m_CamPos = m_Controller->CAM_STRAIGHT;
		}
	}
	else {
		m_Controller->printString("", me, 13);
	}
	if (m_CamPos == m_Controller->CAM_STRAIGHT) {
		//m_Controller->printString("CAM-State: CAM_STRAIGHT", me, 14);
		m_DistCorrAng = 0;
	}
	else if (m_CamPos == m_Controller->CAM_TURN_RIGHT) {
		//m_Controller->printString("CAM-State: CAM_TURN_RIGHT", me, 14);
		m_DistCorrAng = CAM_ANG_CORR_VAL;
	}
	else if (m_CamPos == m_Controller->CAM_TURN_LEFT) {
		//m_Controller->printString("CAM-State: CAM_TURN_LEFT", me, 14);
		m_DistCorrAng = -CAM_ANG_CORR_VAL;
	}
	sprintf(str,"Cam position change counter: %d", m_CamPosCorrCnt);
	m_Controller->printString(str, me, 15);
}

RouteCalculation::routeVals RouteCalculation::checkLeftRouteLimit(cv::Mat* edgeImg, vector<cv::Vec4i>& lines,
		float& xDist, float& yDist, ushort textStartPos) {
	char str[40];
	float slope = -100;
	if (lines.size()>0) {
		xDist = lines[0][2] - lines[0][0];
		yDist = abs(lines[0][3] - lines[0][1]);
		if (abs(xDist) > 5 && abs(yDist) > 20) {
			slope = yDist/xDist;
		}
		if (slope == -100) {
			return NOTHING;
		}
		//sprintf(str,"X: %.2f , Y: %.2f", xDist, yDist);
		//putText(*edgeImg, str, cv::Point(textStartPos, 50), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255));
		//m_Controller->printString(str, me, 16);
		sprintf(str,"Slope: %.2f", slope);
		m_Controller->printString(str, me, 17);
		if (abs(slope) < SLOPE_VAL_FOR_BEND_LEFT) {
			m_CamPosCorrCnt++;
			//m_Controller->printString("View line slope min Limit reached", me, 18);
			return BEND_RIGHT;
		}
		if (abs(slope) > SLOPE_VAL_FOR_STRAIGHT_LEFT) {
			m_CamPosCorrCnt++;
			//m_Controller->printString("View line slope max Limit reached", me, 18);
			return STRAIGHT;
		}
		else {
			m_CamPosCorrCnt = 0;
			return NOTHING;
		}
	}
	return NOTHING;
}

RouteCalculation::routeVals RouteCalculation::checkRightRouteLimit(cv::Mat* edgeImg, vector<cv::Vec4i>& lines,
		float& xDist, float& yDist, ushort textStartPos) {
	//char str[40];
	float slope = -100;
	if (lines.size()>0) {
		xDist = lines[0][2] - lines[0][0];
		yDist = abs(lines[0][3] - lines[0][1]);
		if (abs(xDist) > 5 && abs(yDist) > 20) {
			slope = yDist/xDist;
		}
		if (slope == -100) {
			return NOTHING;
		}
		//sprintf(str,"X: %.2f , Y: %.2f", xDist, yDist);
		//putText(*edgeImg, str, cv::Point(textStartPos, 50), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255));
		//m_Controller->printString(str, me, 16);
		//sprintf(str,"Slope: %.2f", slope);
		//m_Controller->printString(str, me, 17);
		if (abs(slope) < SLOPE_VAL_FOR_BEND_RIGHT) {
			m_CamPosCorrCnt++;
			//m_Controller->printString("View line slope min Limit reached", me, 18);
			return BEND_LEFT;
		}
		if (abs(slope) > SLOPE_VAL_FOR_STRAIGHT_RIGHT) {
			m_CamPosCorrCnt++;
			//m_Controller->printString("View line slope max Limit reached", me, 18);
			return STRAIGHT;
		}
		else {
			m_CamPosCorrCnt = 0;
			return NOTHING;
		}
	}
	return NOTHING;
}

void RouteCalculation::analyzeHorizonalLines(vector<cv::Vec4i>* lines) {
	if (lines->size() < 4 || !m_SearchCrossing) {
		return;
	}

	char str[50];
	sprintf(str,"Nr. of lines %d", static_cast<int>(lines->size()));
	m_Controller->printString(str, me, 34);
	ushort maxLength = 0;
	ushort length;
	vector<cv::Vec4i>::const_iterator itr;
	itr = lines->begin();
	while (itr != lines->end()) {
		length = m_RouteCalc->calcLineLength(*itr);
		if ( length > maxLength) {
			maxLength = length;
		}
		itr++;
	}

	if (maxLength > 500 ) {
		m_Controller->printString("Crossing line found", me, 35);
		if (m_CrossingLinesCnt >= 5) {
			m_CrossingCnt++;
			sprintf(str,"Nr. of crossings found: %d %d", m_CrossingCnt, maxLength);
			m_Controller->printString(str, me, 36);
			m_CrossingLinesCnt = 0;
			m_SearchCrossing = false;
		}
		else {
			m_CrossingLinesCnt++;
		}
	}
}
