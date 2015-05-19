// Cameras.cpp : Define o ponto de entrada para a aplicação de console.
//

#include "stdafx.h"

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
#include "opencv2/opencv.hpp"
#include "opencv2\opencv_modules.hpp"
/*******************************************************************************/

using namespace cv;
using namespace std;

void change(int state, void* userdata){
	NULL;
}

void change2(int event,int x, int y,int flag, void* userdata){
	if (event == CV_EVENT_LBUTTONDOWN)
		cout << "Posicao: " << x << " x " << y << endl;
}

void change3(int event,int x, int y,int flag, void* userdata){	
	if (event == CV_EVENT_LBUTTONUP)
		cout << "Posicao: " << x << " x " << y << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int value = 100;
	int value2 = 0;

	Mat M(100,100,CV_8UC3,Scalar(100,100,0));

	namedWindow("main1",CV_WINDOW_AUTOSIZE); 
	imshow("main1",M);

	createTrackbar( "track1", "main1", &value, 255, change,NULL);//OK tested 
	cvCreateButton("dummy",NULL,NULL,CV_RADIOBOX,false);
	cvCreateButton("dumm2y",NULL,NULL,CV_RADIOBOX,false);
	setMouseCallback("main1",change2,NULL);
	setMouseCallback("main1",change3,NULL);
	
	waitKey(0);
	destroyAllWindows(); 
	
	return 0;
}

