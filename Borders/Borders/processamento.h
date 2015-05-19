#ifndef OPENCV_LIB
#define OPENCV_LIB
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
#endif

#ifndef NAME_SPC
#define NAME_SPC

using namespace cv;
using namespace std;

#endif

#define WHITE_GRAY 255
#define var2str(x) #x

//Gaussian filter with shape of a rectangular strip.
void filter_strip(Mat &src, Mat &dst, int szWidth = 5, int szHeight = 29, int deltaX = 55, int deltaY = 1500, bool silent = true);

//Gaussian filter with shape of a square.
void filter_square(Mat &src, Mat &dst, int size = 5, int delta = 700, bool silent = true);

//Region identification using adaptiveThreshold() as base function.
bool region_adaptiveThreshold(Mat &src, Mat &dst, int block_sz = 41, int C = 1209, bool silent = true);

//Region identification using Canny() as base function.
bool region_Canny(Mat &src, Mat &dst, );