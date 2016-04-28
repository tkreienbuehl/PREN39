#include "../header/RouteFinder.hpp"

RouteFinder::RouteFinder(PrenController* controller, PictureCreator* picCreator)
: MINLENGTH(10), MINXDIFF(4), MINYDIFF(4), NROFLINES(20), MAX_PIX_DIFF(7), MIN_RT_WIDTH(50) {
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_GradMat = NULL;
	m_State = false;
	m_leftRoutePos = 0;
	m_rightRoutePos = 0;
	m_rtWidth = 0;
	pthread_mutex_init(&m_mutex, NULL);
}

RouteFinder::~RouteFinder() {
	delete m_GradMat;
}

void RouteFinder::edgeDetection(cv::Mat* mat, cv::Mat* changesMat) {

    // accept only char type matrices
    CV_Assert(mat->depth() == CV_8U);

    short xDiff, yDiff;
    unsigned short upperLimit, lowerLimit, i,j;
    unsigned short nRows = mat->rows;
    unsigned short nCols = mat->cols* mat->channels();

    for(i = static_cast<short>(nRows/2)-10; i < nRows-1; ++i) {
        for (j = 1; j < nCols-2; ++j) {
        	xDiff = (static_cast<short>(mat->at<uchar>(i,j+1)) - static_cast<short>(mat->at<uchar>(i,j-1))) / 2;
        	yDiff = (static_cast<short>(mat->at<uchar>(i-1,j)) - static_cast<short>(mat->at<uchar>(i+1,j))) / 2;
        	Gradient grad(xDiff,yDiff);
        	if (abs(grad.getLength()) > MINLENGTH) {
        		changesMat->at<uchar>(i,j) = 255;
        	}
        }
    }

    i = nRows-1;
    upperLimit = 0;
    lowerLimit = 0;
    for( i = nRows-3; i > 0; i--) {
    	if (i > nRows - NROFLINES) {
    		approxLimit(changesMat, upperLimit, lowerLimit, i);
    	}
    	else {
    		if (i == nRows - NROFLINES) {
    			calcAverageLimit(upperLimit, lowerLimit);
    			//cout << upperLimit << " " << lowerLimit << endl;
    		}
    	}
    }
    routeLocker(changesMat,upperLimit, lowerLimit, i);

}

void RouteFinder::routeLocker(cv::Mat* edgeImg,
		unsigned short upperLimit,
		unsigned short lowerLimit,
		unsigned short row) {

	if ((m_leftRoutePos == 0 && m_rightRoutePos == 0) ||
			(compareTolerance(m_leftRoutePos,lowerLimit) && compareTolerance(m_rightRoutePos,upperLimit))) {
		m_leftRoutePos = lowerLimit;
		m_rightRoutePos = upperLimit;
	}
	m_rtWidth = m_rightRoutePos-m_leftRoutePos;
	if (m_rtWidth < MIN_RT_WIDTH) {
		m_rightRoutePos = 0;
		m_leftRoutePos = 0;
	}
    // Fahrtrichtungsvektor
    cv::line(*edgeImg,cv::Point(edgeImg->cols/2,edgeImg->rows),
    		cv::Point(edgeImg->cols/2,edgeImg->rows/2),cv::Scalar(255,0,0), 2);
	cv::circle(*edgeImg, cv::Point(m_leftRoutePos,edgeImg->rows-5), 3, cv::Scalar(255,0,0),2);
	cv::circle(*edgeImg, cv::Point(m_rightRoutePos,edgeImg->rows-5), 3, cv::Scalar(255,0,0),2);
}

void RouteFinder::calcDriveDirection(cv::Mat* edgeImg) {
	//int middle;
	unsigned short rows = edgeImg->rows;
	for (short i = rows; i > static_cast<short>(rows-rows/4) ; i-=5 ) {
		for (short j = m_leftRoutePos-(2*MAX_PIX_DIFF) ; j< m_leftRoutePos+(2*MAX_PIX_DIFF); j++) {
			if (edgeImg->at<uchar>(i,j) == 255) {
				if (compareTolerance(m_leftRoutePos, j)) {
					//cv::circle(*edgeImg, cv::Point(j,i), 3, cv::Scalar(255,0,0),2);
				}
			}
		}
		for (short j = m_rightRoutePos-(2*MAX_PIX_DIFF) ; j< m_rightRoutePos+(2*MAX_PIX_DIFF); j++) {
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

	unsigned short val;
	bool minim = false;
	for ( int j = mat->cols/2; j <  mat->cols-1; j++) {
		val = static_cast<unsigned short>(mat->at<uchar>(row,j));
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
	for (int j = mat->cols/2; j > 0; j--) {
		val = static_cast<unsigned short>(mat->at<uchar>(row,j));
		if (val == 255) {
			if (minim == false) {
				lowerLimit = j;
				minim = true;
			}
			else {
				break;
			}
		}
	}
	m_minVals.push_back(lowerLimit);
	m_maxVals.push_back(upperLimit);
}

void RouteFinder::calcAverageLimit(unsigned short& upperLimit, unsigned short& lowerLimit) {

	bubbleSort(&m_minVals);
	bubbleSort(&m_maxVals);

	lowerLimit = m_minVals.at(8);
	upperLimit = m_maxVals.at(8);

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
    for(int i = 0; i<25000; i++) {

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
