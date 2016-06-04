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
		char str[20];
		char strO[20];
		if (!conf.IS_ON_PI) {
			cv::namedWindow( "The Image", CV_WINDOW_AUTOSIZE );
		}
		while (m_state) {
			try {
				origImg = m_rtFinder->getOriginalImage();
				fltImage = m_rtFinder->getFilteredImage();
				objectImg =  m_objectFinder->getImage();

				if (!origImg.empty()) {
					if (conf.IS_ON_PI) {
						sprintf(str,"/tmp/img_%i.jpg",imgNr);
						cv::imwrite(str, origImg);
					//	sprintf(strO,"/tmp/object_img_%i.jpg",imgNr);
					//	cv::imwrite(strO, objectImg);
						imgNr++;
					} else {
						try {
							cv::imshow("The Image", fltImage);
						} catch (...) {
							cout << "Error occured" << endl;
						}
						try {
							cv::imshow("The Source gray Image", m_rtFinder->getGrayImage());
						} catch (...) {
							cout << "Error occured" << endl;
						}
						try {
							cv::imshow("Object Detection", objectImg);
						} catch (...) {
							cout << "Error occured" << endl;
						}
						cv::waitKey(0);
					}
				}
				else {
					m_state = false;
				}
			}
			catch (...) {
				cout << "Error occured" << endl;
			}
		}
		if (!conf.IS_ON_PI) {
			cv::destroyAllWindows();
			cv::waitKey(1);
		}
	}
