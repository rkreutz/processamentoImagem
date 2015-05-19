#ifndef OPENCV_H
#define OPENCV_H

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


#define WHITE_GRAY 255

//Gaussian filter with shape of a rectangular strip.
void filter_strip(cv::Mat &src, cv::Mat &dst, int szWidth = 5, int szHeight = 29, int deltaX = 55, int deltaY = 1500);

//Gaussian filter with shape of a square.
void filter_square(cv::Mat &src, cv::Mat &dst, int size = 5, int delta = 700);

//Region identification using adaptiveThreshold() as base function. Image must be in grayscale
bool region_adaptiveThreshold(cv::Mat &src, cv::Mat &dst, std::vector<cv::Point> matchingShape, int block_sz = 41, int C = 1209);

//Region identification using Canny() as base function. Image must be in grayscale
bool region_Canny(cv::Mat &src, cv::Mat &dst, int threshold1 = 17, int threshold2 = 20);

//Region identification customized. It is based on the HSV values variation. 'values' order: min->Hue,Sat,Val;max->Hue,Sat,Val
//Image must be in HSV form.
bool region_custom(cv::Mat &src, cv::Mat &dst, int values[], const int rowBusca = 15, const int colBusca = 5, double approximation = 3);

//Returns the number that relates to the different concentrations of the control and test. This will give our final concentration.
//This returns only the 'x' value, it is needed to apply the right curve to this values to determine the final concentration.
double relation(cv::Mat &control, cv::Mat &controlReg, cv::Mat &test, cv::Mat &testReg);

//Takes a vector of values and returns the mean value of those values and the coefficient of variance of those values.
//'mean' = [0]; 'cv' = [1];
std::vector<double> meanAndCv(std::vector<double> values);

//Returns the equation of the calibration curve and the correlation of the curve.
//A = [0]; B = [1]; r2 = [2];
std::vector<double> calibrationCurve(std::vector<double> sample, std::vector<double> prediction);

//Goes through all cameras available inside. Returns number of cameras.
int search_cameras(const int maxCameras = 10);

//Saves the image on the computer. Default is .jpg format with 100% quality.
//Can also be 'imageFormat' = "png" and 'quality' = 0 to 9
bool photo(cv::Mat img, std::string name, std::string imageFormat = "jpg", int quality = 100);



#endif /*OPENCV_H*/