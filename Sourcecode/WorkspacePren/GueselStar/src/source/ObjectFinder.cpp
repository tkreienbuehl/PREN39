#include "../header/ObjectFinder.hpp"

#include <sstream>
#include <string>

using namespace std;

ObjectFinder::ObjectFinder(PrenController* controller,
		PictureCreator* picCreator) {
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_state = false;
	pthread_mutex_init(&m_mutex,NULL);
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

		if(1==0) { //ultraschall
		}
		else {

		cv::Mat image = m_PicCreator->GetImage();

			//origImage = createImage(
					//"/home/patbrant/Pictures/pren/street_distance1.jpg");

		origImage = image.clone();
			if (!origImage.empty()) {
				cv::Rect areaToCrop(origImage.cols / 2, 0, origImage.cols / 2,
						origImage.rows);

				croppedImage = origImage(areaToCrop);
				hsvImage = convertImageToHSV(croppedImage);
				filteredImageGreen = filterColorInImage("green", hsvImage);
				filteredImageBlue = filterColorInImage("blue", hsvImage);
				contoursGreen = findContainersInImage(filteredImageGreen);
				contoursBlue = findContainersInImage(filteredImageBlue);
				contours = mergeContours(contoursGreen, contoursBlue);
				resultImage = markFoundContoursInImage(contours, croppedImage);
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

	cvtColor(rgbImage, HSVImage, CV_RGB2HSV);
	return HSVImage;
}

cv::Mat ObjectFinder::filterColorInImage(cv::String color,
		cv::Mat imageToFilter) {

	cv::Mat filteredImage;
	if (color == "green") {
		cv::inRange(imageToFilter, cv::Scalar(m_Controller->getPrenConfig()->GREEN_RANGE_H_LOW, m_Controller->getPrenConfig()->GREEN_RANGE_S_LOW, m_Controller->getPrenConfig()->GREEN_RANGE_V_LOW),
				cv::Scalar(m_Controller->getPrenConfig()->GREEN_RANGE_H_HIGH, m_Controller->getPrenConfig()->GREEN_RANGE_S_HIGH, m_Controller->getPrenConfig()->GREEN_RANGE_V_HIGH), filteredImage);
	} else if (color == "blue") {
		cv::inRange(imageToFilter, cv::Scalar(m_Controller->getPrenConfig()->BLUE_RANGE_H_LOW, m_Controller->getPrenConfig()->BLUE_RANGE_S_LOW, m_Controller->getPrenConfig()->BLUE_RANGE_V_LOW),
						cv::Scalar(m_Controller->getPrenConfig()->BLUE_RANGE_H_HIGH, m_Controller->getPrenConfig()->BLUE_RANGE_S_HIGH, m_Controller->getPrenConfig()->BLUE_RANGE_V_HIGH), filteredImage);
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

		if (boundRect[i].height > boundRect[i].width
				&& boundRect[i].height > 150) {

			rectangle(markedImage, boundRect[i].tl(), boundRect[i].br(),
					cv::Scalar(0, 255, 0), 2, 8, 0);

			int distanceToContainer = m_Controller->getPrenConfig()->REFERENCE_DISTANCE * m_Controller->getPrenConfig()->REFERENCE_HEIGHT / boundRect[i].height;

			m_Controller->setContainerFound(distanceToContainer);

			std::ostringstream stringConverter;
			stringConverter << distanceToContainer;
			std::string distanceString = stringConverter.str();

			putText(markedImage,
					"Distance to Container: " + distanceString + "mm",
					cvPoint(30, 30),
					CV_FONT_HERSHEY_PLAIN, 1.0, cvScalar(200, 200, 250), 1,
					CV_AA);
		}
	}

	return markedImage;
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
