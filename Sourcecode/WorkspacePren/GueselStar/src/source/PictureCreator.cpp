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
	m_ImageLocked = false;
	pthread_mutex_init(&m_mutex, NULL);
}

PictureCreator::~PictureCreator() {

}

void* PictureCreator::staticEntryPoint(void* threadId) {
	reinterpret_cast<PictureCreator*>(threadId)->StartRecording();
	cout << "Thread PictureCreator ended" << endl;
	pthread_exit(threadId);
	return threadId;
}

cv::Mat PictureCreator::GetImage() {
	pthread_mutex_lock(&m_mutex);
		cv::Mat retImg = m_TheImage.clone();
	pthread_mutex_unlock(&m_mutex);
	return retImg;
}

void PictureCreator::StopRecording() {
	m_State = false;
}

void PictureCreator::StartRecording() {

	PrenConfiguration conf;
	if (conf.IS_DEBUG && !conf.IS_ON_PI) {
		ReadFromFiles(conf);
	}
	else {
		RecordFromCam(conf);
	}

}

void PictureCreator::RecordFromCam(PrenConfiguration conf) {
	m_State = true;

	CvCapture* capture = cvCaptureFromCAM(0);

	if(!capture) {
		cout << "No camera detected" << endl;
		m_Controller->setState(m_Controller->END);
		return;
	}

	while (m_State) {
	    IplImage* iplImg = cvQueryFrame( capture );
	    cv::Mat capturedImage =  cv::cvarrToMat(iplImg);
	    m_ImageLocked = true;
	    cv::resize(capturedImage, m_TheImage, m_TheImage.size(),0.5,0.5,cv::INTER_LANCZOS4);
	    m_ImageLocked = false;
	    usleep(5);

        if( m_TheImage.empty() ) {
        	cout << "Image is empty" << endl;
        	m_State = false;
        }
	}

	cvReleaseCapture( &capture );
}

void PictureCreator::ReadFromFiles(PrenConfiguration conf) {
	usleep(1000);
	m_State = true;
	while (m_State) {
		for (int i = conf.TEST_IMG_FIRST; i < conf.TEST_IMG_LAST; i++) {
			string path = conf.TEST_IMG_DIR;
			char file[20];
			sprintf(file,"img_%i.jpg",i);
			path.append(file);
			cv::Mat capturedImage = cv::imread(path);
			m_TheImage = capturedImage;
			path.clear();
			if( m_TheImage.empty() ) {
				cout << "Image is empty" << endl;
				m_State = false;
				break;
			}
			else {
				cv::resize(capturedImage, m_TheImage, m_TheImage.size(),0.5,0.5,cv::INTER_LANCZOS4);
				usleep(10000);
			}
		}
	}
}
