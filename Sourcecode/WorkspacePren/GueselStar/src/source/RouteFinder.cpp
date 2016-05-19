#include "../header/RouteFinder.hpp"

RouteFinder::RouteFinder(PrenController* controller, PictureCreator* picCreator) {
	PrenConfiguration conf;
	MINLENGTH = conf.MINLENGTH;
	MINXDIFF = conf.MINXDIFF;
	MINYDIFF = conf.MINYDIFF;
	NROFLINES = conf.NROFLINES;
	MAX_PIX_DIFF = conf.MAX_PIX_DIFF;
	MIN_RT_WIDTH = conf.MIN_RT_WIDTH;
	MAX_ENGINE_SPEED = conf.MAX_SPEED;
	MAX_NR_OF_IMAGES = conf.MAX_NR_OF_IMAGES;
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_GradMat = NULL;
	m_State = false;
	pthread_mutex_init(&m_mutex, NULL);
	m_rightSidePositiveSlope = true;
	m_leftSidePositiveSlope = false;
	m_Cols = 0;
	m_Rows = 0;
	m_pidCalc = new PIDCalculation(m_Controller);
	m_outStr = "";
	m_RouteFound = false;
	m_Driving = false;
	me = m_Controller->ROUTE_FINDER;
}

RouteFinder::~RouteFinder() {
	delete m_GradMat;
	delete m_pidCalc;
}

void* RouteFinder::staticEntryPoint(void* threadId) {
	reinterpret_cast<RouteFinder*>(threadId)->runProcess();
	return NULL;
}

cv::Mat RouteFinder::getOriginalImage() {
	return m_PicCreator->GetImage();
}

cv::Mat RouteFinder::getGrayImage() {
	pthread_mutex_lock(&m_mutex);
		cv::Mat retImg = m_GrayImg;
	pthread_mutex_unlock(&m_mutex);
	return retImg;
}

cv::Mat RouteFinder::getFilteredImage() {
	pthread_mutex_lock(&m_mutex);
		cv::Mat retImg = m_FinalFltImg;
	pthread_mutex_unlock(&m_mutex);
	return retImg;
}

int RouteFinder::runProcess() {
	cv::Mat grayImg, image;

	m_Controller->printString("Start", me, 1);
    for(int i = 0; i<MAX_NR_OF_IMAGES; i++) {
        image = m_PicCreator->GetImage();

        if (!image.empty()) {
			cv::cvtColor(image,grayImg,CV_BGR2GRAY);
			cv::Mat fltImg = cv::Mat::zeros(grayImg.rows, grayImg.cols, CV_8UC1);
			m_Rows = grayImg.rows;
			m_Cols = grayImg.cols * grayImg.channels();
			edgeDetection(&grayImg, &fltImg);
			m_GrayImg = grayImg;
			m_FinalFltImg = fltImg;
			if (i%100 == 0) {
				char str[50];
				bzero(str,sizeof(str));
				sprintf(str,"image processed nr: %d",i);
				m_Controller->printString(str, me, 5);
			}
			if (m_RouteFound && !m_Driving) {
				m_Controller->setEngineSpeed(MAX_ENGINE_SPEED);
				m_Driving = true;
			}
        }
        else {
        	i--;
        }
    }
    m_Controller->printString("Ende :)", me ,6);
    m_Controller->setState(m_Controller->END);
	string bye = "Now I've done my job, have fun with your pics ;)";
	m_Controller->printString(bye, me, 6);

	return 0;

}

void RouteFinder::bubbleSort(std::vector<unsigned short>* vals) {
	unsigned short i, j, tmp;
	for (i=0 ; i<vals->size(); i++) {
		for (j=0; j<vals->size()-(i+1) ; j++) {
			if (vals->at(j+1) < vals->at(j)) {
				tmp = vals->at(j);
				vals->at(j) = vals->at(j+1);
				vals->at(j+1) = tmp;
			}
		}
	}
}

bool RouteFinder::compareTolerance(unsigned short refVal, unsigned short compVal) {
	if (abs(abs(refVal)-abs(compVal)) < MAX_PIX_DIFF) {
		return true;
	}
	return false;
}

void RouteFinder::setLineDirection(cv::Vec4i& line) {
	cv::Point p1(line[0], line[1]);
	cv::Point p2(line[2], line[3]);
	cv::Point tmp;
	if (p1.y < p2.y) {
		tmp = p2;
		p2 = p1;
		p1 = tmp;
	}
}

void RouteFinder::adjustLineLength(cv::Vec4i& line) {
	cv::Point pt1(line[0], line[1]);
	cv::Point pt2(line[2], line[3]);
	short distX, distY, newDistX, newDistY;
	if (pt2.y == m_Rows ) {
		return;
	}
	distX = pt1.x - pt2.x;
	distY = pt1.y - pt2.y;
	if (distY == 0 ) {
		return;
	}
	newDistY = m_Rows - pt2.y;
	newDistX = (newDistY * distX) / distY;
	line[0] += newDistX;
	line[1] += newDistY;
}

void RouteFinder::edgeDetection(cv::Mat* mat, cv::Mat* changesMat) {

    // accept only char type matrices
    CV_Assert(mat->depth() == CV_8U);

    short xDiff, yDiff;
    ushort i,j;

    for(i = (m_Rows >> 1); i < m_Rows-1; ++i) {
        for (j = 1; j < m_Cols-2; ++j) {
        	xDiff = (static_cast<ushort>(mat->at<uchar>(i,j+1)) - static_cast<ushort>(mat->at<uchar>(i,j-1))) >> 1;
        	yDiff = (static_cast<ushort>(mat->at<uchar>(i-1,j)) - static_cast<ushort>(mat->at<uchar>(i+1,j))) >> 1;
        	Gradient grad(xDiff,yDiff);
        	if (abs(grad.getLength()) > MINLENGTH) {
        		changesMat->at<uchar>(i,j) = 255;
        	}
        }
    }
    lineDetection(changesMat);

}

