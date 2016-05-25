#include "../header/ObjectFinder.hpp"

#include <sstream>
#include <string>

using namespace std;

ObjectFinder::ObjectFinder(PrenController* controller,
		PictureCreator* picCreator) {
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_state = false;
	pthread_mutex_init(&m_mutex, NULL);
	informedController = false;
	m_crossingAhead = false;
	lastCenterX = 0;
	lastCenterY = 0;
}

ObjectFinder::~ObjectFinder() {

}

void* ObjectFinder::staticEntryPoint(void* threadId) {
	((ObjectFinder*) threadId)->RunProcess();
	return NULL;
}

void ObjectFinder::RunProcess() {

	cv::Mat origImage;
	cv::Mat croppedImage;
	cv::Mat hsvImage;
	cv::Mat filteredImageGreen;
	cv::Mat filteredImageBlue;
	cv::Mat resultImage;

	m_state = true;

	while (m_state) {

		if (m_Controller->checkObjectOnLane()) { //ultraschall: no object detected

			cv::Mat image = m_PicCreator->GetImage();

			origImage = image.clone();
			if (!origImage.empty()) {
				cv::Rect areaToCrop(origImage.cols / 2, 0, origImage.cols / 2,
						origImage.rows);

				croppedImage = origImage(areaToCrop);

				cv::medianBlur(croppedImage, croppedImage, 3);
				hsvImage = convertImageToHSV(croppedImage);
				filteredImageGreen = filterColorInImage("green", hsvImage);
				filteredImageBlue = filterColorInImage("blue", hsvImage);

				cv::Mat blue_green_combined;
				cv::addWeighted(filteredImageGreen, 1.0, filteredImageBlue, 1.0,
						0.0, blue_green_combined);

				contoursGreen = findContainersInImage(filteredImageGreen);
				contoursBlue = findContainersInImage(filteredImageBlue);
				contours = mergeContours(contoursGreen, contoursBlue);

				// TODO: IDEA for two following containers with same color
				//cv::Scalar color( 255, 255, 255 );
				//cv::drawContours(croppedImage, contours, -1, color, 1, 8);
				// cv::Mat ourfitLine;
				// cv::fitLine(InputArray points, OutputArray line, int distType, double param, double reps, double aeps);

				resultImage = markFoundContoursInImage(contours, croppedImage);
				line(resultImage, cv::Point(4 * croppedImage.cols / 5, 0),
						cv::Point(4 * croppedImage.cols / 5, croppedImage.rows),
						cv::Scalar(0, 255, 255), 1, 8, 0);
				m_MarkedImage = resultImage;

				usleep(10);
			}
		}
	}
}

cv::Mat ObjectFinder::createImage(cv::String filename) {
	cv::Mat srcImage = imread(filename, 1);

	if (srcImage.data == NULL) {
		printf("file cannot be loaded\n");
	}

	cv::Mat origImage;
	cv::Size size(900, 675);
	resize(srcImage, origImage, size);

	return origImage;
}

cv::Mat ObjectFinder::convertImageToHSV(cv::Mat rgbImage) {
	cv::Mat HSVImage;

	cvtColor(rgbImage, HSVImage, CV_BGR2HSV);
	return HSVImage;
}

cv::Mat ObjectFinder::filterColorInImage(cv::String color,
		cv::Mat imageToFilter) {

	cv::Mat filteredImage;
	if (color == "green") {
		cv::inRange(imageToFilter,
				cv::Scalar(m_Controller->getPrenConfig()->GREEN_RANGE_H_LOW,
						m_Controller->getPrenConfig()->GREEN_RANGE_S_LOW,
						m_Controller->getPrenConfig()->GREEN_RANGE_V_LOW),
				cv::Scalar(m_Controller->getPrenConfig()->GREEN_RANGE_H_HIGH,
						m_Controller->getPrenConfig()->GREEN_RANGE_S_HIGH,
						m_Controller->getPrenConfig()->GREEN_RANGE_V_HIGH),
				filteredImage);
	} else if (color == "blue") {
		cv::inRange(imageToFilter,
				cv::Scalar(m_Controller->getPrenConfig()->BLUE_RANGE_H_LOW,
						m_Controller->getPrenConfig()->BLUE_RANGE_S_LOW,
						m_Controller->getPrenConfig()->BLUE_RANGE_V_LOW),
				cv::Scalar(m_Controller->getPrenConfig()->BLUE_RANGE_H_HIGH,
						m_Controller->getPrenConfig()->BLUE_RANGE_S_HIGH,
						m_Controller->getPrenConfig()->BLUE_RANGE_V_HIGH),
				filteredImage);
	}
	return filteredImage;
}

