#pragma once
#include <gtest/gtest.h>

class PictureCreatorTest : public testing::Test {

public:
	PictureCreatorTest();
	~PictureCreatorTest();

	int GetValue();

private:


	int myTestVar;

};
