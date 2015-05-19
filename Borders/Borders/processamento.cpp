#include "stdafx.h"
#include "processamento.h"

void filter_strip(Mat &src, Mat &dst, int szWidth, int szHeight, int deltaX, int deltaY, bool silent) {
	Size siz(szWidth,szHeight);
	GaussianBlur(src,dst,siz,(deltaX/100.0),(deltaY/100.0));
	if (!silent)
		cout << "Function " << var2str(filter_strip()) << ": " << var2str(GaussianBlur())
			 << var2str(szWidth) << " = " << szWidth << endl
			 << var2str(szHeight) << " = " << szHeight << endl
			 << var2str(deltaX/100) << " = " << deltaX << "/100" << endl
			 <<	var2str(deltaY/100) << " = " << deltaY << "/100" << endl;
	return;
}

void filter_square(Mat &src, Mat &dst, int size, int delta, bool silent) {
	Size siz(size,size);
	GaussianBlur(src,dst,siz,delta/100.0);
	if (!silent)
		cout << "Function " << var2str(filter_square()) << ": " << var2str(GaussianBlur())
			 << var2str(size) << " = " << size << endl
			 << var2str(delta/100) << " = " << delta << "/100" << endl;
	return;
}

bool region_adaptiveThreshold(Mat &src, Mat &dst, int block_sz, int C, bool silent) {
	if (src.type() != CV_8UC1) {
		dst = Mat::zeros(src.rows,src.cols,src.type());
		if(!silent)
			cout << "Function " << var2str(region_adaptiveThreshold()) << ": \"" << var2str(src) << "\" should be of type " << var2str(CV_8UC1) << ".\n";
		return false;
	}
	
	Mat temp;
	bool crescent = true;
	int var = C,maxC = 0, minC = 0;
	vector<vector<Point>> contours;

	adaptiveThreshold(src,temp,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,C/100);
	findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

	zero:
		// If there is no regions you have to go back, because your threshold is too high.
		if (contours.size() == 0) {				
			crescent = false;			//We're saying that we're going back to lower thresholds.
			if (var == 0) {				//If we are at our minimal threshold there is no regions to find.
				dst = Mat::zeros(src.rows,src.cols,src.type());
				if(!silent)
					cout << "Function " << var2str(region_adaptiveThreshold()) << ": minimal threshold but there is no regions (Nothing detected)\n";
				return false;
			}
			var--;						//Decreasing threshold.
			adaptiveThreshold(src,temp,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,var/100);
			contours.clear();
			findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
			goto zero;
		}
		//If there are more than one regions, we might have a threshold too high or to low.
		else if (contours.size() > 1) {
			//If the function is ascending, we increase the threshold. Otherwise we decreased it.
			if (crescent)
				var++;
			else
				var--;
			adaptiveThreshold(src,temp,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,var/100);
			contours.clear();
			findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
			goto zero;
		}
		//In case we have only one region, we have to analyse what to do.
		else if (contours.size() == 1) {
			//If we were decreasing and we found one region, we have our maximum threshold.
			if (!crescent) {
				maxC = var;				//Maximum threshold
				//Now we can find our minimal threshold. We keep decreasing the threshold until we have regions different than 1
				while(contours.size() == 1) {
					var--;
					adaptiveThreshold(src,temp,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,var/100);
					contours.clear();
					findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
				}
				minC = var++;			//Minimum threshold
			}
			//If we are increasing the threshold and we find one region, we might be at the start of the program or at our minimal threshold
			//So to be sure we increase the threshold until we loose this region and then we do the inverse process.
			else {
				while(contours.size() == 1) {
					var++;
					adaptiveThreshold(src,temp,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,var/100);
					contours.clear();
					findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
				}
				crescent = false;			//We have to specify that we are going backwards now.
				goto zero;
			}
		}
		//If we have negative regions, something is wrong.
		else {
			dst = Mat::zeros(src.rows,src.cols,src.type());
			cout << "Function " << var2str(region_adaptiveThreshold()) << ": negative regions, something went wrong.\n";
			return false;
		}

	double area, best = 0, match, bestMatch = 10;
	//Rectangle to be compared with our shapes.
	vector<Point> rect;
	rect.push_back(Point(0,0));
	rect.push_back(Point(0,15));
	rect.push_back(Point(5,15));
	rect.push_back(Point(5,0));
	rect.push_back(Point(0,0));

	//Goes through all our possible thresholds.
	for(int x = minC; x <= maxC; x++) {
		adaptiveThreshold(src,temp,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,x/100);
		contours.clear();
		findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		area = contourArea(contours[0]);								//Area of the contour
		match = matchShapes(contours[0],rect,CV_CONTOURS_MATCH_I2,0);	//Match Shape of the contour with the rectangle.
		if(area > best && match < bestMatch && match != 0) {			//Pick the best threshold, by form and size.
			best = area;
			bestMatch = match;
			var = x;
		}
	}
	//If there isn't any thresholds that were good enough, the function fails.
	if (best == 0) {
		dst = Mat::zeros(src.rows,src.cols,src.type());
		if (!silent)
			cout << "Function " << var2str(region_adaptiveThreshold()) << ": no regions were good enough.\n";
		return false;
	}

	adaptiveThreshold(src,dst,WHITE_GRAY,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,block_sz,var/100);
	if (!silent)
		cout << "Function " << var2str(region_adaptiveThreshold()) << ": succesfull.\n";
	return true;
}