vector<vector<cv::Point> > ObjectFinder::findContainersInImage(
		cv::Mat imageToFindContainer) {
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;
	cv::findContours(imageToFindContainer, contours, hierarchy, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	return contours;
}

vector<vector<cv::Point> > ObjectFinder::mergeContours(
		vector<vector<cv::Point> > contours1,
		vector<vector<cv::Point> > contours2) {
	vector<vector<cv::Point> > contours;

	contours.reserve(contours1.size() + contours2.size());
	contours.insert(contours.end(), contours1.begin(), contours1.end());
	contours.insert(contours.end(), contours2.begin(), contours2.end());

	return contours;
}

cv::Mat ObjectFinder::markFoundContoursInImage(
		vector<vector<cv::Point> > contours, cv::Mat imageToMarkContainer) {

	vector<vector<cv::Point> > contours_poly(contours.size());
	vector<cv::Rect> boundRect(contours.size());
	cv::Mat markedImage = imageToMarkContainer;

	for (unsigned int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));

		// check if it is a container
		if (boundRect[i].height > boundRect[i].width && boundRect[i].width > 50
				&& boundRect[i].height > 40) {
			//m_Controller->printString("Container found: waiting for distance calculation...", m_Controller->OBJECT_FINDER, 0);
			// mark the container
			rectangle(markedImage, boundRect[i].tl(), boundRect[i].br(),
					cv::Scalar(0, 255, 0), 1, 8, 0);

			// if container is close enough
			if (boundRect[i].height > 65) {
				// calculate center-Position of container
				int centerX = boundRect[i].x + boundRect[i].width / 2;
				int centerY = boundRect[i].y + boundRect[i].height / 2;

				// if it's a new Container
				if (abs(lastCenterX - centerX) > 10) {
					informedController = false;
				}
				lastCenterX = centerX;
				lastCenterY = centerY;

				// if container needs to be announcecd
				if (centerX >= 4 * imageToMarkContainer.cols / 5
						&& !informedController) {

					int distanceToContainer =
							m_Controller->getPrenConfig()->REFERENCE_DISTANCE
									* m_Controller->getPrenConfig()->REFERENCE_HEIGHT
									/ boundRect[i].height;

					m_Controller->setContainerFound(distanceToContainer);

					informedController = true;
				}

				if (informedController) {
					rectangle(markedImage, cv::Point(centerX - 1, centerY - 1),
							cv::Point(centerX + 1, centerY + 1),
							cv::Scalar(0, 255, 0), 1, 8, 0);
					//m_Controller->printString("", m_Controller->OBJECT_FINDER, 0);

				} else {
					rectangle(markedImage, cv::Point(centerX - 1, centerY - 1),
							cv::Point(centerX + 1, centerY + 1),
							cv::Scalar(0, 0, 255), 1, 8, 0);
				}
			}

		}
	}
	return markedImage;
}

void ObjectFinder::updateCrossingState(bool crossingAhead) {
	m_crossingAhead = crossingAhead;
}

cv::Mat ObjectFinder::getImage() {
	pthread_mutex_lock(&m_mutex);
	cv::Mat retImg = m_MarkedImage;
	pthread_mutex_unlock(&m_mutex);
	return retImg;
}

void ObjectFinder::stopProcess() {
	m_state = false;
}
