/* ColorDetect.cpp
 * g++ `pkg-config --cflags --libs opencv` ColorTESTS_1.cpp -o ColorTESTS_1
 */
 
/* HSV Color Code:
    Color               Hue      Saturation    Value
    Red     ...     163 - 179    161 - 228   116 - 169
    Orange  ...     0 - 12       112 - 227   134 - 255
    Yellow  ...     22 - 34      38 - 244    160 - 255
    Green   ...     45 - 108     39 - 153    92 - 255
    Blue    ...     92 - 135     62 - 176    32 - 126
    Purple  ...     109 - 139    58 - 148    43 - 87
    **Note: Color May Vary depending on lighting!
*/

#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

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
    
    Mat image;
    capture >> image;
    if(image.empty()){
        cout << "Failed to capture an image" << endl;
        return -1;
    }
    /*
    int iLowH = 0;
	int iHighH = 179;

	int iLowS = 0; 
	int iHighS = 255;

	int iLowV = 0;
	int iHighV = 255;
    */
    Mat imgHSV;

	cvtColor(image, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	//Mat mask1, mask2;
	
    //inRange(imgHSV, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1); //Threshold the image
    //inRange(imgHSV, Scalar(163, 100, 100), Scalar(179, 255, 255), mask2);
	Mat imgThresholded;
	inRange(imgHSV, Scalar(45, 100, 100), Scalar(92, 255, 255), imgThresholded);
	
    
	
    
    int pixleSUM = 0;
    
    for (int i=0; i<imgThresholded.rows; i++){
        for (int j=0; j<imgThresholded.cols; j++){
            if (imgThresholded.at<uchar>(i,j) == 255){
                pixleSUM++;
            }
        }
    }
    
    cout << pixleSUM << endl;
    imwrite("DetectedColor.png", imgThresholded);
    imwrite("Origonal.png", image);
    
}
