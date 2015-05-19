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
/*******************************************************************************/

using namespace cv;
using namespace std;

Mat display;
Mat original;
CvMemStorage *storage;
CvSeq* contour1;  //hold the pointer to a contour
int sens;

void on_trackbar(int state, void* userdata){
	NULL;
}

void on_mouse(int event,int x, int y,int flag, void* userdata){
	CvSeq* contour2 = contour1;  //hold the pointer to a contour
	CvSeq* result;   //hold sequence of points of a contour
	Mat disp(original);
	vector<int> param;
	if (event == CV_EVENT_LBUTTONDOWN) {
		 while(contour2)
		{
			//obtain a sequence of points of the countour, pointed by the variable 'countour'
			result = cvApproxPoly(contour2, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour2)*(sens/1000.0), 0);
           
			//if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
			if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>100 && fabs(cvContourArea(result, CV_WHOLE_SEQ))<90000)
			{
				//iterating through each point
				CvPoint *pt[4];
				int x1=999999,x2=0,y1=999999,y2=0;
				for(int i=0;i<4;i++){
					pt[i] = (CvPoint*)cvGetSeqElem(result, i);
					if(pt[i]->x < x1) x1 = pt[i]->x;
					if(pt[i]->x > x2) x2 = pt[i]->x;
					if(pt[i]->y < y1) y1 = pt[i]->y;
					if(pt[i]->y > y2) y2 = pt[i]->y;
				}

				if(x > x1 && x < x2 && y > y1 && y < y2) {
					namedWindow("FOTO");
					disp = disp.rowRange(y1,y2);
					disp = disp.colRange(x1,x2);
					imshow("FOTO",disp);
					waitKey();
					destroyWindow("FOTO");
					param.push_back(IMWRITE_JPEG_QUALITY);
					param.push_back(100);
					if(imwrite("Placa.jpg",disp,param) == false) {
						cout << "Erro ao tentar salvar a imagem.\n";
						system("PAUSE");
						exit(-1);
					}
					cout << "Imagem tirada com sucesso.\n";
				}
				
			}

			//obtain the next contour
			contour2 = contour2->h_next; 
		}
	}
}

int main()
{
  Mat frame;
  sens = 30;
  VideoCapture cap(1);
  CvSeq* contour;
  CvSeq* result;   //hold sequence of points of a contour
  cvNamedWindow("Filmagem");
  //cvNamedWindow("Original");
  //cvNamedWindow("GrayScale Image");
  //cvNamedWindow("Thresholded Image");
  createTrackbar( "Sensiblity", "Filmagem", &sens, 100, on_trackbar,NULL);
  setMouseCallback("Filmagem",on_mouse,NULL);
  loop:
  cap.read(frame);
  display = frame.clone();
  original = display.clone();
  IplImage* disp = &display.operator IplImage();
	
  
  
 //show the original image
 
 //cvShowImage("Original",img);

 //smooth the original image using Gaussian kernel to remove noise
  Size siz(5,29);
	GaussianBlur(frame,frame,siz,(55/100.0),(1500/100.0));
	IplImage* img = &frame.operator IplImage();
  //converting the original image into grayscale
 IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1); 
 cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

  
 //cvShowImage("GrayScale Image",imgGrayScale);

  //thresholding the grayscale image to get better results
 //cvThreshold(imgGrayScale,imgGrayScale,200,255,CV_THRESH_BINARY);
 cvAdaptiveThreshold(imgGrayScale,imgGrayScale,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,41,1.043);

  
 //cvShowImage("Thresholded Image",imgGrayScale);

 storage = cvCreateMemStorage(0); //storage area for all contours
 
 //finding all contours in the image
 cvFindContours(imgGrayScale, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
  
 //iterating through each contour
 contour1 = contour;
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
	 result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*(sens/1000.0), 0);
           
  //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
  if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>100 && fabs(cvContourArea(result, CV_WHOLE_SEQ))<90000)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   
   //drawing lines around the triangle
   cvLine(disp, *pt[0], *pt[1], cvScalar(0,0,255),2);
   cvLine(disp, *pt[1], *pt[2], cvScalar(0,0,255),2);
   cvLine(disp, *pt[2], *pt[3], cvScalar(0,0,255),2);
   cvLine(disp, *pt[3], *pt[0], cvScalar(0,0,255),2);
       
  }

  //obtain the next contour
  contour = contour->h_next; 
 }

  //show the image in which identified shapes are marked   
 
 cvShowImage("Filmagem",disp);
 cvShowImage("Threshold",imgGrayScale);
   
 if (cvWaitKey(30) != ' ') {
	 cvReleaseImage(&imgGrayScale);
	 cvReleaseMemStorage(&storage);
	 goto loop; //wait for a key press
 }

  //cleaning up
 cvDestroyAllWindows(); 
 cvReleaseMemStorage(&storage);
 //cvReleaseImage(&img);
 cvReleaseImage(&imgGrayScale);

  return 0;
}
