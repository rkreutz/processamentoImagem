#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"

class Histogram
{
private:
	int bin;
	float **xAxis;
	float yAxis[2];
	std::string histName;

public:
	Histogram(void);
	~Histogram(void);
};

