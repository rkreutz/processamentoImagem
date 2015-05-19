/*******************************************************************************
Author: Rodrigo Kreutz
Date:
Project:
Description:
********************************************************************************/

#include "stdafx.h"

//General Libraries
/*******************************************************************************/
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
/*******************************************************************************/

//OpenCV Libraries
/*******************************************************************************/
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
/*******************************************************************************/

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/
#define CAMERA_ID 1
#define CAMERA_WINDOW "Camera"
#define CONFIG_WINDOW "Config"
#define MAX_DRAWING_CIRCLE_RADIUS 200
#define MAX_DRAWING_RECT_BASE 200
#define MAX_DRAWING_RECT_HEIGHT 200
#define MAX_FONT_TYPE 7
#define MAX_FONT_SCALE 30
#define MAX_TEXT_OFFSET 200
#define MAX_LEFT_TEXT_OFFSET 50
#define OUTLINE_COLOR Scalar(0,255,0)
#define OUTLINE_THICKNESS 4
#define TEXT_THICKNESS 3
#define FILL_CIRCLE -1
#define WHITE_1C Scalar(255)
#define WHITE_3C Scalar(255,255,255)
#define ANGLE_SQUARE_INSIDE_SEMICIRCLE 26.565051
#define PI 3.141593
#define FULL_WHEIGHT 1.0
#define THREE_FOURTHS_WHEIGHT 0.75
#define HALF_WHEIGHT 0.5
#define ONE_FOURTH_WHEIGHT 0.25
#define NO_WHEIGHT 0
#define OUTPUT_CONTROL "control"
#define OUTPUT_TEST "test"
/*******************************************************************************/

//Error Constants
/*******************************************************************************/
#define ERR_NO_CAMERA 32
#define ERR_NO_FRAME 33
#define ERR_FAILED_SAVE_JPG 34
/*******************************************************************************/



//Global Variables
/*******************************************************************************/

/*******************************************************************************/

