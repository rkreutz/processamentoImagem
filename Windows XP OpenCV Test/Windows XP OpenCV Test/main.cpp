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
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
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



//Global Variables
/*******************************************************************************/

/*******************************************************************************/


using namespace std;
using namespace cv;

int _tmain (int argc, char **argv) {
	vector<string> devices = VideoCapture::listDevices();
	for (unsigned x = 0; x < devices.size(); x++)
		cout << x << " - " << devices[x] << endl;
	namedWindow("amostra",CV_GUI_EXPANDED);
	int e = 0;
	createTrackbar("amostra:","amostra",&e,5);
	if ( windowOpen("amostra") == 0) cout << "No window.\n";
	else cout << "Window open.\n";
	waitKey();
	destroyAllWindows();
	if( windowOpen("amostra") == 0) cout << "No window.\n";
	else cout << "Window opne.\n";
	system("PAUSE");
	return 0;
}