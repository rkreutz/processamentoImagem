#pragma once
#ifndef SHAPESIMPLE_H
#define SHAPESIMPLE_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"

#define TEXT_FONT 5
#define TEXT_SCALE 2.3
#define TEXT_THICKNESS 2
#define LETTER_HEIGHT 35
#define LETTER_WIDTH 34
#define LETTER_DIST_BORDER 7

class ShapeSimple
{
private:
	cv::Rect control, test;
	cv::Scalar bordersColor;
	int bordersSize;
	cv::Mat borders;
	cv::Point C,T;
public:
	ShapeSimple &operator= (ShapeSimple &src);
	friend std::ostream& operator<< (std::ostream& out,const ShapeSimple &src);
	bool printBorders(cv::Mat &image, cv::Mat &dst);
	void deleteRegions();
	//_cr = control region; _tr = test region;
	bool createRegions(std::vector<cv::Point> _cr, std::vector<cv::Point> _tr);
	bool createRegions(std::vector<cv::Point> region);
	bool prepareImage(const cv::Mat &image,bool withText = true);
	void setBordersAspect(cv::Scalar color,int size);
	inline const cv::Scalar getBorderColor() {return bordersColor;};
	inline const int getBorderSize() {return bordersSize;};
	inline const cv::Rect getTestRegion() {return test;};
	inline const cv::Rect getControlRegion() {return control;};
	inline const cv::Mat getBordersImage() {return borders;};
	void swapRegions();
	cv::Point meanPoint(std::vector<cv::Point> point);
	inline bool isEmpty() {return (control.area() == 0 || test.area() == 0);};

	//Constructors and Destructor
	ShapeSimple(void);
	//_cr = control region; _tr = test region;
	ShapeSimple(std::vector<cv::Point> _cr, std::vector<cv::Point> _tr);
	ShapeSimple(const ShapeSimple &src);
	virtual ~ShapeSimple(void);
};

#endif //SHAPESIMPLE_H