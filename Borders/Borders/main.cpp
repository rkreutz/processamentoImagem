/*******************************************************************************
Author: Rodrigo Kreutz
Date:
Project:
Description:
********************************************************************************/

#include "stdafx.h"

#include "processamento.h"

//General Libraries
/*******************************************************************************/
#include <iostream>
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

//Constants
/*******************************************************************************/
#define IMAGE "test2mg1.jpg"
#define WINDOW_NAME "Borders"
#define var2str(x) #x
#define WHITE_GRAY 255
/*******************************************************************************/

//Error Constants
/*******************************************************************************/

/*******************************************************************************/



//Global Variables
/*******************************************************************************/

/*******************************************************************************/


using namespace std;
using namespace cv;


int _tmain (int argc, char **argv) {
	Mat imag = imread(IMAGE);
	Mat corners,show,filtered;
	Mat kernel = -Mat::ones(3,3,CV_8UC1);
	vector<vector<Point>> contours;
	namedWindow(WINDOW_NAME);
	int T1=17,T2=20,T4=3,T5=11209;
	createTrackbar("Threshold 1",WINDOW_NAME,&T1,255);
	createTrackbar("Threshold 2",WINDOW_NAME,&T2,255);
	createTrackbar("Block Size",WINDOW_NAME,&T4,7);
	createTrackbar("C",WINDOW_NAME,&T5,20000);
	
	//Point3_<uchar>* P = M.ptr<Point3_<uchar> >(y,x);
	filter_strip(imag,filtered);
	cvtColor(filtered,filtered,CV_BGR2GRAY);
	Scalar white(255,255,255);
	//region_adaptiveThreshold(filtered,corners,41,1209,false);
	//cvtColor(corners,corners,CV_GRAY2BGR);
	copyMakeBorder(filtered,filtered,5,5,0,0,0,white);

	while(waitKey(30) != ' ') {		
		if(T4%2==0) T4++;
		if(T4==1) T4=3;
		
		//adaptiveThreshold(filtered,corners,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,T4,T5/100-100);
		Canny(filtered,corners,T1,T2,T4,true);
		findContours(corners,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		cout << contours.size() << endl;
		corners = Mat::zeros(corners.rows,corners.cols,corners.type());
		drawContours(corners,contours,0,Scalar(255,255,255),-1);
		cvtColor(corners,corners,CV_GRAY2BGR);
		corners = corners.rowRange(5,corners.rows-5);
		addWeighted(imag,1,corners,0.3,0,show);
		imshow(WINDOW_NAME,show);
	}
	return 0;
}