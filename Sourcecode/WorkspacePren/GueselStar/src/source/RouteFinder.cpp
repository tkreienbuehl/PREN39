#include "../header/RouteFinder.hpp"

RouteFinder::RouteFinder(PrenController* controller, PictureCreator* picCreator)
{
	m_Controller = controller;
	//
	MINLENGTH = m_Controller->getPrenConfig()->MINLENGTH;
	MAX_NR_OF_IMAGES = m_Controller->getPrenConfig()->MAX_NR_OF_IMAGES;
	MAX_ENGINE_SPEED = m_Controller->getPrenConfig()->MAX_SPEED;
	MINXDIFF = m_Controller->getPrenConfig()->MINXDIFF;
	MINYDIFF = m_Controller->getPrenConfig()->MINYDIFF;
	MAX_PIX_DIFF = m_Controller->getPrenConfig()->MAX_PIX_DIFF;
	LINE_LOST_LIMIT = m_Controller->getPrenConfig()->LINE_LOST_LIMIT;
	NR_OF_IMS_FOR_CHECK_BEND = m_Controller->getPrenConfig()->NR_OF_IMS_FOR_CHECK_BEND;
	//
	m_PicCreator = picCreator;
	m_waitImgCnt = 0;
	m_State = false;
	pthread_mutex_init(&m_mutex, NULL);
	m_Cols = m_Rows = 0;
	m_pidCalc = new PIDCalculation(m_Controller);
	m_RouteCalculator = new RouteCalculation(m_Controller);
	m_CrossingReported = false;
	m_Driving = false;
	m_CrossingCnt = m_CrossingMatchCnt = 0;
	me = m_Controller->ROUTE_FINDER;
	m_Running = false;
}

RouteFinder::~RouteFinder() {
	delete m_pidCalc;
	delete m_RouteCalculator;
}

void* RouteFinder::staticEntryPoint(void* threadId) {
	reinterpret_cast<RouteFinder*>(threadId)->runProcess();
	cout << "Thread RouteFinder ended" << endl;
	return(threadId);
}

cv::Mat RouteFinder::getOriginalImage() {
	return m_PicCreator->GetImage();
}

cv::Mat RouteFinder::getGrayImage() {
	pthread_mutex_lock(&m_mutex);
	cv::Mat retImg = m_GrayImg.clone();
	pthread_mutex_unlock(&m_mutex);
	return retImg;
}

cv::Mat RouteFinder::getFilteredImage() {
	pthread_mutex_lock(&m_mutex);
	cv::Mat retImg = m_FinalFltImg.clone();
	pthread_mutex_unlock(&m_mutex);
	return retImg;
}

void RouteFinder::stopProcess() {
	cout << "RouteFinder stop command received" << endl;
	m_Running = false;
}

void RouteFinder::runProcess() {
	cv::Mat grayImg, image;
	ushort imgCntBendCrossing = 0;
	ushort imgCntDone = 0;
	ushort setSpeedCnt = 0;
	m_Running = true;

	m_Controller->printString("Start", me, 1);
    while(m_Running) {
        image = m_PicCreator->GetImage();
        if (!image.empty()) {
			cv::cvtColor(image,grayImg,CV_BGR2GRAY);
			cv::Mat fltImg = cv::Mat::zeros(grayImg.rows, grayImg.cols, CV_8UC1);
			if (m_Rows == 0) {
				m_Rows = grayImg.rows;
				m_Cols = grayImg.cols;
			}
			edgeDetection(&grayImg, &fltImg);
			m_GrayImg = grayImg.clone();
			m_FinalFltImg = fltImg.clone();
			outPutNrOfIms(imgCntDone);
			setDriveState(setSpeedCnt);
			checkLaneLost();
			setBendCheck(imgCntBendCrossing);
			setCrossingSearchState(imgCntBendCrossing);
			checkCrossingPriorityLane();
			checkTargetFieldFound();
			checkMaxNrOfImgsReached(imgCntDone);
        }
    }
    m_Controller->printString("Stop", me, 1);
}

void RouteFinder::edgeDetection(cv::Mat* mat, cv::Mat* changesMat) {

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
    m_RouteCalculator->lineDetection(changesMat);
    m_pidCalc->pidDoWork(m_RouteCalculator->getCamCorrVal());

}

void RouteFinder::outPutNrOfIms(uint imgCnt) {
	if (imgCnt%10 == 0) {
		char str[50];
		bzero(str,sizeof(str));
		sprintf(str,"image processed nr: %d",imgCnt);
		m_Controller->printString(str, me, 5);
	}
}

void RouteFinder::setDriveState(ushort& setSpeedCnt) {
	if (m_RouteCalculator->getRouteFoundState() && !m_Driving) {
		m_Controller->setEngineSpeed(MAX_ENGINE_SPEED);
		usleep(20 * 1000);
		if (setSpeedCnt == 3) {
			m_Driving = true;
		}
		else {
			setSpeedCnt++;
		}
	}
}

void RouteFinder::checkLaneLost() {
	if (m_RouteCalculator->getLineLostCnt() >= LINE_LOST_LIMIT) {
		m_Controller->setLaneLost();
	}
}

void RouteFinder::setBendCheck(ushort& imgCnt) {
	if (imgCnt >= NR_OF_IMS_FOR_CHECK_BEND) {
		m_RouteCalculator->startCheckForBend();
	}
	else {
		imgCnt++;
	}
}

void RouteFinder::setCrossingSearchState(ushort imgCnt) {

	if (m_waitImgCnt >= NR_OF_IMS_FOR_CHECK_BEND) {
		m_Controller->printString("Crossing search is running", me, 40);
		if (m_RouteCalculator->startCheckForCrossing()) {
		}
		else {
			if (m_CrossingCnt < m_RouteCalculator->getCrossingCnt()) {
				char str[60];
				m_CrossingCnt = m_RouteCalculator->getCrossingCnt();
				m_CrossingMatchCnt++;
				sprintf(str, "Crossing registered nr: %d" , m_CrossingMatchCnt);
				m_Controller->printString(str, me, 41);
				m_waitImgCnt = 0;
			}
		}
	}
	else {
		m_Controller->printString("Crossing search is sleeping", me, 40);
		m_waitImgCnt++;
		if (m_CrossingCnt < m_RouteCalculator->getCrossingCnt()) {
			m_CrossingCnt = m_RouteCalculator->getCrossingCnt();
		}
	}
}

void RouteFinder::checkCrossingPriorityLane() {
	if (m_CrossingMatchCnt == 2 && !m_CrossingReported) {
		m_Controller->setCrossingFound(200);
		m_CrossingReported = true;
	}
}

void RouteFinder::checkTargetFieldFound() {
	if (m_CrossingMatchCnt == 4) {
		m_Controller->setTargetFieldFound(150);
		string bye = "Now I've done my job, we are in target field ;)";
		m_Controller->printString(bye, me, 6);
		usleep(2000 * 1000);
	}
}

void RouteFinder::checkMaxNrOfImgsReached(ushort& nrOfImgs) {
	if (nrOfImgs >= MAX_NR_OF_IMAGES) {
	    m_Controller->printString("Ende :)", me ,6);
		string bye = "Now I've done my job, have fun with your pics ;)";
		m_Controller->printString(bye, me, 6);
		usleep(2000 * 1000);
		m_Controller->setState(m_Controller->END);
	}
	else {
		nrOfImgs++;
	}
}
