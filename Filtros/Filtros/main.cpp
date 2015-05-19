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

/*******************************************************************************/

//Error Constants
/*******************************************************************************/

/*******************************************************************************/

using namespace std;
using namespace cv;


//Global Variables
/*******************************************************************************/
int _szWidth,_szHeight,delta;
Mat m;
/*******************************************************************************/

void filter() {
	Size siz(_szWidth,_szHeight);
	GaussianBlur(m,m,siz,delta/100.0);
}

void impares(int state, void* userdata){
	int *num = (int*) userdata;
	if (*num%2 == 0) *num += 1;
	m = imread("control.jpg");
	filter();
}

void centesimal(int state, void* userdata){
	m = imread("control.jpg");
	filter();
}

int _tmain (int argc, char **argv) {
	m = imread("control.jpg");
	_szWidth = 1,_szHeight = 1,delta = 1;
	
	namedWindow("Configuracoes",CV_WINDOW_FREERATIO);
	namedWindow("Amostra");

	createTrackbar( "szWidth", "Configuracoes", &_szWidth, 55, impares,&_szWidth);
	createTrackbar( "szHeight", "Configuracoes", &_szHeight, 55, impares,&_szHeight);
	createTrackbar( "delta", "Configuracoes", &delta, 5000, centesimal,NULL);

	while(waitKey(50) != ' ') {
		imshow("Amostra",m);
	}

	return 0;
}