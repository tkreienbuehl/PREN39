#pragma once

#include "PrenController.hpp"
#include "PictureCreator.hpp"
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "GradientMat.hpp"
#include <unistd.h>
#include "PIDCalculation.h"
#include <stdint.h>
#include <math.h>

using namespace std;

class RouteFinder {
public:
	RouteFinder(PrenController* controller, PictureCreator* picCreator);
	~RouteFinder();

	cv::Mat getOriginalImage();
	cv::Mat getGrayImage();
	cv::Mat getFilteredImage();

	static void* staticEntryPoint(void* threadId);

private:
	int runProcess();
	void bubbleSort(std::vector<unsigned short>* vals);
	bool compareTolerance(unsigned short refVal, unsigned short compVal);
	void setLineDirection(cv::Vec4i& line);
	void edgeDetection(cv::Mat* mat, cv::Mat* changesMat);
	void lineDetection(cv::Mat* changesMat);
	void lineFilter(cv::Mat* changesMat, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines);
	void routeLocker(cv::Mat* edgeImg, vector<cv::Vec4i>& leftLines, vector<cv::Vec4i>& rightLines);
	void calcDriveDirection(cv::Mat* edgeImg);
	void approxLimit(cv::Mat* mat, unsigned short& upperLimit, unsigned short& lowerLimit, unsigned short row);
	void calcAverageLimit(unsigned short& upperLimit, unsigned short& lowerLimit);
	short calcRefDistance(cv::Point pt);
	int calcCorrAng(short distVal);
	void printString(string str, uint line = 0);

	vector<unsigned short>m_minVals;
	vector<unsigned short>m_maxVals;
	ushort MINLENGTH;
	ushort MINXDIFF;
	ushort MINYDIFF;
	ushort NROFLINES;
	ushort MAX_PIX_DIFF;
	ushort MIN_RT_WIDTH;
	GradientMat* m_GradMat;
	PictureCreator* m_PicCreator;
	PrenController* m_Controller;
	bool m_State;
	bool m_RouteFound;
	bool m_Driving;
	cv::Mat m_GrayImg;
	cv::Mat m_FinalFltImg;
	unsigned short m_leftRoutePos, m_rightRoutePos, m_rtWidth;
	pthread_mutex_t m_mutex;
	bool m_rightSidePositiveSlope;
	bool m_leftSidePositiveSlope;
	unsigned short m_Rows, m_Cols;
	PIDCalculation* m_pidCalc;
	string m_outStr;


};
