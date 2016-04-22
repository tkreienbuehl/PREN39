/*
 * PictureCreator.hpp
 *
 *  Created on: Dec 4, 2015
 *      Author: tobias
 */

#pragma once

#include "PrenController.hpp"
#include <iostream>
#include <pthread.h>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <unistd.h>

using namespace std;

class PictureCreator {
public:
	PictureCreator(PrenController* controller);
	~PictureCreator();

	cv::Mat GetImage();
	void StopRecording();

	static void* staticEntryPoint(void* threadId);

private:
	void StartRecording(void);

    cv::Mat m_TheImage;
    bool m_State;
    PrenController* m_Controller;
    pthread_mutex_t m_mutex;
};
