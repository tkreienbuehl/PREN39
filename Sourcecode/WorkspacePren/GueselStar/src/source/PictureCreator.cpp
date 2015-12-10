/*
 * PictureCreator.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: tobias
 */

#include "../header/PictureCreator.hpp"

PictureCreator::PictureCreator(PrenController* controller) {

	pthread_t thread;
	short i = 0;
	int rc;
	m_State = true;
	cout << "main() : creating thread, " << i << endl;
	rc = pthread_create(&thread, NULL, PictureCreator::StaticEntryPoint, this);
	if (rc) {
	 cout << "Error:unable to create thread," << rc << endl;
	 exit(-1);
	}
}

PictureCreator::~PictureCreator() {

}

void* PictureCreator::StaticEntryPoint(void* threadId) {
	((PictureCreator*)threadId)->StartRecording();
	return NULL;
}

cv::Mat* PictureCreator::GetImage() {
	return &m_TheImage;
}

void PictureCreator::StopRecording() {
	m_State = false;
}

void PictureCreator::StartRecording() {

	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);

	cout << "running :)" << endl;
	if(!capture) {
		cout << "No camera detected" << endl;
		return;
	}

	while (m_State) {
	    IplImage* iplImg = cvQueryFrame( capture );
	    m_TheImage = cv::cvarrToMat(iplImg);
	    usleep(10);

        if( m_TheImage.empty() ) {
        	cout << "Image is empty" << endl;
        	m_State = false;
        }
	}

	cvReleaseCapture( &capture );
}
