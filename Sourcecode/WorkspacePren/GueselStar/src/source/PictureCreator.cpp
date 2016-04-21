/*
 * PictureCreator.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: tobias
 */

#include "../header/PictureCreator.hpp"

PictureCreator::PictureCreator(PrenController* controller) {
	m_Controller = controller;
	m_State = false;
	pthread_mutex_init(&m_mutex, NULL);
}

PictureCreator::~PictureCreator() {

}

void* PictureCreator::staticEntryPoint(void* threadId) {
	((PictureCreator*)threadId)->StartRecording();
	cout << "Stop recording pictures" << endl;
	pthread_exit(threadId);
}

cv::Mat* PictureCreator::GetImage() {
	return &m_TheImage;
}

void PictureCreator::StopRecording() {
	m_State = false;
}

void PictureCreator::StartRecording() {

	usleep(1000);
	cout << "Start recording pictures" << endl;
	m_State = true;

	CvCapture* capture = cvCaptureFromCAM(0);

	if(!capture) {
		cout << "No camera detected" << endl;
		return;
	}

	cout << "running :)" << endl;
	while (m_State) {
	    IplImage* iplImg = cvQueryFrame( capture );
		pthread_mutex_lock(&m_mutex);
	    m_TheImage = cv::cvarrToMat(iplImg);
		pthread_mutex_unlock(&m_mutex);
	    usleep(5);

        if( m_TheImage.empty() ) {
        	cout << "Image is empty" << endl;
        	m_State = false;
        }
	}

	cvReleaseCapture( &capture );

}
