#include "../header/ObjectFinder.hpp"

#include <sstream>
#include <string>

using namespace std;

ObjectFinder::ObjectFinder(PrenController* controller ,PictureCreator* picCreator) {
	m_Controller = controller;
	m_PicCreator = picCreator;
	m_state = false;
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

	m_state = true;

	while (m_state) {

		//origImage = *m_PicCreator->GetImage();

		origImage = createImage("/home/patbrant/Pictures/pren/street_distance1.jpg");

		if ( !origImage.empty()) {
			cv::Rect areaToCrop(origImage.cols / 2, 0, origImage.cols / 2, origImage.rows);
			croppedImage = origImage(areaToCrop);
			hsvImage = convertImageToHSV(croppedImage);
			filteredImageGreen = filterColorInImage("green", hsvImage);
			filteredImageBlue = filterColorInImage("blue", hsvImage);
			contoursGreen = findContainersInImage(filteredImageGreen);
			contoursBlue = findContainersInImage(filteredImageBlue);
			contours = mergeContours(contoursGreen, contoursBlue);
			m_MarkedImage = markFindContoursInImage(contours, croppedImage);

			cout << "Picture worked" << endl;
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
		cv::inRange(imageToFilter, cv::Scalar(38, 65, 10), cv::Scalar(75, 255, 255),
				filteredImage);
	} else if (color == "blue") {
		cv::inRange(imageToFilter, cv::Scalar(10, 140, 90),
				cv::Scalar(20, 255, 255), filteredImage);
	} else if (color == "yellow") {
		cv::inRange(imageToFilter, cv::Scalar(20, 100, 100),
				cv::Scalar(38, 255, 255), filteredImage);
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

cv::Mat ObjectFinder::markFindContoursInImage(
	vector<vector<cv::Point> > contours, cv::Mat imageToMarkContainer) {

	vector<vector<cv::Point> > contours_poly(contours.size());
	vector<cv::Rect> boundRect(contours.size());
	cv::Mat markedImage = imageToMarkContainer;
	for (unsigned int i = 0; i < contours.size(); i++) {
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));

		if (boundRect[i].height > boundRect[i].width && boundRect[i].height > 150) {
			rectangle(markedImage, boundRect[i].tl(), boundRect[i].br(),
					cv::Scalar(0, 255, 0), 2, 8, 0);

			int distanceToContainer = 280 * 216 / boundRect[i].height;

			 std::ostringstream stringConverter;
			 stringConverter << distanceToContainer;
			 std::string distanceString = stringConverter.str();

			putText(markedImage, "Distance to Container: " + distanceString + "mm", cvPoint(30,30),
					CV_FONT_HERSHEY_PLAIN, 1.5, cvScalar(200,200,250), 1, CV_AA);
		}
	}

	return markedImage;
}

cv::Mat ObjectFinder::getImage() {
	return m_MarkedImage;
}

void ObjectFinder::stopProcess() {
	m_state = false;
}
