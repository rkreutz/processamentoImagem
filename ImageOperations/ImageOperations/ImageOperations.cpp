// ImageOperations.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\imgproc\types_c.h>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat imagem = cv::imread("imagem2.jpg"), disp;
	cv::namedWindow("Options", 0);
	vector<cv::Mat> comp;
	int R = 1, G = 1, B = 1, sizeR = 1, sizeG = 1, sizeB = 1, sigma = 0;
	cv::createTrackbar("R", "Options", &R, 255);
	cv::createTrackbar("G", "Options", &G, 255);
	cv::createTrackbar("B", "Options", &B, 255);
	cv::createTrackbar("SizeR", "Options", &sizeR, 100);
	cv::createTrackbar("SizeG", "Options", &sizeG, 100);
	cv::createTrackbar("SizeB", "Options", &sizeB, 100);
	cv::createTrackbar("Sigma", "Options", &sigma, 15);

	while (cv::waitKey(30) != ' ') {
		//cv::cvtColor(imagem, disp, CV_BGR2YUV);
		cv::split(imagem, comp);
		if (sizeR % 2 == 0) sizeR++;
		if (sizeG % 2 == 0) sizeG++;
		if (sizeB % 2 == 0) sizeB++;
		cv::threshold(comp[0], comp[0],B, 255, 2);
		cv::threshold(comp[1], comp[1], G, 255, 2);
		cv::threshold(comp[2], comp[2], R, 255, 2);
		cv::GaussianBlur(comp[0], comp[0], cv::Size(sizeB, sizeB), sigma);
		cv::GaussianBlur(comp[1], comp[1], cv::Size(sizeG, sizeG), sigma);
		cv::GaussianBlur(comp[2], comp[2], cv::Size(sizeR, sizeR), sigma);
		cv::merge(comp, disp);

		//cv::cvtColor(disp, disp, CV_YUV2BGR);
		cv::imshow("Imagem", disp);
	}

	return 0;
}

