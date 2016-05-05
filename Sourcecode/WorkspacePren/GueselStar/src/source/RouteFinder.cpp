#include "../header/RouteFinder.hpp"

RouteFinder::RouteFinder(PrenController* controller, PictureCreator* picCreator) {
	PrenConfiguration conf;
	MINLENGTH = conf.MINLENGTH;
	MINXDIFF = conf.MINXDIFF;
	MINYDIFF = conf.MINYDIFF;
	NROFLINES = conf.NROFLINES;
	MAX_PIX_DIFF = conf.MAX_PIX_DIFF;
	MIN_RT_WIDTH = conf.MIN_RT_WIDTH;
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_GradMat = NULL;
	m_State = false;
	m_leftRoutePos = 0;
	m_rightRoutePos = 320;
	m_rtWidth = 0;
	pthread_mutex_init(&m_mutex, NULL);
	cout << MINLENGTH << " " << NROFLINES << endl;
	m_rightSidePositiveSlope = true;
	m_leftSidePositiveSlope = false;
}

RouteFinder::~RouteFinder() {
	delete m_GradMat;
}

void RouteFinder::edgeDetection(cv::Mat* mat, cv::Mat* changesMat) {

    // accept only char type matrices
    CV_Assert(mat->depth() == CV_8U);

    short xDiff, yDiff;
    ushort upperLimit, lowerLimit, i,j;
    ushort nRows = mat->rows;
    ushort nCols = mat->cols * mat->channels();
    bool leftSidePositive, rightSidePositive;
    short lNegSignCnt, lPosSignCnt, rNegSignCnt, rPosSignCnt;
    lNegSignCnt = lPosSignCnt = rNegSignCnt = rPosSignCnt = 0;
    leftSidePositive = m_leftSidePositiveSlope;
	rightSidePositive = m_rightSidePositiveSlope;

    for(i = (nRows >> 1)-10; i < nRows-1; ++i) {
        for (j = 1; j < nCols-2; ++j) {
        	xDiff = (static_cast<ushort>(mat->at<uchar>(i,j+1)) - static_cast<ushort>(mat->at<uchar>(i,j-1))) >> 1;
        	yDiff = (static_cast<ushort>(mat->at<uchar>(i-1,j)) - static_cast<ushort>(mat->at<uchar>(i+1,j))) >> 1;
        	Gradient grad(xDiff,yDiff);
        	if (abs(grad.getLength()) > MINLENGTH) {
        		changesMat->at<uchar>(i,j) = 255;
        	}
        }
    }

    std::vector<cv::Vec4i> lines;
    std::vector<cv::Vec4i> savedlines;
    cv::HoughLinesP(*changesMat, lines, 1, CV_PI/180, 15, 30, 20 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
		cv::Vec4i l = lines[i];
		setLineDirection(l);
		if ((l[2] - l[0]) != 0) {
			int slope = (l[3] - l[1]) / (l[2] - l[0]);
			if (abs(slope) == 1 || abs(slope) == 2) {
				//leftSide
				if (l[2] < ((nCols >> 1) - MAX_PIX_DIFF) && l[0] < ((nCols >> 1) - MAX_PIX_DIFF)) {
					if (slope < 0) {
						lNegSignCnt++;
					}
					else {
						lPosSignCnt++;
					}
					savedlines.push_back(l);
				}
				//right side
				if (l[2] > ((nCols >> 1) + MAX_PIX_DIFF) && l[0] > ((nCols >> 1) + MAX_PIX_DIFF)) {
					if (slope < 0) {
						rNegSignCnt++;
					}
					else {
						rPosSignCnt++;
					}
					savedlines.push_back(l);
				}
			}
		}
    }
    if (lPosSignCnt < lNegSignCnt) {
    	leftSidePositive = false;
    }
    if (rPosSignCnt < rNegSignCnt) {
    	rightSidePositive = false;
    }

	if (m_rightSidePositiveSlope != rightSidePositive) {
		rightSidePositive = m_rightSidePositiveSlope;
	}
	if (m_leftSidePositiveSlope != leftSidePositive) {
		m_leftSidePositiveSlope = leftSidePositive;
	}

    for( size_t i = 0; i < savedlines.size(); i++ )
    {
		cv::Vec4i l = savedlines[i];
		int slope = (l[3] - l[1]) / (l[2] - l[0]);
		if (l[2] < ((nCols >> 1) - MAX_PIX_DIFF) && l[0] < ((nCols >> 1) - MAX_PIX_DIFF)) {
			if ( (slope > 0 && leftSidePositive) || (slope < 0 && !leftSidePositive)) {
				line(*changesMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255), 3, CV_AA);
			}
		}
		//right side
		if (l[2] > ((nCols >> 1) + MAX_PIX_DIFF) && l[0] > ((nCols >> 1) + MAX_PIX_DIFF)) {
			if ( (slope > 0 && rightSidePositive) || (slope < 0 && !rightSidePositive)) {
				line(*changesMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255), 3, CV_AA);
			}
		}
    }
    usleep(10000);

    i = nRows-1;
    upperLimit = mat->cols;
    lowerLimit = 0;
    for( i = nRows-10; i > (nRows >> 1); i--) {
    	if ((i % NROFLINES) == 0 && m_minVals.size() > 0) {
    		calcAverageLimit(upperLimit, lowerLimit);
    		//cv::circle(*changesMat, cv::Point(lowerLimit,i), 2, cv::Scalar(255),2);
    		//cv::circle(*changesMat, cv::Point(upperLimit,i), 2, cv::Scalar(255,0,0),2);
    	}
    	else {
    		approxLimit(changesMat, upperLimit, lowerLimit, i);
    	}
    }
    routeLocker(changesMat,upperLimit, lowerLimit, i);

}