int _tmain (int argc, char **argv) {
	VideoCapture camera;
	Mat frame, drawing, image;
	char command;
	int radius=0;		//radius of the outline circle.
	int base=0,height=0;//square dimensions.
	int offsetX=0;		//offset for the X component for the center of the outline circle.
	int offsetY=0;		//offset for the Y component for the center of the outline circle.
	int text_offsetX=0;	//offset for the X component for the text.
	int text_offsetY=0; //offset for the Y component for the text.
	int fontFace=1;		//Font type (Maximum 7)
	int scale=1;		//Font scale (will be divided by 10)
	int left_text=0;	//offset to adjust X component for the left text.
	int times = 0;		//times that the camera frame initiliazition failed.
	int timesPhoto = 0;	//times phot has been taken.
	
	camera.open(CAMERA_ID);

	//If there is no camera, points an error.
	if( !camera.isOpened() ) {
		cout << "Couldn't find camera ID " << CAMERA_ID << endl;
		cout << "Exit( " << ERR_NO_CAMERA << " )\n";
		return (ERR_NO_CAMERA);
	}

	namedWindow(CAMERA_WINDOW);
	namedWindow(CONFIG_WINDOW,CV_WINDOW_FREERATIO);
	
	//If the frame has failed to be load.
	loop:
	if( !camera.read(frame) ) {
		cout << "Not able to read frame from camera\n";
		cout << "Exit( " << ERR_NO_FRAME << " )\n";
		if (times < 5) {
			times++;
			goto loop;
		}
		return (ERR_NO_FRAME);
	}
	times = 0;

	
	//Trackbars for adjusting the drawing.
	//createTrackbar("Radius",CONFIG_WINDOW,&radius,MAX_DRAWING_CIRCLE_RADIUS);
	createTrackbar("Base",CONFIG_WINDOW,&base,MAX_DRAWING_RECT_BASE);
	createTrackbar("Height",CONFIG_WINDOW,&height,MAX_DRAWING_RECT_HEIGHT);
	createTrackbar("offsetX",CONFIG_WINDOW,&offsetX,frame.cols);
	createTrackbar("offsety",CONFIG_WINDOW,&offsetY,frame.rows);
	createTrackbar("Text X",CONFIG_WINDOW,&text_offsetX,MAX_TEXT_OFFSET);
	createTrackbar("Text Y",CONFIG_WINDOW,&text_offsetY,MAX_TEXT_OFFSET);
	createTrackbar("left_text",CONFIG_WINDOW,&left_text,MAX_TEXT_OFFSET);
	createTrackbar("fontFace",CONFIG_WINDOW,&fontFace,MAX_FONT_TYPE);
	createTrackbar("scale",CONFIG_WINDOW,&scale,MAX_FONT_SCALE);

	//Keeps refreshing frame and adding it to the drawing until ESC is pressed.
	while ( (command = waitKey(30)) != 27 ) {

		//If the frame has failed to be load.
		if( !camera.read(frame) ) {
			cout << "Not able to read frame from camera\n";
			cout << "Exit( " << ERR_NO_FRAME << " )\n";
			if (times < 5) {
				times++;
				goto loop;
			}
			return (ERR_NO_FRAME);
		}
		times = 0;

		//Initialize drawing to all black.
		drawing = Mat::zeros(frame.rows,frame.cols,CV_8UC3);

		//Draws a circle in drawing.
		//circle(drawing,Point(offsetX,offsetY),radius,OUTLINE_COLOR,OUTLINE_THICKNESS);

		//Draws a rectangle in drawing.
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2),Point(offsetX+base/2,offsetY+height/2),Scalar(0,255,0),OUTLINE_THICKNESS);

		//Draws the vertical middle line of the circle.
		line(drawing,Point(offsetX,(offsetY-height/2)),Point(offsetX,(offsetY+height/2)),OUTLINE_COLOR,OUTLINE_THICKNESS);

		/*//Write text.
		putText(drawing,"C",Point((offsetX-radius-text_offsetX-left_text),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+radius+text_offsetX),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		*/

		putText(drawing,"C",Point((offsetX-base/2-text_offsetX-left_text),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+base/2+text_offsetX),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		

		//Add the actual frame with the drawing, forming a new image.
		addWeighted(frame,FULL_WHEIGHT,drawing,THREE_FOURTHS_WHEIGHT,NO_WHEIGHT,image);

		//Exposes the new image.
		imshow("Camera",image);

		//To take a picture just press spacebar.
		if(command == ' ') {
			Mat control;		//Image with the control.
			Mat test;			//Image with the test.
			/*int sin265,cos265;	//sin and cos of the angle of 26,5 degrees. See constants for exact value.

			//We calculate the biggest SQUARE inside each semi-circle, which has height equals to 2*sin(26,5) and width of cos(26,5).
			sin265 = radius*sin(ANGLE_SQUARE_INSIDE_SEMICIRCLE * PI/180);
			cos265 = radius*cos(ANGLE_SQUARE_INSIDE_SEMICIRCLE * PI/180);

			//We estipulate the new ranges according to our previous calculations.
			control = frame.colRange(offsetX-cos265,offsetX);
			control = control.rowRange(offsetY-sin265,offsetY+sin265);
			test = frame.colRange(offsetX,offsetX+cos265);
			test = test.rowRange(offsetY-sin265,offsetY+sin265);

			control = frame.colRange(offsetX-radius,offsetX);
			control = control.rowRange(offsetY-radius,offsetY+radius);
			test = frame.colRange(offsetX,offsetX+radius);
			test = test.rowRange(offsetY-radius,offsetY+radius);*/

			control = frame.colRange(offsetX-base/2,offsetX);
			control = control.rowRange(offsetY-height/2,offsetY+height/2);
			test = frame.colRange(offsetX,offsetX+base/2);
			test = test.rowRange(offsetY-height/2,offsetY+height/2);

			//Save our result.
			vector<int> parameters;
			parameters.push_back(IMWRITE_JPEG_QUALITY);
			parameters.push_back(100);
			stringstream str1,str2;
			str1 << OUTPUT_CONTROL << timesPhoto << ".jpg";
			if ( !imwrite(str1.str(),control,parameters) ) {
				cout << "Failed to save control image.";
				cout << "Exit( " << ERR_FAILED_SAVE_JPG << " )\n";
				return (ERR_FAILED_SAVE_JPG);
			}
			
			str2 << OUTPUT_TEST << timesPhoto << ".jpg";
			if ( !imwrite(str2.str(),test,parameters) ) {
				cout << "Failed to save test image.";
				cout << "Exit( " << ERR_FAILED_SAVE_JPG << " )\n";
				return (ERR_FAILED_SAVE_JPG);
			}

			timesPhoto++;
			parameters.clear();

		}
	}

	return 0;
}