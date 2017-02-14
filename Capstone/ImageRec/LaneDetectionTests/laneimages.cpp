/*---------------------------------------------------------------------*\
	Lane Detection
	Edits(2017-02-14)==>Nabil Hamza
			
	For laneimages.cpp run:
	g++ `pkg-config --libs opencv` laneimages.cpp -o laneimages
	./laneimages original.bmp

	You can use any picture for the original image, and in the laneimages.cpp file
	There are some commented imwrite() functions you can uncomment to see the 
	image proccessing steps

	Probably will look into a more roubust alcorithim in the future
\*---------------------------------------------------------------------*/

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "linefinder.h"

#define PI 3.1415926

using namespace cv;

int main(int argc, char* argv[]) {
	int houghVote = 200;
	// Load the image file and check for success
	Mat image = imread(argv[1]);
	if(!image.data) {
		std::cout << "Unable to open the image file" << std::endl;
		return -1;
	}
	
	Rect roi(0,image.cols/3,image.cols-1,image.rows - image.cols/3);// set the ROI for the image
	Mat imgROI = image(roi);
	
	// Canny algorithm
    Mat contours;
    Canny(imgROI,contours,50,350);
    Mat contoursInv;
    threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);
	
	//imwrite("contour.bmp", contoursInv);
	//---------------------------------------------------------//
	std::vector<Vec2f> lines;
	if (houghVote < 1 or lines.size() > 2){ // we lost all lines. reset 
		houghVote = 200; 
	}
	else{ houghVote += 25;} 
	while(lines.size() < 5 && houghVote > 0){
		HoughLines(contours,lines,1,PI/180, houghVote);
		houghVote -= 5;
	}
	std::cout << houghVote << "\n";
	Mat result(imgROI.size(),CV_8U,Scalar(255));
	imgROI.copyTo(result);

   // Draw the limes
	std::vector<Vec2f>::const_iterator it= lines.begin();
	Mat hough(imgROI.size(),CV_8U,Scalar(0));
	while (it!=lines.end()) {

		float rho= (*it)[0];   // first element is distance rho
		float theta= (*it)[1]; // second element is angle theta
		
		if ( theta > 0.09 && theta < 1.48 || theta < 3.14 && theta > 1.66 ) { // filter to remove vertical and horizontal lines
		
			// point of intersection of the line with first row
			Point pt1(rho/cos(theta),0);        
			// point of intersection of the line with last row
			Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
			// draw a white line
			line( result, pt1, pt2, Scalar(255), 8); 
			line( hough, pt1, pt2, Scalar(255), 8);
		}

		//std::cout << "line: (" << rho << "," << theta << ")\n"; 
		++it;
	}
	
	//imwrite("hough.bmp", result);
	//----------------------------------------------------------//
	
	// Create LineFinder instance
	LineFinder ld;

   // Set probabilistic Hough parameters
	ld.setLineLengthAndGap(60,10);
	ld.setMinVote(4);

   // Detect lines
	std::vector<Vec4i> li= ld.findLines(contours);
	Mat houghP(imgROI.size(),CV_8U,Scalar(0));
	ld.setShift(0);
	ld.drawDetectedLines(houghP);
	std::cout << "First Hough" << "\n";

	//imwrite("houghP.bmp", houghP);
	//----------------------------------------------------------//
	
   // bitwise AND of the two hough images
	bitwise_and(houghP,hough,houghP);
	Mat houghPinv(imgROI.size(),CV_8U,Scalar(0));
	Mat dst(imgROI.size(),CV_8U,Scalar(0));
	threshold(houghP,houghPinv,150,255,THRESH_BINARY_INV); // threshold and invert to black lines
	
	//imwrite("houghPinv.bmp", houghP);
	//----------------------------------------------------------//
	
	Canny(houghPinv,contours,100,350);
	li= ld.findLines(contours);
   // Display Canny image

	//imwrite("contours2.bmp", contoursInv);
	//----------------------------------------------------------//

	   // Set probabilistic Hough parameters
	ld.setLineLengthAndGap(5,2);
	ld.setMinVote(1);
	ld.setShift(image.cols/3);
	ld.drawDetectedLines(image);
		
	std::stringstream stream;
	stream << "Lines Segments: " << lines.size();
	
	putText(image, stream.str(), Point(10,image.rows-10), 2, 0.8, Scalar(0,0,255),0);
		
	imwrite("processed.bmp", image);
	//----------------------------------------------------------//
	return 0;
}
