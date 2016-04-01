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

		cv::Mat fltImage;

		usleep(2000000);
		m_state = true;

		cout << "Viever online" << endl;
		cv::namedWindow( "The Image", CV_WINDOW_AUTOSIZE );
		while (m_state) {

			fltImage = *m_rtFinder->getFilteredImage();

			if (!fltImage.empty()) {
				cv::imshow("The Image", fltImage);
				cv::imshow("The Source gray Image", *m_rtFinder->getGrayImage());
				cv::imshow("Object Detection", *m_objectFinder->getImage());
				cv::waitKey(0);
			}
		}

	}
