// Test to convert a color image to gray
// Build on Linux with:
// g++ test_2.cpp -o test_2 -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "linefinder.h"

using namespace cv;

int main(int argc, char* argv[]) {
	int houghVote = 200;
	// Load the image file and check for success
	Mat image = imread(argv[1]);
	if(!image.data) {
		std::cout << "Unable to open the image file" << std::endl;
		return -1;
	}

	// Canny algorithm
    Mat contours;
    Canny(image,contours,50,350);
    Mat contoursInv;
    threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);
	
	imwrite("contour.bmp", contoursInv);
	
	return 0;
}
