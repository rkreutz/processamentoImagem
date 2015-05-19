#pragma once

#ifndef SHAPEMULTIPLE_H
#define SHAPEMULTIPLE_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"

class ShapeMultiple
{
	int borderSize;

public:
	ShapeMultiple(void);
	~ShapeMultiple(void);
};

#endif /*SHAPEMULTIPLE_H*/