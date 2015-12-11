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

	std::string formatFileName(std::string fileStr, int nr);
	void outputMat(cv::Mat* mat, cv::Mat* changesMat);
	void approxLimit(cv::Mat* mat, int* upperLimit, int* lowerLimit, int row);
	void calcAverageLimit(int* upperLimit, int* lowerLimit);

	cv::Mat* getGrayImage();
	cv::Mat* getFilteredImage();

	static void* staticEntryPoint(void* threadId);

private:
	int runProcess();

	vector<int>minVals;
	vector<int>maxVals;
	const short MINLENGTH;
	const short MINYDIFF;
	const short NROFLINES;
	GradientMat* m_GradMat;
	PictureCreator* m_PicCreator;
	PrenController* m_Controller;
	bool m_State;
	cv::Mat m_GrayImg;
	cv::Mat m_FltImg;

};
