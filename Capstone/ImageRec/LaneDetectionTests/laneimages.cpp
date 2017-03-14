/*---------------------------Lane Detection---------------------------*\
	Edits By ==> Nabil Hamza
			
	For laneimages.cpp run:
	g++ `pkg-config --cflags --libs opencv` laneimages.cpp -o laneimages
	OR
	g++ -O2 `pkg-config --cflags --libs opencv` laneimages.cpp -o laneimages ==> Optimized
	
	Execution:
	./laneimages original.bmp ==> Processing existing frame
	OR
	./laneimages ==> Capturing frame

	You can use any picture for the original image, and in the laneimages.cpp file
	There are some commented imwrite() functions you can uncomment to see the 
	image processing steps

	Probably will look into a more robust algorithm in the future
\*--------------------------------------------------------------------*/

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "linefinder.h" //Used for HoughP and 

#define PI 3.1415926

using namespace cv;

int main(int argc, char* argv[]) {
	
	//-----------------Capture & Prepare Image------------------//
	// Load the image file and check for success
	
	/* // Capture from camera
	VideoCapture capture(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    if(!capture.isOpened()){
	    std::cout << "Failed to connect to the camera." << std::endl;
    }
    Mat image;
    capture >> image;
    if(image.empty()){
		std::cout << "Failed to capture an image" << std::endl;
		return -1;
    }
	*/
	
	// Use stock image
	Mat image = imread(argv[1]);
	if(!image.data) {
		std::cout << "Unable to open the image file" << std::endl;
		return -1;
	}
		
	std::cout << image.cols << std::endl; // # Of cols in image
	std::cout << image.rows << std::endl; // # Of rows in image
	
	int top = 0;
    int left = 0;
    int width = image.cols - 1;
    int height = image.rows - 1;
	
	Rect roi(top,left,width,height);// set the ROI for the image
	Mat imgROI = image(roi);
	
	// Canny algorithm
    Mat contours;
    Canny(imgROI,contours,150,350);
    Mat contoursInv;
    threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);
	
	imwrite("contour.png", contoursInv);
	//--------------------Hough Transform-----------------------//
	std::vector<Vec2f> lines;
	int houghVote = 50; // HoughLines threshold value (higher ==> less lines, lower ==> more lines)
	
	/* // Loop for determining proper houghVote
	if (houghVote < 1 or lines.size() > 2){ // we lost all lines. reset 
		houghVote = 200; 
	}
	else{ houghVote += 25;} 
	while(lines.size() < 5 && houghVote > 0){
		houghVote = 100; 
		HoughLines(contours,lines,1,PI/180, houghVote);
		houghVote -= 5;
	}
	*/
	
	HoughLines(contours,lines,1,PI/180, houghVote);
	
	std::cout << houghVote << "\n";
	Mat result(imgROI.size(),CV_8U,Scalar(255));
	imgROI.copyTo(result);

    // Draw the limes
	std::vector<Vec2f>::const_iterator it= lines.begin();
	Mat hough(imgROI.size(),CV_8U,Scalar(0));
	while (it!=lines.end()) {

		float rho= (*it)[0];   // first element is distance rho
		float theta= (*it)[1]; // second element is angle theta
		
		// filter to remove vertical and horizontal lines
		if ( theta > (5*(PI/180)) && theta < (85*(PI/180)) || theta < (180*(PI/180)) && theta > (90*(PI/180)) ) { 
		
			// point of intersection of the line with first row
			Point pt1(rho/cos(theta),0);        
			// point of intersection of the line with last row
			Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
			// draw a white line
			line( result, pt1, pt2, Scalar(255), 8); 
			line( hough, pt1, pt2, Scalar(255), 8);
		}

		//std::cout << "Hough line: (" << rho << "," << theta << ")\n"; 
		++it;
	}
	
	imwrite("hough.png", result);
	//---------------Probabilistic Hough Transform--------------//
	// Create LineFinder instance
	LineFinder ld;

    // Set probabilistic Hough parameters
	ld.setLineLengthAndGap(30,20);
	ld.setMinVote(30);
	
    // Detect lines
	
	std::vector<Vec4i> li= ld.findLines(contours);
	Mat houghP(imgROI.size(),CV_8U,Scalar(0));
	
	
	//std::cout << "HoughP Lines" << "\n";
	ld.drawDetectedLines(houghP);
	
	
	imwrite("houghP.png", houghP);
	//-------------Bitwise Of Hough Transformations-------------//
	// Bitwise AND of the two hough images
	bitwise_and(houghP,hough,houghP);
	Mat houghPinv(imgROI.size(),CV_8U,Scalar(0));
	Mat dst(imgROI.size(),CV_8U,Scalar(0));
	threshold(houghP,houghPinv,150,255,THRESH_BINARY_INV); // threshold and invert to black lines
	
	imwrite("houghPinv.png", houghP);
	//----------------------Processed Image---------------------//
	//Second Canny transform
	Canny(houghPinv,contours,150,350);
	li= ld.findLines(contours);

	//imwrite("contours2.png", contoursInv);
	
	// Set probabilistic Hough parameters
	ld.setLineLengthAndGap(5,2);
	ld.setMinVote(10);
	//Use shifts when drawing on a non ROI image
	ld.setShiftV(left);
	ld.setShiftH(top);
	//std::cout << "Original Bitwise" << "\n";
	ld.drawDetectedLines(image);
	
	int YmidAxis = height/4;
	int XmidPiont = width/2 + top;
	
	// Middle of frame Reference
	Point Pmid1(XmidPiont, height);        
	Point Pmid2(XmidPiont,height - 30);
	line(image, Pmid1, Pmid2, Scalar(0,0,0), 4);
	
	// Midpoint Axis
	Point MidAxis1(0 + top, YmidAxis);        
	Point MidAxis2(width + top , YmidAxis);
	line(image, MidAxis1, MidAxis2, Scalar(0,0,255), 4);
	
	double XLEFT = 0.0;
	double XRIGHT = width;
	
	// Intersection Finder 
	for (int i=0; i<li.size(); ++i){
		double x1 = li[i][0] + top;
		double y1 = li[i][1] + left;
		double x2 = li[i][2] + top;
		double y2 = li[i][3] + left;
		
		double YHigh = 0.0;
		double XHigh = 0.0;
		double YLow = 0.0;
		double XLow = 0.0;
		
		if ( y2 > y1 ){
			YHigh = y2;
			XHigh = x2;
			YLow = y1;
			XLow = x1;
		} else{
			YHigh = y1;
			XHigh = x1;
			YLow = y2;
			XLow = x2;
		}
		
		// LEFT POINT
		if ( XHigh < XmidPiont && XLow < XmidPiont){
			if ( YHigh > YmidAxis && YLow < YmidAxis) {	
				double slope = (YHigh - YLow)/(XHigh - XLow);
				double b = YHigh - slope*XHigh;
				double Ltemp = (YmidAxis - b)/slope;
				if ( Ltemp > XLEFT){
					XLEFT = Ltemp;
				}
			}
		}else if ( XHigh > XmidPiont && XLow > XmidPiont) {  //RIGHT POINT
			if ( YHigh > YmidAxis && YLow < YmidAxis) {	
				double slope = (YHigh - YLow)/(XHigh - XLow);
				double b = YHigh - slope*XHigh;
				double Rtemp = (YmidAxis - b)/slope;
				if ( Rtemp < XRIGHT){
					XRIGHT = Rtemp;
				}
			}
		}
	}
	
	// Intersection Lines
	Point IntL1(XLEFT, 0);        
	Point IntL2(XLEFT, height);
	line(image, IntL1, IntL2, Scalar(0,0,255), 4);
	
	Point IntR1(XRIGHT, 0);        
	Point IntR2(XRIGHT, height);
	line(image, IntR1, IntR2, Scalar(0,0,255), 4);
	
	
	std::cout << "Gathered Bitwise" << "\n";
	for (int i=0; i<li.size(); ++i){
		std::cout << "Bitwise Line "<< i << ": [["<< li[i][0] + top <<"," << li[i][1] + left << "],[" << li[i][2] + top << "," << li[i][3] + left << "]]\n";
	}
	
	std::cout << "Left X Point: " << XLEFT << " Right X Point: " << XRIGHT << " X Mid Point: " << XmidPiont << std::endl; 
	
	/* // Write # of Lines on Image
	std::stringstream stream;
	stream << "Lines Segments: " << lines.size();
	putText(image, stream.str(), Point(10,image.rows-10), 2, 0.8, Scalar(0,0,255),0);
	*/
	
	imwrite("processed.png", image);
	//----------------------------------------------------------//
	return 0;
}
