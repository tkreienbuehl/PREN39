#pragma once

#include "PrenController.hpp"
#include "PictureCreator.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;

class ObjectFinder {


public:
	ObjectFinder(PrenController* controller);
	~ObjectFinder();

	void findObjects(PictureCreator* picCreator);
	cv::Mat createImage(cv::String filename);
	cv::Mat convertImageToHSV(cv::Mat rgbImage);
	cv::Mat filterColorInImage(cv::String color, cv::Mat imageToFilter);
	vector<vector<cv::Point> > findContainersInImage(cv::Mat imageToFindContainer);
	cv::Mat markFindContoursInImage(vector<vector<cv::Point> > contours, cv::Mat imageToMarkContainer);
	vector<vector<cv::Point> > mergeContours(vector<vector<cv::Point> > contours1, vector<vector<cv::Point> > contours2);

private:
	static void* StaticEntryPoint(void* threadId);
	void RunProcess();
	cv::Mat origImage;
	cv::Mat hsvImage;
	cv::Mat filteredImageGreen;
	cv::Mat filteredImageBlue;
	vector<vector<cv::Point> > contoursGreen;
	vector<vector<cv::Point> > contoursBlue;
	vector<vector<cv::Point> > contours;
	cv::Mat markedImage;
	PictureCreator* m_PicCreator;
	PrenController* m_Controller;
};