void RouteFinder::lineDetection(cv::Mat* changesMat) {
    short lNegSignCnt, lPosSignCnt, rNegSignCnt, rPosSignCnt;
    lNegSignCnt = lPosSignCnt = rNegSignCnt = rPosSignCnt = 0;

    std::vector<cv::Vec4i> lines;
    std::vector<cv::Vec4i> savedLinesLeft, savedLinesRight;
    cv::HoughLinesP(*changesMat, lines, 1, CV_PI/180, 15, 50, 20 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
		cv::Vec4i l = lines[i];
		setLineDirection(l);
		adjustLineLength(l);
		if ((l[2] - l[0]) != 0) {
			int slope = (l[3] - l[1]) / (l[2] - l[0]);
			char str[20];
			bzero(str, sizeof(str));
			sprintf(str,"slope of Line. %d", slope);
			m_Controller->printString(str,me,9);
			if (abs(slope) >= 1) {
				//leftSide
				if (l[2] < ((m_Cols >> 1) - MAX_PIX_DIFF) && l[0] < ((m_Cols >> 1) - MAX_PIX_DIFF)) {
					if (slope < 0) {
						lNegSignCnt++;
					}
					else {
						lPosSignCnt++;
					}
					savedLinesLeft.push_back(l);
				}
				//right side
				if (l[2] > ((m_Cols >> 1) + MAX_PIX_DIFF) && l[0] > ((m_Cols >> 1) + MAX_PIX_DIFF)) {
					if (slope < 0) {
						rNegSignCnt++;
					}
					else {
						rPosSignCnt++;
					}
					savedLinesRight.push_back(l);
				}
			}
		}
    }
    m_leftSidePositiveSlope = true;
    if (lPosSignCnt < lNegSignCnt) {
    	m_leftSidePositiveSlope = false;
    }
    lineFilter(changesMat, savedLinesLeft, savedLinesRight);

}

void RouteFinder::lineFilter(cv::Mat* changesMat, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines) {
    std::vector<cv::Vec4i> usedLinesLeft, usedLinesRight;
    short lmax, rmin;
    rmin = m_Cols + 100;
	lmax = -100;
    // left side
    for( size_t i = 0; i < leftLines.size(); i++ )
    {
		cv::Vec4i l = leftLines[i];
		int slope = (l[3] - l[1]) / (l[2] - l[0]);
		if ( (slope > 0 && m_leftSidePositiveSlope) || (slope < 0 && !m_leftSidePositiveSlope)) {
			if (l[0] >= lmax) {
				lmax = l[0];
				//line(*changesMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255), 3, CV_AA);
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
			if (l[0] <= rmin) {
				rmin = l[0];
				//line(*changesMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255), 3, CV_AA);
				usedLinesRight.push_back(l);
			}
		}
    }

    routeLocker(changesMat,usedLinesLeft, usedLinesRight);
}

void RouteFinder::routeLocker(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines) {

    // Fahrtrichtungsvektor
    cv::line(*edgeImg,cv::Point((edgeImg->cols >> 1),edgeImg->rows),
    		cv::Point((edgeImg->cols >> 1),(edgeImg->rows >> 1)),cv::Scalar(255), 2);

	if (leftLines.size() == 0 && rightLines.size() == 0) {
		m_outStr.clear();
		m_outStr = "*** No lines found, keeping old direction ***";
		m_Controller->printString(m_outStr, me, 7);
		return;
	}
	if (leftLines.size() == 0) {
		m_outStr.clear();
		m_outStr = "*** Right side found only ***";
		m_Controller->printString(m_outStr, me, 7);
	}
	else if (rightLines.size() == 0) {
		m_outStr.clear();
		m_outStr = "*** Left side found only ***";
		m_Controller->printString(m_outStr, me, 7);
	}
	else {
		m_outStr.clear();
		m_Controller->printString(m_outStr, me, 7);
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
	int corrAng;

	if (leftLines.size() > 0) {
		lVal = calcLeftRefDistance(lpt);
		line(*edgeImg, lpt, lpt2, cv::Scalar(255), 3, CV_AA);
	}
	if (rightLines.size() > 0) {
		rVal = calcRightRefDistance(rpt);
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
	corrAng = calcCorrAng(med);
	char numstr[128];

	sprintf(numstr, "Left side dist: %d ", lVal);
	putText(*edgeImg, numstr, cv::Point(5, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255));
	m_Controller->printString(numstr, me, 2);

	sprintf(numstr, "Right side dist: %d ", rVal);
	putText(*edgeImg, numstr, cv::Point(5, 35), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255));
	m_Controller->printString(numstr, me, 3);

	sprintf(numstr, "Input angle to PID: %d ", corrAng);
	putText(*edgeImg, numstr, cv::Point(5, 50), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255));
	m_Controller->printString(numstr, me, 4);
	m_pidCalc->pidDoWork(corrAng);
	m_RouteFound = true;

}

short RouteFinder::calcLeftRefDistance(cv::Point pt) {
	short dist = (pt.y >> 1) + 40;
	short diff = (m_Cols >> 1)-pt.x - dist;
	return diff;
}

short RouteFinder::calcRightRefDistance(cv::Point pt) {
	short dist = (pt.y >> 1) + 40;
	short diff = pt.x-(m_Cols >> 1) - dist;
	return diff;
}

int RouteFinder::calcCorrAng(short distVal) {
	if (abs(distVal) <= 2) {
		return 0;
	}
	return static_cast<int>(acosf(static_cast<float>(distVal)/ 160.0f) * 180 / 3.1415926f) - 90;
}
