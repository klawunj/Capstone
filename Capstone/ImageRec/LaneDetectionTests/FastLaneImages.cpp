/*---------------------------Lane Detection---------------------------*\
	Edits By ==> Nabil Hamza
			
	For FastLaneImages.cpp run:
	g++ -std=c++0x `pkg-config --cflags --libs opencv` FastLaneImages.cpp -o FastLaneImages
	OR
	g++ -std=c++0x -O2 `pkg-config --cflags --libs opencv` FastLaneImages.cpp -o FastLaneImages ==> Optimized
	
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

#define PI 3.1415926

using namespace cv;
using namespace std;
void maxGapFinder (double &XLEFT, double &XRIGHT,int yAXIS ,int WIDTH, int xMID, int xSHIFT, int ySHIFT , vector<Vec4i> LINES){
//Here we add the shifts and sort the coordinates by comparing Y values
	
	double sortedSlopeLINES[LINES.size()];
	double sortedLINES[LINES.size()][4];
	for (int i=0; i<LINES.size(); ++i){
		double x1 = LINES[i][0] + xSHIFT;
		double y1 = LINES[i][1] + ySHIFT;
		double x2 = LINES[i][2] + xSHIFT;
		double y2 = LINES[i][3] + ySHIFT;
		
		double YHigh = 0.0;
		double XHigh = 0.0;
		double YLow = 0.0;
		double XLow = 0.0;
		
		//The Higher Y Value is Lower on the Frame, but has a higher value
		//The X values are only orientated with the Y values, so XHigh is not
		//Always higher than Xlow
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
		
		double slope;
		
		// Avoid Infinity or zero in calculations
		if ( YHigh == YLow){
			slope = 0;
		} else if (XHigh == XLow){
			slope = 9999;
		} else {
			slope = (YHigh - YLow)/(XHigh - XLow);
		}
		
		sortedLINES[i][0] = XHigh;
		sortedLINES[i][1] = YHigh;
		sortedLINES[i][2] = XLow;
		sortedLINES[i][3] = YLow;
		
		sortedSlopeLINES[i] = slope;
	}
	
	int c,d;
	double swap, swap2;
	
	for (c = 0 ; c < ( LINES.size() - 1 ); c++) {
		for (d = 0 ; d < LINES.size() - c - 1; d++) {
			if (sortedLINES[d][0] > sortedLINES[d+1][0]) {
				swap = sortedLINES[d][0];
				swap2 = sortedSlopeLINES[d];
				sortedLINES[d][0] = sortedLINES[d+1][0];
				sortedSlopeLINES[d] = sortedSlopeLINES[d+1];
				sortedLINES[d+1][0] = swap;
				sortedSlopeLINES[d+1] = swap2;
			}
		}
	}
	int lineMarker;
	int X1,X2;
	int GAP = 0;
	for (int i=0; i<LINES.size() + 1; ++i){
		cout << i << endl;
		if (i == 0){
			X1 = 0;
			X2 = sortedLINES[i][0];
		}else if(i == LINES.size()){
			X1 = sortedLINES[i-1][0];
			X2 = WIDTH + xSHIFT;
		}else{
			X1 = sortedLINES[i-1][0];
			X2 = sortedLINES[i][0];
		}
		if ( ((X2 - X1) > GAP )){
			if ((sortedSlopeLINES[i-1] >= 0) && (X2 >= WIDTH + xSHIFT)){
			} else if ((sortedSlopeLINES[i] <=  0) && (X1 == 0)){
			} else {
				GAP = X2 - X1;
				lineMarker = i;
			}
		}
	}
	
	if (lineMarker == 0){
		XLEFT = 0.0;
		XRIGHT = sortedLINES[0][0];
		cout << sortedSlopeLINES[0] << endl;
	}else if( lineMarker == LINES.size()){
		XLEFT = sortedLINES[lineMarker-1][0];
		XRIGHT = WIDTH + xSHIFT;
		cout << sortedSlopeLINES[lineMarker-1] << endl;
		cout << X2 << endl;
		cout << WIDTH + xSHIFT << endl;
	}else{
		XLEFT = sortedLINES[lineMarker-1][0];
		XRIGHT = sortedLINES[lineMarker][0];
	}
	
}

void intersectionFinder( double &XLEFT, double &XRIGHT,int yAXIS ,int WIDTH, int xMID, int xSHIFT, int ySHIFT , vector<Vec4i> LINES){
	
	// Intersection Finder 
	for (int i=0; i<LINES.size(); ++i){
		double x1 = LINES[i][0] + xSHIFT;
		double y1 = LINES[i][1] + ySHIFT;
		double x2 = LINES[i][2] + xSHIFT;
		double y2 = LINES[i][3] + ySHIFT;
		
		double YHigh = 0.0;
		double XHigh = 0.0;
		double YLow = 0.0;
		double XLow = 0.0;
		
		//The Higher Y Value is Lower on the Frame, but has a higher value
		//The X values are only orientated with the Y values, so XHigh is not
		//Always higher than Xlow
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
		double slope,b,LRtemp;
		
		// Avoid Infinity or zero in calculations
		if ( YHigh == YLow){
			LRtemp = XHigh;
		} else if (XHigh == XLow){
			LRtemp = XHigh;
		} else {
			slope = (YHigh - YLow)/(XHigh - XLow);
			b = YHigh - slope*XHigh;
			LRtemp = (yAXIS - b)/slope;
		}
		
		if (slope > 0){ // LEFT ANGLE LINE
			// LEFT POINT
			if ( XHigh < xMID/2 && XLow < xMID/2){
				if ( YHigh > yAXIS && YLow < yAXIS) {	
					if ( (LRtemp > 0.0 + xSHIFT) && (LRtemp > XLEFT)){
						XLEFT = LRtemp;
					}
				}
			// RIGHT POINT
			}else if ( XHigh > xMID*1.5 && XLow > xMID*1.5) {  
				if ( YHigh > yAXIS && YLow < yAXIS) {	
					if ( (LRtemp < WIDTH + xSHIFT) && (LRtemp < XRIGHT)){
						XRIGHT = LRtemp;
					}
				}
			}else { // Middle Quadrants
				if ( YHigh > yAXIS && YLow < yAXIS) {
					if (slope > 2 ){
						if ( (LRtemp > 0.0 + xSHIFT) && (LRtemp > XLEFT)){
							XLEFT = LRtemp;
						}
					}else{
						if ( (LRtemp < WIDTH + xSHIFT) && (LRtemp < XRIGHT)){
							XRIGHT = LRtemp;
						}
					}
				}
			}
			
		} else { // RIGHT ANGLE LINE
			// LEFT POINT
			if ( XHigh < xMID/2 && XLow < xMID/2){
				if ( YHigh > yAXIS && YLow < yAXIS) {	
					if ( (LRtemp > 0.0 + xSHIFT) && (LRtemp > XLEFT)){
						XLEFT = LRtemp;
					}
				}
			// RIGHT POINT
			}else if ( XHigh > xMID*1.5 && XLow > xMID*1.5) {  
				if ( YHigh > yAXIS && YLow < yAXIS) {	
					if ( (LRtemp < WIDTH + xSHIFT) && (LRtemp < XRIGHT)){
						XRIGHT = LRtemp;
					}
				}
			}else { // Middle Quadrants
				if ( YHigh > yAXIS && YLow < yAXIS) {
					if (slope < -2 ){
						if ( (LRtemp < WIDTH + xSHIFT) && (LRtemp < XRIGHT)){
							XRIGHT = LRtemp;
						}
					}else{
						if ( (LRtemp > 0.0 + xSHIFT) && (LRtemp > XLEFT)){
							XLEFT = LRtemp;
						}
					}
				}
			}
		}
	}
}


int main(int argc, char* argv[]) {
	
	//-----------------Capture & Prepare Image------------------//
	// Load the image file and check for success
	
	// Capture from camera
	VideoCapture capture(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
	capture.set(CV_CAP_PROP_FOURCC, CV_FOURCC('H', '2', '6', '4'));
    if(!capture.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
    }
    Mat image;
    capture >> image;
    if(image.empty()){
		cout << "Failed to capture an image" << endl;
		return -1;
    }
	
	
	/*
	// Use stock image
	Mat image = imread(argv[1]);
	if(!image.data) {
		cout << "Unable to open the image file" << endl;
		return -1;
	}
	*/	
	cout << image.cols << endl; // # Of cols in image
	cout << image.rows << endl; // # Of rows in image
	
	int ROIx = 0;
    int ROIy = (image.rows - 1)/4;
    int width = image.cols - 1;
    int height = (image.rows - 1)/8;
	
	Rect roi(ROIx,ROIy,width,height);// set the ROI for the image
	Mat imgROI = image(roi);
	
	// HSV algorithm
	Mat imgThresholded, gray;
	cvtColor(imgROI, gray, CV_BGR2GRAY);
	cvtColor(imgROI, imgThresholded, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    inRange(imgThresholded, Scalar(0, 0, 190), Scalar(180, 255, 255), imgThresholded); //Threshold the image
	imwrite("FastHSV.png", imgThresholded);
	
	// Canny algorithm
    Mat contours;
    Canny(imgThresholded,contours,50,150);
    Mat contoursInv;
    threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);
	
	imwrite("FastContour.png", contoursInv);
	
	
	
	//---------------Probabilistic Hough Transform--------------//
	// Create LineFinder instance
	LineFinder ld;

    // Set probabilistic Hough parameters
	ld.setLineLengthAndGap(20,15);
	ld.setMinVote(20);
	
	ld.setShiftV(ROIy);
	ld.setShiftH(ROIx);
	
    // Detect lines
	vector<Vec4i> li= ld.findLines(contours);
	
	
	double btheta = 15*(PI/180);// The Horizontal filter angle
	ld.drawFilterHLines(image,btheta);
	imwrite("FastHoughP.png", image);
	//-----------------Find Intersection Points-----------------//
	
	int YmidAxis = ROIy;
	int XmidPiont = width/2 + ROIx;
	
	// Middle of frame Reference
	Point Pmid1(XmidPiont, image.rows - 1);        
	Point Pmid2(XmidPiont,image.rows - 1 - 30);
	line(image, Pmid1, Pmid2, Scalar(0,0,0), 4);
	
	double XLEFT = 0.0 + ROIx;
	double XRIGHT = width + ROIx;
	
	/*
	int count = 0;
	
	while ( XLEFT == 0.0 + ROIx || XRIGHT == width + ROIx){
		intersectionFinder(XLEFT,XRIGHT,YmidAxis + count,width, XmidPiont, ROIx, ROIy , li);
		count++;
		if (count >= height){
			break;
		}
	}
	cout << count << endl;
	*/
	if ( li.size() != 0){
		maxGapFinder(XLEFT,XRIGHT,YmidAxis,2*XmidPiont, XmidPiont, ROIx, ROIy , li);
	}
	/*
	// Midpoint Axis
	Point MidAxis1(0 + ROIx, YmidAxis + count);        
	Point MidAxis2(width + ROIx , YmidAxis + count);
	line(image, MidAxis1, MidAxis2, Scalar(0,0,255), 4);
	*/
	// Intersection Lines
	Point IntL1(XLEFT, 0);        
	Point IntL2(XLEFT, image.rows - 1);
	line(image, IntL1, IntL2, Scalar(0,0,255), 4);
	
	Point IntR1(XRIGHT, 0);        
	Point IntR2(XRIGHT, image.rows - 1);
	line(image, IntR1, IntR2, Scalar(0,0,255), 4);
	
	cout << "Gathered Bitwise" << "\n";
	for (int i=0; i<li.size(); ++i){
		cout << "Bitwise Line "<< i << ": [["<< li[i][0] + ROIx <<"," << li[i][1] + ROIy << "],[" << li[i][2] + ROIx << "," << li[i][3] + ROIy << "]]\n";
		
		double x1 = li[i][0] + ROIx;
		double y1 = li[i][1] + ROIy;
		double x2 = li[i][2] + ROIx;
		double y2 = li[i][3] + ROIy;
		
		double YHigh = 0.0;
		double XHigh = 0.0;
		double YLow = 0.0;
		double XLow = 0.0;
		
		//The Higher Y Value is Lower on the Frame, but has a higher value
		//The X values are only orientated with the Y values, so XHigh is not
		//Always higher than Xlow
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
		double slope;
		
		// Avoid Infinity or zero in calculations
		if ( YHigh == YLow){
			slope = 0;
		} else if (XHigh == XLow){
			slope = 9999;
		} else {
			slope = (YHigh - YLow)/(XHigh - XLow);
		}
		
		cout << "Slope :" << slope << endl;
	}
	
	cout << "Left X Point: " << XLEFT << " Right X Point: " << XRIGHT << " X Mid Point: " << XmidPiont << endl; 
	
	imwrite("FastImage.png", image);
	//----------------------------------------------------------//
	return 0;
}
