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

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/

/*******************************************************************************/

//Error Constants
/*******************************************************************************/

/*******************************************************************************/



//Global Variables
/*******************************************************************************/
int row;
int szHeight, szWidth;
int deltaX, deltaY;
Mat dst;
Mat src;
/*******************************************************************************/

Mat x_axis_histogram_transformation(Mat src);

void impares(int state, void* userdata){
	int *num = (int*) userdata;
	if (*num%2 == 0) *num += 1; 
	/// Separate the image in 3 places ( B, G and R )
	 dst = src.clone();
  system("cls");
  vector<Mat> HSV_planes;
  cvtColor(dst,dst,CV_BGR2HSV);

  GaussianBlur(dst,dst,Size(szWidth,szHeight),deltaX,deltaY);
  dst = dst.row(row);

  split( dst, HSV_planes );

  /// Establish the number of bins
  int histSize = src.cols+2;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, src.cols+2  } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = true;
  Mat H_hist, S_hist, V_hist;
  HSV_planes[0] = x_axis_histogram_transformation(HSV_planes[0].clone());
  HSV_planes[1] = x_axis_histogram_transformation(HSV_planes[1].clone());
  HSV_planes[2] = x_axis_histogram_transformation(HSV_planes[2].clone());

  /// Compute the histograms:
  calcHist( &HSV_planes[0], 1, 0, Mat(), H_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &HSV_planes[1], 1, 0, Mat(), S_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &HSV_planes[2], 1, 0, Mat(), V_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = src.cols; int hist_h = src.rows;
  int bin_w = cvRound( (double) hist_w/(histSize-2) );

  Mat histImageH( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageS( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageV( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
  normalize(H_hist, H_hist, 0, histImageH.rows, NORM_MINMAX, -1, Mat() );
  normalize(S_hist, S_hist, 0, histImageS.rows, NORM_MINMAX, -1, Mat() );
  normalize(V_hist, V_hist, 0, histImageV.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 0; i < histSize-2; i++ )
  {
      line( histImageH, Point( bin_w*(i), hist_h - cvRound(H_hist.at<float>(i)) ) ,
					   Point( bin_w*(i+1), hist_h - cvRound(H_hist.at<float>(i+1)) ),
		               Scalar( 0, 255, 0), 1, 1, 0  );
	 
      line( histImageS, Point( bin_w*(i), hist_h - cvRound(S_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(S_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
      line( histImageV, Point( bin_w*(i), hist_h - cvRound(V_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(V_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
  }

  /// Display

  addWeighted(src,1,histImageH,0.7,0,histImageH);
  addWeighted(src,1,histImageS,0.7,0,histImageS);
  addWeighted(src,1,histImageV,0.7,0,histImageV);
  imshow("H", histImageH );
  imshow("S", histImageS );
  imshow("V", histImageV );
}

void centesimal(int state, void* userdata){
	  /// Separate the image in 3 places ( B, G and R )
	 dst = src.clone();
	 system("cls");
  
  vector<Mat> HSV_planes;
  cvtColor(dst,dst,CV_BGR2HSV);

  GaussianBlur(dst,dst,Size(szWidth,szHeight),deltaX,deltaY);
  dst = dst.row(row);

  split( dst, HSV_planes );

  /// Establish the number of bins
  int histSize = src.cols+2;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, src.cols+2  } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = true;
  Mat H_hist, S_hist, V_hist;
  HSV_planes[0] = x_axis_histogram_transformation(HSV_planes[0].clone());
  HSV_planes[1] = x_axis_histogram_transformation(HSV_planes[1].clone());
  HSV_planes[2] = x_axis_histogram_transformation(HSV_planes[2].clone());

  /// Compute the histograms:
  calcHist( &HSV_planes[0], 1, 0, Mat(), H_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &HSV_planes[1], 1, 0, Mat(), S_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &HSV_planes[2], 1, 0, Mat(), V_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = src.cols; int hist_h = src.rows;
  int bin_w = cvRound( (double) hist_w/(histSize-2) );

  Mat histImageH( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageS( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageV( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
  normalize(H_hist, H_hist, 0, histImageH.rows, NORM_MINMAX, -1, Mat() );
  normalize(S_hist, S_hist, 0, histImageS.rows, NORM_MINMAX, -1, Mat() );
  normalize(V_hist, V_hist, 0, histImageV.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 0; i < histSize-2; i++ )
  {
      line( histImageH, Point( bin_w*(i), hist_h - cvRound(H_hist.at<float>(i)) ) ,
					   Point( bin_w*(i+1), hist_h - cvRound(H_hist.at<float>(i+1)) ),
		               Scalar( 0, 255, 0), 1, 1, 0  );
	 
      line( histImageS, Point( bin_w*(i), hist_h - cvRound(S_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(S_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
      line( histImageV, Point( bin_w*(i), hist_h - cvRound(V_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(V_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
  }

  /// Display

  addWeighted(src,1,histImageH,0.7,0,histImageH);
  addWeighted(src,1,histImageS,0.7,0,histImageS);
  addWeighted(src,1,histImageV,0.7,0,histImageV);
  imshow("H", histImageH );
  imshow("S", histImageS );
  imshow("V", histImageV );
}

void normal(int state, void* userdata) {
	  /// Separate the image in 3 places ( B, G and R )
	dst = src.clone();
  system("cls");
  vector<Mat> HSV_planes;
  cvtColor(dst,dst,CV_BGR2HSV);

  GaussianBlur(dst,dst,Size(szWidth,szHeight),deltaX,deltaY);
  dst = dst.row(row);

  split( dst, HSV_planes );

  /// Establish the number of bins
  int histSize = src.cols+2;

  /// Set the ranges ( for B,G,R) )
  float range[] = { 0, src.cols+2  } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = true;
  Mat H_hist, S_hist, V_hist;
  HSV_planes[0] = x_axis_histogram_transformation(HSV_planes[0].clone());
  HSV_planes[1] = x_axis_histogram_transformation(HSV_planes[1].clone());
  HSV_planes[2] = x_axis_histogram_transformation(HSV_planes[2].clone());

  /// Compute the histograms:
  calcHist( &HSV_planes[0], 1, 0, Mat(), H_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &HSV_planes[1], 1, 0, Mat(), S_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &HSV_planes[2], 1, 0, Mat(), V_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = src.cols; int hist_h = src.rows;
  int bin_w = cvRound( (double) hist_w/(histSize-2) );

  Mat histImageH( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageS( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageV( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
  normalize(H_hist, H_hist, 0, histImageH.rows, NORM_MINMAX, -1, Mat() );
  normalize(S_hist, S_hist, 0, histImageS.rows, NORM_MINMAX, -1, Mat() );
  normalize(V_hist, V_hist, 0, histImageV.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 0; i < histSize-2; i++ )
  {
      line( histImageH, Point( bin_w*(i), hist_h - cvRound(H_hist.at<float>(i)) ) ,
					   Point( bin_w*(i+1), hist_h - cvRound(H_hist.at<float>(i+1)) ),
		               Scalar( 0, 255, 0), 1, 1, 0  );
	 
      line( histImageS, Point( bin_w*(i), hist_h - cvRound(S_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(S_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
      line( histImageV, Point( bin_w*(i), hist_h - cvRound(V_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(V_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
  }

  /// Display

  addWeighted(src,1,histImageH,0.7,0,histImageH);
  addWeighted(src,1,histImageS,0.7,0,histImageS);
  addWeighted(src,1,histImageV,0.7,0,histImageV);
  imshow("H", histImageH );
  imshow("S", histImageS );
  imshow("V", histImageV );
}

int _tmain (int argc, char **argv) {
	row = 0;
	szHeight = szWidth = 1;
	deltaX = deltaY = 0;
  /// Load image
  src = imread( "test7mg0.jpg" );

  if( !src.data )
    { return -1; }
  namedWindow("H", CV_WINDOW_AUTOSIZE );
  namedWindow("S", CV_WINDOW_AUTOSIZE );
  namedWindow("V", CV_WINDOW_AUTOSIZE );
  namedWindow("Configuracoes", CV_WINDOW_AUTOSIZE);
	createTrackbar( "szWidth", "Configuracoes", &szWidth, 55, impares,&szWidth);
	createTrackbar( "szHeight", "Configuracoes", &szHeight, 55, impares,&szHeight);
	createTrackbar( "deltaX", "Configuracoes", &deltaX, 5000, centesimal,NULL);
	createTrackbar( "deltaY", "Configuracoes", &deltaY, 5000, centesimal,NULL);
	createTrackbar( "row", "Configuracoes", &row, src.rows - 1, normal, NULL);

  /// Separate the image in 3 places ( B, G and R )
	 /// Separate the image in 3 places ( B, G and R )
	 dst = src.clone();
  
  vector<Mat> HSV_planes;
  vector<Mat> Hue,Sat,Val;
  cvtColor(dst,dst,CV_BGR2HSV);
  Mat imagem;
  GaussianBlur(dst,dst,Size(szWidth,szHeight),deltaX,deltaY);
  for(;row<dst.rows;row++) {
  imagem = dst.row(row);

  split( imagem, HSV_planes );
  HSV_planes[0] = x_axis_histogram_transformation(HSV_planes[0].clone());
  HSV_planes[1] = x_axis_histogram_transformation(HSV_planes[1].clone());
  HSV_planes[2] = x_axis_histogram_transformation(HSV_planes[2].clone());
  Hue.push_back(HSV_planes[0]);
  Sat.push_back(HSV_planes[1]);
  Val.push_back(HSV_planes[2]);
  }

  /// Establish the number of bins
  int histSize = src.cols+2;
  
  /// Set the ranges ( for B,G,R) )
  float range[] = {0,src.cols+2};
  const float* histRange = { range };

  bool uniform = true; bool accumulate = true;
  Mat H_hist, S_hist, V_hist;
  HSV_planes[0] = x_axis_histogram_transformation(HSV_planes[0].clone());
  HSV_planes[1] = x_axis_histogram_transformation(HSV_planes[1].clone());
  HSV_planes[2] = x_axis_histogram_transformation(HSV_planes[2].clone());
  int chan[] = {0};
	  /// Compute the histograms:
  calcHist( &Hue[0], Hue.size(), chan, Mat(), H_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &Sat[0], Sat.size(), chan, Mat(), S_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &Val[0], Val.size(), chan, Mat(), V_hist, 1, &histSize, &histRange, uniform, accumulate );

  // Draw the histograms for B, G and R
  int hist_w = src.cols; int hist_h = src.rows;
  int bin_w = cvRound( (double) hist_w/(histSize-2) );

  Mat histImageH( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageS( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  Mat histImageV( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
  normalize(H_hist, H_hist, 0, histImageH.rows, NORM_MINMAX, -1, Mat() );
  normalize(S_hist, S_hist, 0, histImageS.rows, NORM_MINMAX, -1, Mat() );
  normalize(V_hist, V_hist, 0, histImageV.rows, NORM_MINMAX, -1, Mat() );

  /// Draw for each channel
  for( int i = 0; i < histSize-2; i++ )
  {
      line( histImageH, Point( bin_w*(i), hist_h - cvRound(H_hist.at<float>(i)) ) ,
					   Point( bin_w*(i+1), hist_h - cvRound(H_hist.at<float>(i+1)) ),
		               Scalar( 0, 255, 0), 1, 1, 0  );
	 
      line( histImageS, Point( bin_w*(i), hist_h - cvRound(S_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(S_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
      line( histImageV, Point( bin_w*(i), hist_h - cvRound(V_hist.at<float>(i)) ) ,
                       Point( bin_w*(i+1), hist_h - cvRound(V_hist.at<float>(i+1)) ),
                       Scalar( 0, 255, 0), 1, 1, 0  );
  }

  /// Display

  addWeighted(src,1,histImageH,0.7,0,histImageH);
  addWeighted(src,1,histImageS,0.7,0,histImageS);
  addWeighted(src,1,histImageV,0.7,0,histImageV);
  imshow("H", histImageH );
  imshow("S", histImageS );
  imshow("V", histImageV );
  split(src,HSV_planes);
  equalizeHist(HSV_planes[0],HSV_planes[0]);
  equalizeHist(HSV_planes[1],HSV_planes[1]);
  equalizeHist(HSV_planes[2],HSV_planes[2]);
  cv::merge(HSV_planes,imagem);
  cvtColor(imagem,imagem,CV_BGR2GRAY);
  threshold(imagem,imagem,60,255,CV_THRESH_BINARY_INV);
  imshow("Imagem Equalizada",imagem);


  while (waitKey(30) != ' ');

	return 0;
}

Mat x_axis_histogram_transformation(Mat src) {
	Mat ex = src.clone();
	int size_total = 0;
	for (int i = 0; i < ex.cols; i++) {
		uchar* P = ex.ptr<uchar>(0,i);
		size_total += *P;
	}
	int current = 0;
	int maxVal = 0, minVal = 255;
	Mat res(1,size_total+256,CV_8UC1);
	for (int i = 0; i < ex.cols ; i++) {
		uchar* P = ex.ptr<uchar>(0,i);
		//cout << (int)*P << endl;
		for (int j = 0 ; j < *P; j++) {
			uchar* k = res.ptr<uchar>(0,current);
			*k = i;
			current++;
		}
		if ((int) *P > maxVal) maxVal = (int) *P;
		else if ((int) *P < minVal) minVal = (int) *P;
	}
	cout << "Max Value: " << maxVal << endl << "Min Value: " << minVal << endl << endl;
	for(;current<size_total+1;current++) {
		uchar* k = res.ptr<uchar>(0,current);
		*k = ex.cols;
	}
	uchar* k = res.ptr<uchar>(0,current);
	*k = ex.cols+1;
	return res;
}