void RouteFinder::routeLocker(cv::Mat* edgeImg,
		unsigned short upperLimit,
		unsigned short lowerLimit,
		unsigned short row) {

	if ((m_leftRoutePos == 0 && m_rightRoutePos == 320) ||
			(compareTolerance(m_leftRoutePos,lowerLimit) && compareTolerance(m_rightRoutePos,upperLimit))) {
		m_leftRoutePos = lowerLimit;
		m_rightRoutePos = upperLimit;
	}
	m_rtWidth = m_rightRoutePos-m_leftRoutePos;
	if (m_rtWidth < MIN_RT_WIDTH) {
		//m_rightRoutePos = edgeImg->cols;
		//m_leftRoutePos = 0;
	}
    // Fahrtrichtungsvektor
    cv::line(*edgeImg,cv::Point((edgeImg->cols >> 1),edgeImg->rows),
    		cv::Point((edgeImg->cols >> 1),(edgeImg->rows >> 1)),cv::Scalar(255), 2);
}

void RouteFinder::calcDriveDirection(cv::Mat* edgeImg) {
	//int middle;
	unsigned short rows = edgeImg->rows;
	for (short i = rows; i > (rows >> 1) ; i-=5 ) {
		for (short j = m_leftRoutePos-MAX_PIX_DIFF ; j< m_leftRoutePos+MAX_PIX_DIFF; j++) {
			if (edgeImg->at<uchar>(i,j) == 255) {
				if (compareTolerance(m_leftRoutePos, j)) {
					//cv::circle(*edgeImg, cv::Point(j,i), 3, cv::Scalar(255,0,0),2);
				}
			}
		}
		for (short j = m_rightRoutePos-MAX_PIX_DIFF ; j< m_rightRoutePos+MAX_PIX_DIFF; j++) {
			if (edgeImg->at<uchar>(i,j) == 255) {
				if (compareTolerance(m_rightRoutePos, j)) {
					//cv::circle(*edgeImg, cv::Point(i,j), 3, cv::Scalar(255,0,0),2);
				}
			}
		}
		//middle = static_cast<unsigned short>((m_leftRoutePos+m_rightRoutePos)/2);
	}

}

void RouteFinder::approxLimit(cv::Mat* mat, unsigned short& upperLimit, unsigned short& lowerLimit, unsigned short row) {

	ushort val;
	ushort cnt;
	bool minim = false;
	for ( int j = (mat->cols >> 1)+MAX_PIX_DIFF; j <  mat->cols-1; j++) {
		val = static_cast<ushort>(mat->at<uchar>(row,j));
		if (val == 255) {
			if (minim == false) {
				upperLimit = j;
				minim = true;
			}
			else {
				break;
			}
		}
	}
	minim = false;

	for (int j = (mat->cols >> 1)-(MAX_PIX_DIFF << 1); j > 0; j--) {
		val = static_cast<ushort>(mat->at<uchar>(row,j));
		if (val == 255) {
			if (minim == false) {
				if (cnt == 1) {
					lowerLimit = j;
					minim = true;
				}
				else {
					cnt++;
				}
			}
			else {
				break;
			}
		}
		else {
			cnt = 0;
		}
	}
	//cout << "Row: " << row << " Low: " << lowerLimit << " Up: " << upperLimit << endl;
	//usleep(100000);
	m_minVals.push_back(lowerLimit);
	m_maxVals.push_back(upperLimit);
}

void RouteFinder::calcAverageLimit(unsigned short& upperLimit, unsigned short& lowerLimit) {

	bubbleSort(&m_minVals);
	bubbleSort(&m_maxVals);

	ushort medVal = static_cast<ushort>(m_minVals.size()/2);

	lowerLimit = m_minVals.at(medVal);
	upperLimit = m_maxVals.at(medVal);

	m_minVals.clear();
	m_maxVals.clear();
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

void* RouteFinder::staticEntryPoint(void* threadId) {
	reinterpret_cast<RouteFinder*>(threadId)->runProcess();
	cout << "Thread ended " << endl;
	return NULL;
}

int RouteFinder::runProcess() {
	cv::Mat grayImg, image;

	cout << "Start" << endl;
    for(int i = 0; i<2500000; i++) {

        image = m_PicCreator->GetImage();

        if (!image.empty()) {

			cv::cvtColor(image,grayImg,CV_BGR2GRAY);
			cv::Mat fltImg = cv::Mat::zeros(grayImg.rows, grayImg.cols, CV_8UC1);
			edgeDetection(&grayImg, &fltImg);
			calcDriveDirection(&fltImg);
			m_GrayImg = grayImg;
			m_FinalFltImg = fltImg;
			if (i%100 == 0) {
				cout << "image processed nr:" << i << endl;
			}
        }
        else {
        	i--;
        }
    }

    m_Controller->setState(m_Controller->END);
	string bye = "Now I've done my job, have fun with your pics ;)";
	cout << bye << endl;

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
