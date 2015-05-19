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

#include "ShapeSimple.h"

using namespace std;
using namespace cv;

int points;
Point first, last;
Mat img,clone;
ShapeSimple shape;
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

void callback(int event,int x, int y,int flag, void* userdata) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		if (points == 0) {
			first.x = x;
			first.y = y;
			last = first;
			points++;
		}
		else {
			line(img,last,Point(x,y),Scalar(255));
			last.x = x;
			last.y = y;
			points++;
			imshow("Image",img);
		}
	}

	else if(event == CV_EVENT_MOUSEMOVE) {
		if(points > 0) {
			clone = img.clone();
			line(clone,last,Point(x,y),Scalar(255));
			imshow("Image",clone);
		}
	}

	else if (event == CV_EVENT_RBUTTONDOWN) {
		if (points > 1) {
			line(img,last,first,Scalar(255));

			findContours(img.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			double match;
			if (contours.size() > 1) {
				//match = matchShapes(contours[0],contours[1],CV_CONTOURS_MATCH_I3,0);

				//cout << match << endl;
				Mat show;
				img = Mat::zeros(img.rows,img.cols,img.type());
				shape.deleteRegions();
				shape.createRegions(contours[1],contours[0]);
				shape.prepareImage(img);
				if(!shape.printBorders(show,show))
					cout << "Something wen't wrong\n";
				for(int j = 3; j < 10; j++) {
					shape.setBordersAspect(Scalar(255),j,0.5);
					shape.prepareImage(img);
					//shape.printBorders(img,show);
					imshow("Image",shape.getBordersImage());
					waitKey();
				}

			}
			cout << "Found " << contours.size() << " contours\n";

			//drawContours(img,contours,-1,Scalar(255));

			imshow("Image",img);
			points = 0;
		}
	}
}

int main()
{
	shape.setBordersAspect(Scalar(255),3,0.5);
	img = Mat::zeros(400,800,CV_8UC1);
	points = 0;
	
	namedWindow("Image");
	//circle( img, Point(200,100), 50, Scalar(255),1);
	imshow("Image",img);
	setMouseCallback("Image",callback);

	char c = 0;

	while ((c = waitKey(30)) != ' ') {
		if (c == 'c') {
			img = Mat::zeros(400,800,CV_8UC1);
			//circle( img, Point(200,100), 50, Scalar(255),1);
			imshow("Image",img);
		}
	}

    return 0;
}
