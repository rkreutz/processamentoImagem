#include "stdafx.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#include <random>

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
//Constantes
/*******************************************************************************/
#define szWidth 5
#define szHeight 29
#define deltaXGaussianBlur 55
#define deltaYGaussianBlur 1500
#define minHue 157
#define minSat 0
#define minVal 0
#define maxHue 179
#define maxSat 255
#define maxVal 255
#define maxPoints 10
#define Trans 80
#define noTrans 100
#define file_name "7mg17.jpg"
#define data_file "teste2-scan.txt"
/*******************************************************************************/

using namespace cv;
using namespace std;

//Variaveis Globais
/*******************************************************************************/
Mat M,m,mt,mtRGB,result;											//Matrizes contendo as imagens do programa.

int _szWidth, _szHeight, deltaX, deltaY;								//Variaveis para a funcao GaussianBlur
int _minHue, _minSat, _minVal, _maxHue, _maxSat, _maxVal;	//Variaveis para a identificacao de area de cor desejada

Point first,current;

Mat comp[maxPoints];										//Variaveis para o armazenamento dos pontos do clique esquerdo.
Range MatRow[maxPoints];
Range MatCol[maxPoints];
int clicks;

/*******************************************************************************/

void filter() {
	m = M.clone();
	//cvtColor(M,m,CV_BGR2HSV);
	Size siz(_szWidth,_szHeight);
	GaussianBlur(m,m,siz,(deltaX/100.0) ,(deltaY/100.0));
	cvtColor(m,m,CV_BGR2HSV);
	
}

void filter_old() {
	//_szWidht = _szHeight
	cvtColor(M,m,CV_BGR2HSV);
	Size siz(_szWidth,_szHeight);
	GaussianBlur(m,m,siz,(deltaX/100.0));
}

void proc() {
	inRange(m,Scalar(_minHue,_minSat,_minVal),Scalar(_maxHue,_maxSat,_maxVal),mt);
	cvtColor(mt,mtRGB,CV_GRAY2BGR);
}

void show(int x) {
	addWeighted(M,x/100.0,mtRGB,(100-x)/100.0,0.0,result);
	imshow("amostra",result);
}

void impares(int state, void* userdata){
	int *num = (int*) userdata;
	if (*num%2 == 0) *num += 1;
	filter();
	proc();
	show(Trans);
}

void centesimal(int state, void* userdata){
	filter();
	proc();
	show(Trans);
}

void transp(int state, void* userdata) {
	show(Trans);
}

void escalar(int state, void* userdata){
	proc();
	show(Trans);
}

bool allOnes(Mat VcMat[], int VcSize, int numElements) {
	bool res = true;
	for(int z = 0; z < VcSize; z++) {
		if(countNonZero(VcMat[z]) < numElements) {
			res = false;
			break;
		}
	}
	return res;
}

void ajuste(Mat VcMat[], int VcSize, Range MatRow[], Range MatCol[], int *properties[]){
	if (VcSize == 0) {
		cout << "VcSize = 0\n";
		return;
	}
	
	int u = 0;
	while(u < 3) {
		*properties[u] += 1;
		proc();
		for (int y = 0; y < VcSize; y++) {
			VcMat[y] = mt(MatRow[y],MatCol[y]);
		}
		if(!allOnes(VcMat,VcSize,75)) {
			*properties[u] -= 1;
			proc();
			for (int y = 0; y < VcSize; y++) {
				VcMat[y] = mt(MatRow[y],MatCol[y]);
			}
			u += 1;
		}
	}
	while (u < 6) {
		*properties[u] -= 1;
		proc();
		for (int y = 0; y < VcSize; y++) {
			VcMat[y] = mt(MatRow[y],MatCol[y]);
		}
		if(!allOnes(VcMat,VcSize,75)) {
			*properties[u] += 1;
			proc();
			for (int y = 0; y < VcSize; y++) {
				VcMat[y] = mt(MatRow[y],MatCol[y]);
			}
			u += 1;
		}
	}
	createTrackbar( "szWidth", "Configuracoes", &_szWidth, 55, impares,&_szWidth);
	createTrackbar( "szHeight", "Configuracoes", &_szHeight, 55, impares,&_szHeight);
	createTrackbar( "deltaX", "Configuracoes", &deltaX, 5000, centesimal,NULL);
	createTrackbar( "deltaY", "Configuracoes", &deltaY, 5000, centesimal,NULL);
	createTrackbar( "minHue", "Configuracoes", &_minHue, 255, escalar,NULL);
	createTrackbar( "maxHue", "Configuracoes", &_maxHue, 255, escalar,NULL);
	createTrackbar( "minSat", "Configuracoes", &_minSat, 255, escalar,NULL);
	createTrackbar( "maxSat", "Configuracoes", &_maxSat, 255, escalar,NULL);
	createTrackbar( "minVal", "Configuracoes", &_minVal, 255, escalar,NULL);
	createTrackbar( "maxVal", "Configuracoes", &_maxVal, 255, escalar,NULL); 
}

