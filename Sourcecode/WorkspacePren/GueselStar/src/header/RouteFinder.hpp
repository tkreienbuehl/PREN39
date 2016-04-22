#pragma once

#include "PrenController.hpp"
#include "PictureCreator.hpp"
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "GradientMat.hpp"
#include <unistd.h>

using namespace std;

class RouteFinder {
public:
	RouteFinder(PrenController* controller, PictureCreator* picCreator);
	~RouteFinder();

	std::string formatFileName(std::string fileStr, unsigned short nr);
	void edgeDetection(cv::Mat* mat, cv::Mat* changesMat);
	void routeLocker(cv::Mat* edgeImg, unsigned short upperLimit, unsigned short lowerLimit, unsigned short row);
	void calcDriveDirection(cv::Mat* edgeImg);
	void approxLimit(cv::Mat* mat, unsigned short& upperLimit, unsigned short& lowerLimit, unsigned short row);
	void calcAverageLimit(unsigned short& upperLimit, unsigned short& lowerLimit);

	cv::Mat getOriginalImage();
	cv::Mat getGrayImage();
	cv::Mat getFilteredImage();

	static void* staticEntryPoint(void* threadId);

private:
	int runProcess();
	void bubbleSort(std::vector<unsigned short>* vals);
	bool compareTolerance(unsigned short refVal, unsigned short compVal);

	vector<unsigned short>m_minVals;
	vector<unsigned short>m_maxVals;
	const short MINLENGTH;
	const short MINYDIFF, MINXDIFF, MIN_RT_WIDTH;
	const short NROFLINES;
	const short MAX_PIX_DIFF;
	GradientMat* m_GradMat;
	PictureCreator* m_PicCreator;
	PrenController* m_Controller;
	bool m_State;
	cv::Mat m_GrayImg;
	cv::Mat m_FinalFltImg;
	unsigned short m_leftRoutePos, m_rightRoutePos, m_rtWidth;

};
