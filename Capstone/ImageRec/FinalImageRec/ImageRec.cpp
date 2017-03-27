/*---------------------------Lane Detection---------------------------*\
	Edits By ==> Nabil Hamza
			
	For ImageRec.cpp run:
	g++ -std=c++0x `pkg-config --cflags --libs opencv` serialib.cpp ImageRec.cpp -o ImageRec
	OR
	g++ -std=c++0x -O2 `pkg-config --cflags --libs opencv` serialib.cpp ImageRec.cpp -o ImageRec ==> Optimized
	
	Execution:
	./ImageRec original.bmp ==> Processing existing frame
	OR
	./ImageRec ==> Capturing frame

	You can use any picture for the original image, and in the ImageRec.cpp file
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
#include <string>

#define PI 3.1415926
#define MOTOR_TURNING 36
#define MAX_TURNING_ANGLE 38

#include "serialib.h"

#ifdef __linux__
#define     DEVICE_PORT    "/dev/ttyS1"
#endif

using namespace cv;
using namespace std;

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
			LRtemp = YHigh;
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
			}else if ( XHigh > xMID/2 && XLow > xMID/2) {  
				if ( YHigh > yAXIS && YLow < yAXIS) {	
					if ( (LRtemp < WIDTH + xSHIFT) && (LRtemp < XRIGHT)){
						XRIGHT = LRtemp;
					}
				}
			}
		} else { // RIGHT ANGLE LINE
			// LEFT POINT
			if ( XHigh < xMID*1.5 && XLow < xMID*1.5){
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
    if(!capture.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
    }
	
	
	//int frames=10;
	while(1){
	
		Mat image;
		capture >> image;
		if(image.empty()){
			cout << "Failed to capture an image" << endl;
			return -1;
		}
		
		//cout << image.cols << endl; // # Of cols in image
		//cout << image.rows << endl; // # Of rows in image
		
		int ROIx = 0;
		int ROIy = (image.rows - 1)/8;
		int width = image.cols - 1;
		int height = (image.rows - 1)/8;
		
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
		int YmidAxis = ROIy;
		int XmidPiont = width/2 + ROIx;
		
		double XLEFT = 0.0 + ROIx;
		double XRIGHT = width + ROIx;
		
		int count = 0;
		
		while ( XLEFT == 0.0 + ROIx || XRIGHT == width + ROIx){
			intersectionFinder(XLEFT,XRIGHT,YmidAxis + count,width, XmidPiont, ROIx, ROIy , li);
			count++;
			if (count >= height){
				break;
			}
		}
		cout << count << endl;
		
		cout << "Left X Point: " << XLEFT << " Right X Point: " << XRIGHT << " X Mid Point: " << XmidPiont << endl; 
		
		int Angle = 18 - (638-XLEFT-XRIGHT)/10;
		//imwrite("FastImage.png", image);
		//----------------------------------------------------------//
		
		
		serialib LS; //the main class to access
		int Ret;
		char Buffer[128];

		Ret= LS.Open(DEVICE_PORT,9600);
		if (Ret!=1){
		  return 0;
		}
		Ret=LS.ReadString(Buffer,'\n',128,5000);                                // Read a maximum of 128 characters with a timeout of 5 seconds

		string s = "2 " + to_string(Angle) + " 0#";
		char const *pchar = s.c_str();
		Ret=LS.WriteString(pchar);                                           
		if (Ret!=1) {                                                                                        
			return Ret;                                                         
		}
		cout << s << endl;
		
		LS.Close();
	}
	return 0;
}
