#include "../header/RouteFinder.hpp"

RouteFinder::RouteFinder(PrenController* controller, PictureCreator* picCreator)
: MINLENGTH(10), MINYDIFF(3), NROFLINES(20), MAX_PIX_DIFF(10) {
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_GradMat = NULL;
	m_State = false;
	m_leftRoutePos = 0;
	m_rightRoutePos = 0;
}

RouteFinder::~RouteFinder() {
	delete m_GradMat;
}

std::string RouteFinder::formatFileName(std::string fileStr, unsigned short nr) {
	ostringstream nrStream;
	nrStream << nr;
	unsigned short start = fileStr.find("000");
	cout << nrStream.str() << endl;
	fileStr.replace(start,3,nrStream.str().c_str(),nrStream.str().length());
	cout << fileStr << endl;
	return fileStr;
}

void RouteFinder::edgeDetection(cv::Mat* mat, cv::Mat* changesMat) {

    // accept only char type matrices
    CV_Assert(mat->depth() == CV_8U);

    short xDiff, yDiff;
    unsigned short upperLimit, lowerLimit, i,j;
    unsigned short nRows = mat->rows;
    unsigned short nCols = mat->cols* mat->channels();

    for(i = 1; i < nRows; ++i) {
        for (j = 0; j < nCols-1; ++j) {
        	xDiff = static_cast<short>(mat->at<uchar>(i,j+1)) - static_cast<short>(mat->at<uchar>(i,j));
        	yDiff = static_cast<short>(mat->at<uchar>(i-1,j)) - static_cast<short>(mat->at<uchar>(i,j));
        	Gradient grad(xDiff,yDiff);
        	m_GradMat->setValue(i,j,grad);
        	if (abs(m_GradMat->getValue(i,j).getLength()) > MINLENGTH
        			&& m_GradMat->getValue(i,j).getYValue() > MINYDIFF) {
        		changesMat->at<uchar>(i,j) = 255;
        	}
        }
    }

    i = nRows-1;
    upperLimit = 0;
    lowerLimit = 0;
    for( i = nRows-3; i > 0; i--) {
    	if (i > nRows - NROFLINES) {
    		approxLimit(changesMat, &upperLimit, &lowerLimit, i);
    	}
    	else {
    		if (i == nRows - NROFLINES) {
    			calcAverageLimit(&upperLimit, &lowerLimit);
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
    // Fahrtrichtungsvektor
    cv::line(*edgeImg,cv::Point(edgeImg->cols/2,edgeImg->rows),
    		cv::Point(edgeImg->cols/2,edgeImg->rows/2),cv::Scalar(255,0,0), 2);
	cv::circle(*edgeImg, cv::Point(m_leftRoutePos,edgeImg->rows-5), 3, cv::Scalar(255,0,0),2);
	cv::circle(*edgeImg, cv::Point(m_rightRoutePos,edgeImg->rows-5), 3, cv::Scalar(255,0,0),2);
}

void RouteFinder::calcDriveDirection(cv::Mat* edgeImg) {
	int middle;
	for (short i = edgeImg->rows; i > 10 ; i-=5 ) {
		for (short j = m_leftRoutePos-40 ; j< m_leftRoutePos+40; j++) {
			if (edgeImg->at<uchar>(i,j) == 255) {
				if (compareTolerance(m_leftRoutePos, j)) {
					//cv::circle(*edgeImg, cv::Point(i,j), 3, cv::Scalar(255,0,0),2);
				}
			}
		}
		for (short j = m_rightRoutePos-40 ; j< m_rightRoutePos+40; j++) {
			if (edgeImg->at<uchar>(i,j) == 255) {
				if (compareTolerance(m_rightRoutePos, j)) {
					//cv::circle(*edgeImg, cv::Point(i,j), 3, cv::Scalar(255,0,0),2);
				}
			}
		}
		middle = static_cast<unsigned short>((m_leftRoutePos+m_rightRoutePos)/2);
	}

}

void RouteFinder::approxLimit(cv::Mat* mat, unsigned short* upperLimit, unsigned short* lowerLimit, unsigned short row) {

	unsigned short val;
	bool minim = false;
	for ( int j = mat->cols/2; j <  mat->cols-1; j++) {
		val = static_cast<unsigned short>(mat->at<uchar>(row,j));
		if (val == 255) {
			if (minim == false) {
				*upperLimit = j;
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
				*lowerLimit = j;
				minim = true;
			}
			else {
				break;
			}
		}
	}
	m_minVals.push_back(*lowerLimit);
	m_maxVals.push_back(*upperLimit);
}

void RouteFinder::calcAverageLimit(unsigned short* upperLimit, unsigned short* lowerLimit) {

	bubbleSort(&m_minVals);
	bubbleSort(&m_maxVals);

	*lowerLimit = m_minVals.at(8);
	*upperLimit = m_maxVals.at(8);

	//cout << "**********************************************" << endl;
	m_minVals.clear();
	m_maxVals.clear();
	//cout << " Average min" << *lowerLimit << " Average max" << *upperLimit << endl;
}

cv::Mat RouteFinder::getGrayImage() {
	return m_GrayImg;
}

cv::Mat RouteFinder::getFilteredImage() {
	return m_FinalFltImg;
}

void* RouteFinder::staticEntryPoint(void* threadId) {
	((RouteFinder*)threadId)->runProcess();
	cout << "Thread ended " << endl;
	return NULL;
}

int RouteFinder::runProcess() {
	cv::Mat grayImg, reducedImg, image;
	ostringstream nrStream;

	cout << "Start" << endl;
    for(int i = 0; i<8000; i++) {

        image = *m_PicCreator->GetImage();

        if (!image.empty()) {

			cv::resize(image, reducedImg, reducedImg.size(),0.5,0.5,cv::INTER_LANCZOS4);
			cv::cvtColor(reducedImg,grayImg,CV_BGR2GRAY);

			cv::Mat fltImg = cv::Mat::zeros(reducedImg.rows, reducedImg.cols, CV_8UC1);
			m_GradMat = GradientMat::getInstance(static_cast<short>(reducedImg.rows), static_cast<short>(reducedImg.cols));
			edgeDetection(&grayImg, &fltImg);
			calcDriveDirection(&fltImg);
			m_GrayImg = grayImg;
			m_FinalFltImg = fltImg;
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
