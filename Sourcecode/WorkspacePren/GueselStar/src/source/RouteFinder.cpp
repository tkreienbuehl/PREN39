#include "../header/RouteFinder.hpp"

RouteFinder::RouteFinder(PrenController* controller)
: MINLENGTH(10), MINYDIFF(3), NROFLINES(20) {
	m_Controller = controller;
	m_PicCreator = nullptr;
	m_GradMat = nullptr;
	m_State = false;
}

RouteFinder::~RouteFinder() {
	delete m_GradMat;
}

int RouteFinder::findRoute(PictureCreator* picCreator) {

	m_PicCreator = picCreator;
	pthread_t thread;
	short i = 1;
	int rc;
	m_State = true;
	cout << "main() : creating thread, " << i << endl;
	rc = pthread_create(&thread, NULL, RouteFinder::staticEntryPoint, this);
	if (rc) {
	 cout << "Error:unable to create thread," << rc << endl;
	 exit(-1);
	}

	return 0;
}

std::string RouteFinder::formatFileName(std::string fileStr, int nr) {
	ostringstream nrStream;
	nrStream << nr;
	int start = fileStr.find("000");
	cout << nrStream.str() << endl;
	fileStr.replace(start,3,nrStream.str().c_str(),nrStream.str().length());
	cout << fileStr << endl;
	return fileStr;
}

void RouteFinder::outputMat(cv::Mat* mat, cv::Mat* changesMat) {

    // accept only char type matrices
    CV_Assert(mat->depth() == CV_8U);

    int channels = mat->channels();
    short xDiff, yDiff;
    int upperLimit, lowerLimit;

    short i,j;

    int nRows = mat->rows;
    int nCols = mat->cols * channels;

    for(i = 1; i < nRows; ++i) {
        for (j = 0; j < nCols-1; ++j) {
        	xDiff = static_cast<short>(mat->at<uchar>(i,j+1)) - static_cast<short>(mat->at<uchar>(i,j));
        	yDiff = static_cast<short>(mat->at<uchar>(i-1,j)) - static_cast<short>(mat->at<uchar>(i,j));
        	Gradient grad(xDiff,yDiff);
        	m_GradMat->setValue(i,j,grad);
        	if (abs(m_GradMat->getValue(i,j).getLength()) > MINLENGTH && m_GradMat->getValue(i,j).getYValue() > MINYDIFF) {
        		changesMat->at<uchar>(i,j) = 255;
        	}
        	else {
        		changesMat->at<uchar>(i,j) = 0;
        	}
        }
    }
    i = nRows-1;
    upperLimit = -100;
    lowerLimit = -100;
    for( i = nRows-3; i > 0; i--) {
    	if (i > nRows - NROFLINES) {
    		approxLimit(changesMat, &upperLimit, &lowerLimit, i);
    	}
    	else {
    		if (i == nRows - NROFLINES) {
    			calcAverageLimit(&upperLimit, &lowerLimit);
    		}
    	}
    }
}

void RouteFinder::approxLimit(cv::Mat* mat, int* upperLimit, int* lowerLimit, int row) {

	int val;
	bool minim = false;
	for ( int j = mat->cols/2; j <  mat->cols-1; j++) {
		val = static_cast<int>(mat->at<uchar>(row,j));
		if (val == 255) {
			if (minim == false) {
				*upperLimit = 10*(static_cast<int>(j/10)+0.5);
				minim = true;
			}
			else {
				break;
			}
		}
	}
	minim = false;
	for (int j = mat->cols/2; j > 0; j--) {
		val = static_cast<int>(mat->at<uchar>(row,j));
		if (val == 255) {
			if (minim == false) {
				*lowerLimit = 10*(static_cast<int>(j/10)+0.5);
				minim = true;
			}
			else {
				break;
			}
		}
	}
	cout << " min: " << *lowerLimit << " max: " << *upperLimit << endl;
	minVals.push_back(*lowerLimit);
	maxVals.push_back(*upperLimit);
}

void RouteFinder::calcAverageLimit(int* upperLimit, int* lowerLimit) {

	int minVal = 0, maxVal = 0, nrs;
	nrs = minVals.size();

	while (!minVals.empty()) {
		minVal += minVals.back();
		minVals.pop_back();
		maxVal += maxVals.back();
		maxVals.pop_back();
	}

	if (nrs > 0) {
		*upperLimit = maxVal / nrs;
		*lowerLimit = minVal / nrs;
	}
	//cout << " Average min" << *lowerLimit << " Average max" << *upperLimit << endl;
}

void* RouteFinder::staticEntryPoint(void* threadId) {
	((RouteFinder*)threadId)->runProcess();
	return NULL;
}

int RouteFinder::runProcess() {
	cv::Mat grayImage, reducedImg, image;
	ostringstream nrStream;

    cv::namedWindow( "The Image", CV_WINDOW_AUTOSIZE );
	cout << "Start" << endl;
    for(int i = 0; i<500; i++) {

        image = *m_PicCreator->GetImage();

        if (!image.empty()) {

			cv::resize(image, reducedImg, reducedImg.size(),0.5,0.5,cv::INTER_LANCZOS4);

			//reducedImg = image;
			cv::cvtColor(reducedImg,grayImage,CV_BGR2GRAY);

			cv::Mat diffsPic(reducedImg.rows, reducedImg.cols, cv::DataType<uchar>::type);
			m_GradMat = GradientMat::getInstance(static_cast<short>(reducedImg.rows), static_cast<short>(reducedImg.cols));
			outputMat(&grayImage, &diffsPic);

			cv::imshow("The Image", diffsPic);
			cv::imshow("The Source gray Image", grayImage);
			cv::waitKey(0);
        }
        else {
        	i--;
        }
    }

    m_Controller->SetState(m_Controller->END);
	string bye = "Now I've done my job, have fun with your pics ;)";
	cout << bye << endl;

	return 0;

}