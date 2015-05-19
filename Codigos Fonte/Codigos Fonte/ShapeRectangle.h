#pragma once

#include "ShapeSimple.h"

class ShapeRectangle : public ShapeSimple
{
private:
	bool horizontal;
	cv::Point center;
	cv::Rect outerRect;
public:
	ShapeRectangle(void);
	ShapeRectangle(cv::Point p1, cv::Point p2, bool _horizontal = true);
	ShapeRectangle(ShapeRectangle &src);
	~ShapeRectangle(void);

	ShapeRectangle& operator=(ShapeRectangle &src);
	friend std::ostream& operator<< (std::ostream& out,const ShapeRectangle &src);
};

