/*
 * PictureCreator_test.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: tobias
 */

#include "ObjectFinderTest.hpp"


TEST(ObjectFinder, ImageLoadingSuccess) {

	ObjectFinder* objectFinder = new ObjectFinder(NULL, NULL);
	cv::Mat original = objectFinder->createImage("/home/patbrant/Pictures/pren/street_distance1.jpg");

	EXPECT_TRUE(original.rows != 0);
}

TEST(ObjectFinder, convertImageToHSV) {

	ObjectFinder* objectFinder = new ObjectFinder(NULL, NULL);
	cv::Mat original = objectFinder->createImage("/home/patbrant/Pictures/pren/street_distance1.jpg");
	cv::Mat hsv = objectFinder->convertImageToHSV(original);
	EXPECT_TRUE(hsv.rows != 0);
}

TEST(ObjectFinder, filterColorInImage) {

	ObjectFinder* objectFinder = new ObjectFinder(NULL, NULL);
	cv::Mat original = objectFinder->createImage("/home/patbrant/Pictures/pren/street_distance1.jpg");
	cv::Mat hsv = objectFinder->convertImageToHSV(original);

	cv::Mat filteredImageGreen = objectFinder->filterColorInImage("green", hsv);
	EXPECT_TRUE(filteredImageGreen.rows != 0);
}

TEST(ObjectFinder, findContainersInImage) {

	ObjectFinder* objectFinder = new ObjectFinder(NULL, NULL);
	cv::Mat original = objectFinder->createImage("/home/patbrant/Pictures/pren/street_distance1.jpg");
	cv::Mat hsv = objectFinder->convertImageToHSV(original);

	cv::Mat filteredImageGreen = objectFinder->filterColorInImage("green", hsv);

	vector<vector<cv::Point> > contours = objectFinder->findContainersInImage(filteredImageGreen);

	EXPECT_TRUE(contours.size() != 0);
}

TEST(ObjectFinder, markFoundContainersInImage) {

	ObjectFinder* objectFinder = new ObjectFinder(NULL, NULL);
	cv::Mat original = objectFinder->createImage("/home/patbrant/Pictures/pren/street_distance1.jpg");
	cv::Mat hsv = objectFinder->convertImageToHSV(original);

	cv::Mat filteredImageGreen = objectFinder->filterColorInImage("green", hsv);

	vector<vector<cv::Point> > contours = objectFinder->findContainersInImage(filteredImageGreen);
	cv::Mat original_copy = original;
	cv::Mat markedImage = objectFinder->markFoundContoursInImage(contours, original_copy);

	EXPECT_TRUE(cv::countNonZero(markedImage!=original) == 0);

}
