/*---------------------------Lane Detection---------------------------*\
	Edits By ==> Nabil Hamza
			
	For ImageRec.cpp run:
	g++ -std=c++0x `pkg-config --cflags --libs opencv` serialib.cpp ImageRec.cpp -o ImageRec
	OR
	g++ -std=c++0x -O2 `pkg-config --cflags --libs opencv` serialib.cpp ImageRec.cpp -o ImageRec ==> Optimized
	g++ -std=c++0x -O3 `pkg-config --cflags --libs opencv` serialib.cpp ImageRec.cpp -o ImageRec ==> Optimized
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

void maxGapFinder (double &XLEFT, double &XRIGHT,int yAXIS ,int WIDTH, int xMID, int xSHIFT, int ySHIFT , vector<Vec4i> LINES){
//Here we add the shifts and sort the coordinates by comparing Y values

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
		
		sortedLINES[i][0] = XHigh;
		sortedLINES[i][1] = YHigh;
		sortedLINES[i][2] = XLow;
		sortedLINES[i][3] = YLow;
	}
	
	int c,d;
	double swap;
	
	for (c = 0 ; c < ( LINES.size() - 1 ); c++) {
		for (d = 0 ; d < LINES.size() - c - 1; d++) {
			if (sortedLINES[d][0] > sortedLINES[d+1][0]) {
				swap = sortedLINES[d][0];
				sortedLINES[d][0] = sortedLINES[d+1][0];
				sortedLINES[d+1][0] = swap;
			}
		}
	}
	int lineMarker;
	double X1,X2;
	double GAP = 0;
	for (int i=0; i<LINES.size() + 1; ++i){
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
		if ( (X2 - X1) > GAP){
			GAP = X2 - X1;
			lineMarker = i;	
		}
	}
	
	if (lineMarker == 0){
		XLEFT = 0.0;
		XRIGHT = sortedLINES[0][0];
	}else if( lineMarker == LINES.size()){
		XLEFT = sortedLINES[lineMarker-1][0];
		XRIGHT = WIDTH + xSHIFT;
	}else{
		XLEFT = sortedLINES[lineMarker-1][0];
		XRIGHT = sortedLINES[lineMarker][0];
	}
	
}
void slopemaxGapFinder (double &XLEFT, double &XRIGHT,int yAXIS ,int WIDTH, int xMID, int xSHIFT, int ySHIFT , vector<Vec4i> LINES){
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
	}else if( lineMarker == LINES.size()){
		XLEFT = sortedLINES[lineMarker-1][0];
		XRIGHT = WIDTH + xSHIFT;
	}else if( (lineMarker > LINES.size()) || (lineMarker < 0)){
		//ERROR PREVENT SEGMENTATION FAULT
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
	//-----------------Main Variables------------------//
	// MISCELLANEOUS FLAGS, BUFFERS & TEMP VARS
	int firstrunFlag = 0;
	int countBuff = 0;
	int prevAngle;
	int Angle = 18;
	int speed = 1;
	int speedCounter = 0;
	int prevSpeed = 1;
	int cornerTRIGGER = 0;
	int cornerSPEED = 1;
	int cornerSPEEDthresh = 3;
	
	double XLEFT;
	double XRIGHT;
	double XRIGHT_PREV;
	double XLEFT_PREV;
	
	//ROI VARS
	int ROIx;
	int ROIy;
	int width;
	int height;
	
	int COLORx;
	int COLORy;
	int COLORwidth;
	int COLORheight;
	
	//IMPORTANT AXIS
	int YmidAxis;
	int XmidPiont;
	
	//COLOR VARS
	int pixleSUMRED;
	int pixleSUMGREEN;
	int colorBuffRED = 0;
	int colorBuffGREEN= 0;
    int serialColor = 0;
	int colorSENDbuff = 0;
	int colorSENDTRIGGER = 0;
	//SERIAL COMUNICATION 
	int Ret,Ret2;
	char Buffer[128];
	string s;
	
	//-----------------Capture & Prepare Image------------------//
	// Load the image file and check for success
	
	// Capture from camera
	VideoCapture capture(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    if(!capture.isOpened()){
	    cout << "Failed to connect to the camera." << endl;
    }
	
	while(true){
		Mat image;
		//for (int i=0; i < 3; i++){capture >> image;} //Filter through Camera Buffer for a better image
		capture >> image;
		if(image.empty()){
			cout << "Failed to capture an image" << endl;
			return -1;
		}
        
		//Lane Detection ROI
		ROIx = 0;
		ROIy = (image.rows - 1)/8;
		width = image.cols - 1;
		height = (image.rows - 1)/8;
        
        Rect roi(ROIx,ROIy,width,height);// set the ROI for the image
		Mat imgROI = image(roi);
		
        //Color Detection ROI
        COLORx = (image.cols - 1)/3;
        COLORy = 0;
        COLORwidth = (image.cols - 1)/3;
        COLORheight = (image.rows - 1);
        
        Rect Croi(COLORx,COLORy,COLORwidth,COLORheight);// set the ROI for the image
		Mat COLORROI = image(Croi);
		
		// HSV algorithm
		Mat imgHSVFilter, gray;
		cvtColor(imgROI, gray, CV_BGR2GRAY);
		cvtColor(imgROI, imgHSVFilter, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		inRange(imgHSVFilter, Scalar(0, 0, 190), Scalar(180, 255, 255), imgHSVFilter); //Threshold the image
		
		// Canny algorithm
		Mat contours;
		Canny(imgHSVFilter,contours,50,150);
		Mat contoursInv;
		threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);

		//---------------Probabilistic Hough Transform--------------//
		// Create LineFinder instance
		LineFinder ld;

		// Set probabilistic Hough parameters
		ld.setLineLengthAndGap(15,10);
		ld.setMinVote(15);
		
		ld.setShiftV(ROIy);
		ld.setShiftH(ROIx);
		
		// Detect lines
		vector<Vec4i> li= ld.findLines(contours);
		//-----------------Find Intersection Points-----------------//
		YmidAxis = ROIy;
		XmidPiont = width/2 + ROIx;

		if (firstrunFlag == 0){
			XLEFT_PREV = 0.0 + ROIx;
			XRIGHT_PREV = width + ROIx;
		}
		
		XLEFT = XLEFT_PREV;
		XRIGHT = XRIGHT_PREV;
		
		if (!li.empty()){
			slopemaxGapFinder(XLEFT,XRIGHT,YmidAxis,width, XmidPiont, ROIx, ROIy , li);
		}
		//cout << "Left X Point: " << XLEFT << " Right X Point: " << XRIGHT << " X Mid Point: " << XmidPiont << endl; 
		//---------------------Angle Calculation----------------------//
		Angle = 18 + (2*XmidPiont-XLEFT-XRIGHT)/22; // Main Turning Algorithm
		if (Angle < 0) { Angle = 0;}
		if (Angle > 36) { Angle = 36;}
		
		if (firstrunFlag == 0){
			prevAngle = Angle;
		}
		
		//Angle Buffer Logic
		if ((abs(prevAngle - Angle) > 12) && (countBuff < 20)) { // To Catch YUUUGE turning changes
			Angle = prevAngle;
			countBuff++;
			//cout << "Buff: " << countBuff << endl;
		} else {
			countBuff = 0;
		}
		
		/*
		if ((prevAngle <= 4) || (prevAngle >= 16*2)) {// Sticky turning for the tight corners
			cornerTRIGGER = 1;
			if(abs(prevAngle - Angle) > 9){
				if (prevAngle <= 4){
					Angle = 0;
				} else {
					Angle = 36;
				}
			}
		} else {
			cornerTRIGGER = 0;
		}*/
		
        //---------------------Color Detection----------------------//
        Mat imgHSV;
        cvtColor(COLORROI, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
        
		//RED
		Mat mask1, mask2;
        inRange(imgHSV, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1); //Threshold the image
        inRange(imgHSV, Scalar(163, 100, 100), Scalar(179, 255, 255), mask2);
        
        Mat imgThresholded = mask1 | mask2;
        
        pixleSUMRED = 0;
        
        for (int i=0; i<imgThresholded.rows; i++){
            for (int j=0; j<imgThresholded.cols; j++){
                if (imgThresholded.at<uchar>(i,j) == 255){
                    pixleSUMRED++;
                }
            }
        }
		
		//GREEN
        inRange(imgHSV, Scalar(45, 100, 100), Scalar(92, 255, 255), imgThresholded); //Threshold the image
		
		pixleSUMGREEN = 0;
        
        for (int i=0; i<imgThresholded.rows; i++){
            for (int j=0; j<imgThresholded.cols; j++){
                if (imgThresholded.at<uchar>(i,j) == 255){
                    pixleSUMGREEN++;
                }
            }
        }
		
		//Color Detection Logic
		if ((pixleSUMRED > 2000) && (colorSENDTRIGGER != 1)){
            colorBuffRED++;
			cout << "RED DETECTED" << endl;
        } else if ((pixleSUMGREEN > 2000) && (colorSENDTRIGGER != 1)){
			colorBuffGREEN++;
			cout << "GREEN DETECTED" << endl;
		} else {
            colorBuffRED = 0;
			colorBuffGREEN = 0;
            serialColor = 0;
			colorSENDbuff = 0;
			cout << "NO RED OR GREEN" << endl;
        }
		
        if (colorBuffRED > 1){
            serialColor = 1;
			colorSENDbuff++;
        }
        if (colorBuffGREEN > 1){
            serialColor = 2;
			colorSENDbuff++;
        }
		
		if (colorSENDbuff >= 2){
			colorSENDTRIGGER = 1;
		}
		//------------------Speed Calibration----------------------//
		/*
		if (Angle <= 4 && Angle > 16*2){
			speed = 1;
		} else if ( Angle > 16 && Angle < 21) {
			speed = 2;
		} else {
			speed = 1;
		}
        */
		/*
		if (abs(prevAngle - Angle) <= 1){// Constant Turning Angle ups speed
			speedCounter++;
		} else {
			speedCounter = 0;
		}
		
		if ((speedCounter > 4) && (cornerTRIGGER != 1)){ 
			speed = 2;
		} else if ((speedCounter > 4) && (cornerTRIGGER == 1) && (cornerSPEEDthresh < 4) ) {
			speed = 2;
			cornerSPEEDthresh++;
		} else {
			speed = 1;
			cornerSPEEDthresh = 0;
		}*/
		//------------------Serial Communication----------------------//
		serialib LS; //the main class to access
		//int Ret,Ret2;
		//char Buffer[128];
		
        s = to_string(speed) + " " + to_string(Angle) + " " + to_string(serialColor) + "#";
        char const *pchar = s.c_str();

		Ret= LS.Open(DEVICE_PORT,9600);
		if ((firstrunFlag == 0) || (((prevAngle != Angle) || (serialColor != 0) || (prevSpeed != speed)) && (colorSENDTRIGGER != 1))){ //Sends only useful info, no repeats
			cout << ":)" << endl;
			Ret2=LS.WriteString(pchar);	
		} else if (colorSENDTRIGGER == 1){
			Ret=LS.ReadString(Buffer,'\n',128,500); 
			cout << "READING" << endl;
			if (Ret>0){
				colorSENDTRIGGER = 0;
				colorBuffRED = 0;
				colorBuffGREEN = 0;
				serialColor = 0;
				colorSENDbuff = 0;
				cout << "GOTEM!!!!!!!" << endl;
				cout << ":)" << endl;
				Ret2=LS.WriteString(pchar);	
			}
		}
		prevSpeed = speed;
		prevAngle = Angle;
		//cout << Angle << endl;
		//cout << Speed << endl;		
		cout << s << endl;
		firstrunFlag = 1;
		LS.Close();
	}
	return 0;
}
