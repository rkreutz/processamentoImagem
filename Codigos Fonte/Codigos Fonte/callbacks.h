#ifndef	CALLBACKS_H
#define CALLBACKS_H

#include "opencv.h"
#include "ShapeSimple.h"

#ifndef MOUSE_CALLBACKS
#define MOUSE_CALLBACKS

//Struct for the arguments of the userdata for drawingRegion()
struct drawingRegionData {
	ShapeSimple *shape;
	cv::Mat *image, *auxiliar;
	std::vector<cv::Point> *region;
}typedef Drawing;

//Draws the regions on the image so you can decide whats the region to be analysed.
void drawingRegion(int event,int x, int y,int flag, void* userdata);

#endif	/*MOUSE_CALLBACKS*/

#ifndef TRACKBAR_CALLBACKS
#define TRACKBAR_CALLBACKS



#endif // !TRACKBAR_CALLBACKS

#endif	/*CALLBACKS_H*/