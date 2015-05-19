#include "stdafx.h"
#include "ShapeRectangle.h"


ShapeRectangle::ShapeRectangle(void)
{
	horizontal = true;
}

ShapeRectangle::ShapeRectangle(cv::Point p1, cv::Point p2, bool _horizontal)
{
	cv::Rect temp(p1,p2);
	outerRect = temp;
	horizontal = _horizontal;
	center = cv::Point((outerRect.tl().x + outerRect.br().x)/2,(outerRect.tl().y+outerRect.br().y)/2);
	
}

ShapeRectangle::ShapeRectangle(ShapeRectangle &src)
{
	horizontal = src.horizontal;
	center = src.center;
	outerRect = src.outerRect;
}

ShapeRectangle::~ShapeRectangle(void)
{
	
}

ShapeRectangle& ShapeRectangle::operator=(ShapeRectangle &src) {
	horizontal = src.horizontal;
	center = src.center;
	outerRect = src.outerRect;
	return src;
}

std::ostream& operator<< (std::ostream& out,const ShapeRectangle &src) {
	out << "\toperator<< for ShapeRectangle\t";
	return out;
}