#include "../header/ObjectFinder.hpp"

#include <sstream>
#include <string>

using namespace std;

ObjectFinder::ObjectFinder(PrenController* controller) {
	m_Controller = controller;
	m_PicCreator = nullptr;
}

ObjectFinder::~ObjectFinder() {

}

void ObjectFinder::findObjects(PictureCreator* picCreator) {

	pthread_t thread;
	short i = 2;
	int rc;

	cout << "main() : creating thread, " << i << endl;
	rc = pthread_create(&thread, NULL, ObjectFinder::StaticEntryPoint, this);

	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		exit(-1);
	}
}

void* ObjectFinder::StaticEntryPoint(void* threadId) {
	((ObjectFinder*) threadId)->RunProcess();
	return NULL;
}

void ObjectFinder::RunProcess() {


		for (int i = 0; i < 500; i++) {

			origImage = *m_PicCreator->GetImage();

			if (!origImage.empty()) {

				hsvImage = convertImageToHSV(origImage);
				filteredImageGreen = filterColorInImage("green", hsvImage);
				filteredImageBlue = filterColorInImage("blue", hsvImage);
				contoursGreen = findContainersInImage(filteredImageGreen);
				contoursBlue = findContainersInImage(filteredImageBlue);
				contours = mergeContours(contoursGreen, contoursBlue);
				markedImage = markFindContoursInImage(contours, origImage);

				cv::namedWindow("Street",
						cv::WINDOW_AUTOSIZE);
				imshow("Street", markedImage);
				cv::waitKey(0);
			}
			else {
				i--;
			}


		}


}

cv::Mat ObjectFinder::createImage(cv::String filename) {
cv::Mat srcImage = imread(filename, 1);

if (srcImage.data == NULL) {
	printf("file cannot be loaded\n");
}

cv::Mat origImage;
cv::Size size(600, 430);
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
for (int i = 0; i < contours.size(); i++) {
	cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
	boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));

	if (boundRect[i].height > boundRect[i].width && boundRect[i].height > 50) {
		rectangle(markedImage, boundRect[i].tl(), boundRect[i].br(),
				cv::Scalar(0, 255, 0), 2, 8, 0);
	}
}

return markedImage;
}

