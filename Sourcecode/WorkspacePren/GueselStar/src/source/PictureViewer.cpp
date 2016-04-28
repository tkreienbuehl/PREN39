#include "../header/PictureViewer.hpp"

	PictureViewer::PictureViewer(RouteFinder* rtFinder, ObjectFinder* objFinder) {
		m_rtFinder = rtFinder;
		m_objectFinder = objFinder;
		m_state = false;
	}

	PictureViewer::~PictureViewer() {

	}

	void* PictureViewer::startThread(void* threadId) {
		((PictureViewer*) threadId)->runProcess();
		return threadId;
	}

	void PictureViewer::stopViewer() {
		m_state = false;
	}

	void PictureViewer::runProcess() {

		cv::Mat fltImage, objectImg, origImg;
		PrenConfiguration conf;

		usleep(1000000);
		m_state = true;
		uint32_t imgNr = 0;

		cout << "Viever online" << endl;
		if (!conf.IS_ON_PI) {
			cv::namedWindow( "The Image", CV_WINDOW_AUTOSIZE );
			cout << "window is opening" << endl;
		}
		while (m_state) {

			try {
				fltImage = m_rtFinder->getFilteredImage();
			}
			catch (...) {
				cout << "Error occured" << endl;
			}


			if (!fltImage.empty()) {
				if (conf.IS_ON_PI) {
					char str[20];
					bzero(str,20);
					sprintf(str,"/tmp/img_%i.jpg",imgNr);
					cv::imwrite(str, m_rtFinder->getOriginalImage());
					//sprintf(str,"/tmp/fltImg_%i.jpg",imgNr);
					//cv::imwrite(str, fltImage);

					/*
					bzero(str,20);
					sprintf(str,"/tmp/imggrey_%i.jpg",imgNr);
					cv::imwrite(str, m_rtFinder->getGrayImage());

					bzero(str,20);
					sprintf(str,"/tmp/imgobj_%i.jpg",imgNr);
					cv::imwrite(str, m_objectFinder->getImage());
					*/
					imgNr++;
				}
				else {
					try {
						cv::imshow("The Image", fltImage);
						cv::waitKey(50);
					}
					catch (...) {
						cout << "Error occured" << endl;
					}
					try {
						cv::imshow("The Source gray Image", m_rtFinder->getGrayImage());
						cv::waitKey(20);
					}
					catch (...) {
						cout << "Error occured" << endl;
					}
					try {
						objectImg =  m_objectFinder->getImage();
						cv::imshow("Object Detection", objectImg);
						cv::waitKey(150);
					}
					catch (...) {
						cout << "Error occured" << endl;
					}

				}
			}
		}
	}
