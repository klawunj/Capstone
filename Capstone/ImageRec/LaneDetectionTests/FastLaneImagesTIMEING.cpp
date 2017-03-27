/*---------------------------Lane Detection---------------------------*\
	Edits By ==> Nabil Hamza
			
	For FastLaneImages.cpp run:
	g++ `pkg-config --cflags --libs opencv` FastLaneImagesTIMEING.cpp -o TimeTest
	OR
	g++ -O2 `pkg-config --cflags --libs opencv` FastLaneImagesTIMEING.cpp -o TimeTest ==> Optimized
	
	Execution:
	./FastLaneImages original.bmp ==> Processing existing frame
	OR
	./FastLaneImages ==> Capturing frame

	You can use any picture for the original image, and in the FastLaneImages.cpp file
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
#include "linefinder.h" //Used for HoughP

#include<time.h>

#define PI 3.1415926

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {

	struct timespec start, end;
    clock_gettime( CLOCK_REALTIME, &start );
	
	//-----------------Capture & Prepare Image------------------//
	// Load the image file and check for success
	
	 // Capture from camera
	VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
	if(!capture.isOpened()){
		cout << "Failed to connect to the camera." << endl;
	}

	/*
	// Use stock image
	Mat image = imread(argv[1]);
	if(!image.data) {
		cout << "Unable to open the image file" << endl;
		return -1;
	}
	*/
	
	int frames=10;
	for(int i=0; i<frames; i++){
		
		Mat image;
		capture >> image;
		if(image.empty()){
			cout << "Failed to capture an image" << endl;
			return -1;
		}
		
	
		//cout << image.cols << endl; // # Of cols in image
		//cout << image.rows << endl; // # Of rows in image
		
		int ROIx = 0;
		int ROIy = 0;
		int width = image.cols - 1;
		int height = image.rows - 1;
		
		Rect roi(ROIx,ROIy,width,height);// set the ROI for the image
		Mat imgROI = image(roi);
		
		// Canny algorithm
		Mat contours;
		Canny(imgROI,contours,150,350);
		Mat contoursInv;
		threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);
		
		//imwrite("FastContour.png", contoursInv);
		//---------------Probabilistic Hough Transform--------------//
		// Create LineFinder instance
		LineFinder ld;

		// Set probabilistic Hough parameters
		ld.setLineLengthAndGap(30,25);
		ld.setMinVote(30);
		
		ld.setShiftV(ROIy);
		ld.setShiftH(ROIx);
		
		// Detect lines
		vector<Vec4i> li= ld.findLines(contours);
		
		double btheta = 15*(PI/180);// The Horizontal filter angle
		ld.drawFilterHLines(image,btheta);
		//imwrite("FastHoughP.png", image);
		//-----------------Find Intersection Points-----------------//
		
		int YmidAxis = height/4 + ROIy;
		int XmidPiont = width/2 + ROIx;
		
		/*
		// Middle of frame Reference
		Point Pmid1(XmidPiont, height + ROIy);        
		Point Pmid2(XmidPiont,height - 30 + ROIy);
		line(image, Pmid1, Pmid2, Scalar(0,0,0), 4);
		
		// Midpoint Axis
		Point MidAxis1(0 + ROIx, YmidAxis);        
		Point MidAxis2(width + ROIx , YmidAxis);
		line(image, MidAxis1, MidAxis2, Scalar(0,0,255), 4);
		*/
		double XLEFT = 0.0 + ROIx;
		double XRIGHT = width + ROIy;
		
		// Intersection Finder 
		for (int i=0; i<li.size(); ++i){
			double x1 = li[i][0] + ROIx;
			double y1 = li[i][1] + ROIy;
			double x2 = li[i][2] + ROIx;
			double y2 = li[i][3] + ROIy;
			
			double YHigh = 0.0;
			double XHigh = 0.0;
			double YLow = 0.0;
			double XLow = 0.0;
			
			if ( y2 > y1 ){
				YHigh = y2;
				XHigh = x2;
				YLow = y1;
				XLow = x1;
			}else {
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
			// RIGHT POINT
			}else if ( XHigh > XmidPiont && XLow > XmidPiont) {  
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
		
		/*
		// Intersection Lines
		Point IntL1(XLEFT, 0 + ROIy);        
		Point IntL2(XLEFT, height + ROIy);
		line(image, IntL1, IntL2, Scalar(0,0,255), 4);
		
		Point IntR1(XRIGHT, 0 + ROIy);        
		Point IntR2(XRIGHT, height + ROIy);
		line(image, IntR1, IntR2, Scalar(0,0,255), 4);
		
		cout << "Gathered Bitwise" << "\n";
		for (int i=0; i<li.size(); ++i){
			cout << "Bitwise Line "<< i << ": [["<< li[i][0] + ROIx <<"," << li[i][1] + ROIy << "],[" << li[i][2] + ROIx << "," << li[i][3] + ROIy << "]]\n";
		}
		
		cout << "Left X Point: " << XLEFT << " Right X Point: " << XRIGHT << " X Mid Point: " << XmidPiont << endl; 
		
		imwrite("FastImage.png", image);
		*/
	}
	clock_gettime( CLOCK_REALTIME, &end );
    double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/1000000000.0d;
    cout << "It took " << difference << " seconds to process " << frames << " frames" << endl;
    cout << "Capturing and processing " << frames/difference << " frames per second " << endl;
	
	
	//----------------------------------------------------------//
	return 0;
}