void change2(int event,int x, int y,int flag, void* userdata){
	if (flag == CV_EVENT_FLAG_CTRLKEY + CV_EVENT_FLAG_LBUTTON) {
		_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
		proc();
		mt = Mat::zeros(mt.rows,mt.cols,CV_8UC1);
		show(noTrans);
	}
	
	else if (event == CV_EVENT_LBUTTONDOWN) {
		
		//Point3_<uchar>* P = M.ptr<Point3_<uchar> >(y,x);
		//Point3_<uchar>* p = m.ptr<Point3_<uchar> >(y,x);
		cout << "Posicao: " << x << " x " << y << endl;
		//cout << int(P->z) << " x " << int(P->y) << " x " << int(P->x) << "    RxGxB\n"; //RxGxB
		//cout << int(p->x) << " x " << int(p->y) << " x " << int(p->z) << "    HxSxV\n"; //HxSxV
		if (clicks == 0) {
			first.x = x;
			first.y = y;
			current= first;
			clicks++;
		}
		else {
			cout << current.x << " x " << current.y << " -> " << x << " x " << y << endl;
			line(mt,current,Point(x,y),Scalar(255));
			current.x = x;
			current.y = y;
			clicks++;
		}

	}

	else if (event == CV_EVENT_RBUTTONDOWN) {
		/*int *properties[6];
		properties[0] = &_minHue; properties[1] = &_minSat; properties[2] = &_minVal; properties[3] = &_maxHue; properties[4] = &_maxSat; properties[5] = &_maxVal;

		ajuste(comp,clicks,MatRow,MatCol,properties);
		
		if (clicks != 0) {
			show(Trans);
			clicks = 0;
			imshow("Detection",mt);
		}
		else {
			show(noTrans);
			imshow("Detection",mt);
		}*/
		if (clicks > 1) {
			vector<vector<Point>> contours;
			cout << current.x << " x " << current.y << " -> " << first.x << " x " << first.y << endl;
			line(mt,current,first,Scalar(255));
			findContours(mt, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			drawContours(mt,contours,0,Scalar(255),-1);
			ofstream file;
			file.open(data_file,ios::app);
			file << file_name << endl << endl;
			cvtColor(mt,mtRGB,CV_GRAY2BGR);
			show(Trans);
			Scalar mean,stdd;
			Mat m_gray;
			m.release();		//added
			m = M.clone();		//added
			cvtColor(m,m_gray,CV_BGR2GRAY);
			cvtColor(m,m,CV_BGR2HSV);
			meanStdDev(m,mean,stdd,mt);
			default_random_engine gen;
			normal_distribution<double> H(mean[0],mean[0]/10);
			normal_distribution<double> S(mean[1],stdd[1]);
			normal_distribution<double> V(mean[2],stdd[2]);
			Mat norm_dist(100,100,CV_8UC3);
			for (int y = 0; y < 100 ; y++)
				for (int x = 0 ; x < 100 ; x++) {
					Point3_<uchar>* P = norm_dist.ptr<Point3_<uchar> >(y,x);
					P->x = H(gen);
					//P->x = mean[0];
					P->y = S(gen);
					//P->y = mean[1];
					P->z = V(gen);
					//P->z = mean[2];
				};
			Size siz(7,7);
			GaussianBlur(norm_dist,norm_dist,siz,3 ,3);
			//cvtColor(norm_dist,norm_dist,CV_HSV2BGR);
			imshow("RESULTADO",norm_dist);
			waitKey();
			//file << "Busca Manual:\n";
			cout << "Busca Manual:\n";
			//file << "HSV:\n";
			cout << "HSV:\n";
			//file << mean << endl << stdd << endl << endl;
			cout << mean << endl << stdd << endl << endl;
			//file << "Gray:\n";
			cout << "Gray:\n";
			meanStdDev(m_gray,mean,stdd,mt);
			//file << mean << endl << stdd << endl << endl;
			cout << mean << endl << stdd << endl << endl;
			clicks = 0;
		}
	}

	else if (event == CV_EVENT_MBUTTONDOWN) {
		int *properties[6];
		double match = 0.7,temp;
		vector<vector<Point>> best;
		vector<vector<Point>> contours;
		vector<Point> rect;
		rect.push_back(Point(0,0));
		rect.push_back(Point(10,0));
		rect.push_back(Point(10,10));
		rect.push_back(Point(0,10));
		rect.push_back(Point(0,0));
		properties[0] = &_minHue; properties[1] = &_minSat; properties[2] = &_minVal; properties[3] = &_maxHue; properties[4] = &_maxSat; properties[5] = &_maxVal;

		for( int ROWS = 0 ; ROWS < mt.rows - 15; ROWS+=1) {
			for( int COLS = 0 ; COLS < mt.cols - 5; COLS+=1) {
				_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
				proc();
				MatRow[0] = Range(ROWS,ROWS+15);
				MatCol[0] = Range(COLS,COLS+5);
				ajuste(comp,1,MatRow,MatCol,properties);
				imshow("Detection",mt);
				cout << COLS << " x " << ROWS << endl;

				Mat mt_show = mt.clone();
				//Hierarchies (contours inside contours)
				vector<Vec4i> hierarchy;

				// "Each contour is stored as a vector of points."
		
				findContours(mt_show, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
				cout << "Found " << contours.size() << " contours." << endl;
				if(contours.size() == 1 && pointPolygonTest(contours[0],Point(COLS,ROWS),false) >= 0) {
					approxPolyDP(contours[0],contours[0],3,true);
					double area = contourArea(contours[0]);
					temp = matchShapes(rect,contours[0],CV_CONTOURS_MATCH_I2,0);
					cout << temp << endl;
					/*mt = Mat::zeros(mt.rows,mt.cols,CV_8UC1);
					drawContours(mt,contours,0,Scalar(255),-1);
					imshow("Detection",mt);
					cvtColor(mt,mtRGB,CV_GRAY2BGR);
					show(Trans);*/
					if(match > temp && area > 100 && temp != 0 && area < 400) {
						match = temp;
						best.clear();
						best.push_back(contours[0]);
						//if( !best.empty() ) cout << "printed" << endl;
					}
					//waitKey();
				}
			}
		}

		ofstream file;
		file.open(data_file,ios::app);
		file << file_name << endl << endl;

		if( !best.empty() ) {
					
			mt = Mat::zeros(mt.rows,mt.cols,CV_8UC1);
			drawContours(mt,best,0,Scalar(255),-1);
			imshow("Detection",mt);
			cvtColor(mt,mtRGB,CV_GRAY2BGR);
			Scalar mean,stdd;
			Mat m_gray;
			//m.release();		//added
			//m = M.clone();		//addded
			cvtColor(m,m_gray,CV_BGR2GRAY);
			/* Para gerar numeros randomicos com distribuicao normal*/
			meanStdDev(m,mean,stdd,mt);
			default_random_engine gen;
			normal_distribution<double> H(mean[0],mean[0]);
			normal_distribution<double> S(mean[1],stdd[1]);
			normal_distribution<double> V(mean[2],stdd[2]);
			Mat norm_dist(100,100,CV_8UC3);
			for (int y = 0; y < 100 ; y++)
				for (int x = 0 ; x < 100 ; x++) {
					Point3_<uchar>* P = norm_dist.ptr<Point3_<uchar> >(y,x);
					P->x = H(gen);
					P->y = S(gen);
					P->z = V(gen);
				};
			cvtColor(norm_dist,norm_dist,CV_HSV2BGR);
			imshow("RESULTADO",norm_dist);
			waitKey();

			file << "Auto-busca:\n";
			file << "HSV:\n";
			file << mean << endl << stdd << endl << endl;
			file << "Gray:\n";
			meanStdDev(m_gray,mean,stdd,mt);
			file << mean << endl << stdd << endl << endl;
			//cout << mean << endl << stdd << endl << contourArea(best[0]);
			show(Trans);
		}

		else
			file << "No contour found\n\n";
	}
}

void detection(int event,int x, int y,int flag, void* userdata){
	if (event == CV_EVENT_LBUTTONDOWN) {
		Mat mt_show(mt);
		//Hierarchies (contours inside contours)
		vector<Vec4i> hierarchy;

		// "Each contour is stored as a vector of points."
		vector<vector<Point> > contours;
		
		findContours(mt_show, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		drawContours(mt_show,contours,-1,Scalar(255),1,8,hierarchy);
		imshow("Detection",mt_show);

		double comparar;
		comparar = matchShapes(contours[0],contours[1],CV_CONTOURS_MATCH_I1,0);
		cout << "Contours " << (1.0-comparar)*100.0 << "% match." << endl;

		// Print number of found contours.
		cout << "Found " << contours.size() << " contours." << endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	M = imread(file_name,CV_LOAD_IMAGE_UNCHANGED);

	if(M.empty()) {
		cout << "Imagem nao foi carregada\n";
		system("PAUSE");
		return -1;
	}
	//resize(M,M,Size(0,0),0.5,0.5);
	_szWidth = szWidth; _szHeight = szHeight; deltaX = deltaXGaussianBlur; deltaY = deltaYGaussianBlur;
	_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
	clicks = 0;

	namedWindow("amostra",CV_WINDOW_FREERATIO);
	namedWindow("Configuracoes",CV_WINDOW_FREERATIO);
	setMouseCallback("amostra",change2,NULL);
	cvCreateButton("aa");
	filter();
	proc();
	show(noTrans);

	imshow("Detection",mt);
	setMouseCallback("Detection",detection,NULL);

	createTrackbar( "szWidth", "Configuracoes", &_szWidth, 55, impares,&_szWidth);
	createTrackbar( "szHeight", "Configuracoes", &_szHeight, 55, impares,&_szHeight);
	createTrackbar( "deltaX", "Configuracoes", &deltaX, 5000, centesimal,NULL);
	createTrackbar( "deltaY", "Configuracoes", &deltaY, 5000, centesimal,NULL);
	createTrackbar( "minHue", "Configuracoes", &_minHue, 255, escalar,NULL);
	createTrackbar( "maxHue", "Configuracoes", &_maxHue, 255, escalar,NULL);
	createTrackbar( "minSat", "Configuracoes", &_minSat, 255, escalar,NULL);
	createTrackbar( "maxSat", "Configuracoes", &_maxSat, 255, escalar,NULL);
	createTrackbar( "minVal", "Configuracoes", &_minVal, 255, escalar,NULL);
	createTrackbar( "maxVal", "Configuracoes", &_maxVal, 255, escalar,NULL);

	//cout << mt.rows << " x " << mt.cols << endl;

	mt = Mat::zeros(mt.rows,mt.cols,CV_8UC1);

	while(waitKey(30) != ' ') {
		
	}

	destroyAllWindows();
	return 0;
}

