#pragma once

#include "PrenController.hpp"
#include "PictureCreator.hpp"
#include "PrenConfiguration.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <pthread.h>

using namespace std;

class ObjectFinder : public ObjectStateObserver {


public:
	ObjectFinder(PrenController* controller, PictureCreator* picCreator);
	virtual ~ObjectFinder();

	void findObjects(PictureCreator* picCreator);
	cv::Mat createImage(cv::String filename);
	cv::Mat convertImageToHSV(cv::Mat rgbImage);
	cv::Mat filterColorInImage(cv::String color, cv::Mat imageToFilter);
	vector<vector<cv::Point> > findContainersInImage(cv::Mat imageToFindContainer);
	cv::Mat markFoundContoursInImage(vector<vector<cv::Point> > contours, cv::Mat imageToMarkContainer);
	vector<vector<cv::Point> > mergeContours(vector<vector<cv::Point> > contours1, vector<vector<cv::Point> > contours2);
	void updateCrossingState(bool crossingAhead);
	void updateObjectOnLaneState(bool objectOnLane);
	static void* staticEntryPoint(void* threadId);

	cv::Mat getImage();
	void stopProcess();

private:
	void RunProcess();
	vector<vector<cv::Point> > contoursGreen;
	vector<vector<cv::Point> > contoursBlue;
	vector<vector<cv::Point> > contours;
	cv::Mat m_MarkedImage;
	PictureCreator* m_PicCreator;
	PrenController* m_Controller;
	bool m_state;
	pthread_mutex_t m_mutex;
	bool m_crossingAhead;
	bool m_objectOnLane;
	int lastCenterX;
	int lastCenterY;
	bool informedController;
};
