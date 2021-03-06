#pragma once

#include "RouteCalculation.hpp"
#include "PictureCreator.hpp"
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#include "Gradient.hpp"
#include "PIDCalculation.hpp"

using namespace std;

class RouteFinder {
public:

	RouteFinder(PrenController* controller, PictureCreator* picCreator);
	~RouteFinder();

	cv::Mat getOriginalImage();
	cv::Mat getGrayImage();
	cv::Mat getFilteredImage();

	static void* staticEntryPoint(void* threadId);

	void stopProcess();

private:

	void runProcess();
	void edgeDetection(cv::Mat* mat, cv::Mat* changesMat);
	void outPutNrOfIms(uint imgCnt);
	void setDriveState(ushort& setSpeedCnt);
	void checkLaneLost();
	void setBendCheck(ushort& imgCnt);
	void setCrossingSearchState(ushort imgCnt);
	void checkCrossingPriorityLane();
	void checkTargetFieldFound();
	void checkMaxNrOfImgsReached(ushort& nrOfImgs);

	ushort MINLENGTH;
	ushort MINXDIFF;
	ushort MINYDIFF;
	ushort MAX_PIX_DIFF;
	ushort MAX_ENGINE_SPEED;
	ushort MAX_NR_OF_IMAGES;
	ushort LINE_LOST_LIMIT;
	ushort NR_OF_IMS_FOR_CHECK_BEND;
	PictureCreator* m_PicCreator;
	PrenController* m_Controller;
	bool m_State;
	bool m_Driving;
	bool m_Running;
	cv::Mat m_GrayImg;
	cv::Mat m_FinalFltImg;
	pthread_mutex_t m_mutex;
	ushort m_Rows, m_Cols;
	PIDCalculation* m_pidCalc;
	PrenController::classes me;
	RouteCalculation* m_RouteCalculator;
	ushort m_waitImgCnt;
	ushort m_CrossingCnt;
	ushort m_CrossingMatchCnt;
	bool m_CrossingReported;

};
