/*******************************************************************************
Author: Rodrigo Kreutz
Date:
Project:
Description:
********************************************************************************/

#include "stdafx.h"
#include "utils.h"
#include "license.h"
#include "language.h"
#include "opencv.h"
#include "ShapeSimple.h"
#include "ShapeRectangle.h"
#include "callbacks.h"
#include "OCVMenu.h"
//#include "opencv2\highgui\highgui_c.h"

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/
#define LANGUAGE "EN"
#define LIC 54727544
#define PROGRAM_WINDOW "Analisador de Testes Rapidos - Versao Beta"
#define PICTURES_TAKEN 7
#define ENTER 13
#define ESC 27
/*******************************************************************************/

//Error Constants
/*******************************************************************************/

/*******************************************************************************/



//Global Variables
/*******************************************************************************/
map<int,string> texts;
string error_log;
bool notification;
/*******************************************************************************/

void analizadorCliente();

int _tmain (int argc, char **argv) {
	notification = true;
	if(!enableKey(LIC,OS_64BIT)) {
		cout << "Problema com a licensa do programa.\n";
		system("PAUSE");
		exit(-1);
	}
	OCVMenu Menu;
	namedWindow(PROGRAM_WINDOW);
	/*
	/******         Menu layout         *****************************
	Menu.setBoxColor(Scalar(0,0,255));
	Menu.setActiveColor(Scalar(0,255,0));
	Menu.setLetterColor(Scalar(0,0,255));
	Mat back = Mat::zeros(600,800,CV_8UC3);
	Menu.loadBackground(back);
	Menu.setWindowName(PROGRAM_WINDOW);
	/****************************************************************

	Menu.addOption("Analizar amostras",Point(100,100),&analizadorCliente);
	Menu.addOption("Tutoriais",Point(200,200));
	imshow(PROGRAM_WINDOW,Menu.displayMenu());

	while(windowOpen(PROGRAM_WINDOW) && waitKey(1000) != ESC) {
		setMouseCallback(PROGRAM_WINDOW,buttonActivation,&Menu);
	}*/

	Mat img(600,800,CV_8UC3,Scalar(175,40,170));
	Mat overlay(img);
	overlay = Scalar(0,0,0);

	destroyAllWindows();
	return 0;
}

void analizadorCliente() {
	VideoCapture camera;
	int camNum=0;

	vector<string> devices = VideoCapture::listDevices();

	if(devices.size() == 0) {
		errorHandler(var2str(VideoCapture::listCameras()),"Nao ha nenhuma camera ligada ao computador.");
		exit(-1);
	}
	/*
	for (unsigned x = 0; x < devices.size(); x++)
		cout << x << " - " << devices[x] << endl;
	
	cout << "\nEscolha uma camera:\n> ";
	cin >> camNum;
	*/



	camera.open(camNum);

	if(!camera.isOpened()) {
		errorHandler(var2str(VideoCapture::listCameras()),"Camera escolhida nao existe.");
		exit(-1);
	}

	Mat frame, shapeMask, auxiliar, show;
	char command;
	ShapeSimple shape;
	vector<Point> region;
	Drawing draw;
	double Ax,Bx;

	if(!camera.read(frame)) {
		errorHandler(var2str(VideoCapture::read()),"Falha ao ler a imagem da camera.");
		exit(-1);
	}

	shape.setBordersAspect(Scalar(-255,255,-255),3);
	shape.prepareImage(frame);
	shapeMask = shape.getBordersImage().clone();
	auxiliar = shapeMask.clone();

	draw.shape = &shape;
	draw.region = &region;
	draw.image = &shapeMask;
	draw.auxiliar = &auxiliar;

	namedWindow(PROGRAM_WINDOW,CV_GUI_NORMAL);	

	while((command = waitKey(30)) != ESC && windowOpen(PROGRAM_WINDOW)) {
		if(!camera.read(frame)) {
			errorHandler(var2str(VideoCapture::read()),"Falha ao ler a imagem da camera.");
			exit(-1);
		}
		shape.printBorders(frame,show);
		imshow(PROGRAM_WINDOW,show);

		if(shape.isEmpty()) {
			//displayOverlay(PROGRAM_WINDOW,"Nao ha contornos a serem avaliados.\nClick esquerdo - indica um ponto do contorno.\nClick direito - finaliza o contorno e conecta ao inicio.",100);
			setMouseCallback(PROGRAM_WINDOW,drawingRegion,(void*)&draw);
			add(frame,shapeMask,show);
			imshow(PROGRAM_WINDOW,show);
		}
		else {
			setMouseCallback(PROGRAM_WINDOW,NULL);
			shape.prepareImage(frame);
		}

		if(command == 'd') {
			shape.deleteRegions();
			shape.prepareImage(frame);
			shapeMask = shape.getBordersImage().clone();
			auxiliar = shapeMask.clone();
		}

		if(command == ' ') {
			if(!shape.isEmpty()) {
				vector<Mat> control, test, pictures;

				for(int x = 0 ; x < PICTURES_TAKEN ; x++) {
					pictures.push_back(frame.clone());
					Sleep(20);
					if ( !camera.read(frame) ) {
						errorHandler(var2str(VideoCapture::read()),"Falha ao ler a imagem da camera.");
						exit(-1);
					}
				}

				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					control.push_back(pictures[x].colRange(shape.getControlRegion().x,shape.getControlRegion().br().x));
					control[x] = control[x].rowRange(shape.getControlRegion().y,shape.getControlRegion().br().y);
					test.push_back(pictures[x].colRange(shape.getTestRegion().x,shape.getTestRegion().br().x));
					test[x] = test[x].rowRange(shape.getTestRegion().y,shape.getTestRegion().br().y);
				}

				if ( (control.empty() || test.empty()) && (control.size() < PICTURES_TAKEN || test.size() < PICTURES_TAKEN) ) {
					errorHandler(var2str(command()),"Imagens carregadas da camera estao vazias ou ha menos do que deveria.");
					exit(-1);
				}

				Mat binC, binT;
				binC = 255*Mat::zeros(control[0].rows,control[0].cols,CV_8UC1);
				binT = 255*Mat::zeros(test[0].rows,test[0].cols,CV_8UC1);
				vector<Point> match;
				match.push_back(Point(0,0));
				match.push_back(Point(0,15));
				match.push_back(Point(5,15));
				match.push_back(Point(5,0));
				match.push_back(Point(0,0));

				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					Mat tempColor, tempGray, tempReg;
					filter_strip(control[x],tempColor);
					cvtColor(tempColor,tempGray,CV_BGR2GRAY);
					region_adaptiveThreshold(tempGray,tempReg,match);
					bitwise_or(tempReg,binC,binC);
					filter_strip(test[x],tempColor);
					cvtColor(tempColor,tempGray,CV_BGR2GRAY);
					region_adaptiveThreshold(tempGray,tempReg,match);
					bitwise_or(tempReg,binT,binT);
				}
				
				vector<double> rel,mean_cv;
				for (int x = 0; x < PICTURES_TAKEN; x++)
					rel.push_back(relation(control[x],binC,test[x],binT));
				mean_cv = meanAndCv(rel);
				if(mean_cv[1] < (double)4) 
					cout << Ax*mean_cv[0]+Bx << endl;
				else
					cout << "O teste nao foi bem sucedido, tire uma nova foto.\n";
				
				rel.clear();
				mean_cv.clear();
				control.clear();
				test.clear();
				pictures.clear();
			}
		}

		if(command == 'p') {
			vector<ShapeSimple> padroes;
			vector<Point> reg;
			ShapeSimple temp;
			Drawing draw_p;
			Mat mask, aux, pronto;
			
			temp.setBordersAspect(Scalar(-255,255,-255),3);
			temp.prepareImage(frame);
			mask = temp.getBordersImage().clone();
			aux = mask.clone();
			pronto = Mat::zeros(frame.rows,frame.cols,frame.type());

			draw_p.shape = &temp;
			draw_p.region = &reg;
			draw_p.image = &mask;
			draw_p.auxiliar = &aux;

			setMouseCallback(PROGRAM_WINDOW,drawingRegion,(void*)&draw_p);

			while(waitKey(30) != ENTER) {
				//displayOverlay(PROGRAM_WINDOW,"Pressione enter para definir as regioes do padrao.",200);
				if(!camera.read(frame)) {
					errorHandler(var2str(VideoCapture::read()),"Falha ao ler a imagem da camera.");
					exit(-1);
				}

				if(temp.isEmpty()) {
					add(frame,mask,show);
					imshow(PROGRAM_WINDOW,show);
				}
				else {
					padroes.push_back(temp);
					temp.prepareImage(frame,false);
					bitwise_or(pronto,temp.getBordersImage().clone(),pronto);
					mask = pronto.clone();
					aux = mask.clone();
					temp.deleteRegions();
					imshow(PROGRAM_WINDOW,show);
				}
			}

			setMouseCallback(PROGRAM_WINDOW,NULL);

			temp.deleteRegions();
			mask.release();
			aux.release();
			reg.clear();

			if(padroes.size() < 1) {
				errorHandler(var2str(main::calibration()),"Nenhuma regiao de analise foi especificada para a calibracao.");
				exit(-1);
			}

			while(waitKey(30) != ' ') {
				//displayOverlay(PROGRAM_WINDOW,"Posicione o padrao e pressione 'SPACEBAR' para realizar a calibracao.",200);
				if(!camera.read(frame)) {
					errorHandler(var2str(VideoCapture::read()),"Falha ao ler a imagem da camera.");
					exit(-1);
				}
			}

			vector<Mat> pictures;
			vector<double> concentrations;

			for(int x = 0 ; x < PICTURES_TAKEN ; x++) {
				pictures.push_back(frame.clone());
				Sleep(20);
				if ( !camera.read(frame) ) {
					errorHandler(var2str(VideoCapture::read()),"Falha ao ler a imagem da camera.");
					exit(-1);
				}
			}

			vector<Point> match;
			match.push_back(Point(0,0));
			match.push_back(Point(0,15));
			match.push_back(Point(5,15));
			match.push_back(Point(5,0));
			match.push_back(Point(0,0));

			for (unsigned y = 0; y < padroes.size(); y++) {
				vector<Mat> control,test;
				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					control.push_back(pictures[x].colRange(padroes[y].getControlRegion().x,padroes[y].getControlRegion().br().x));
					control[x] = control[x].rowRange(padroes[y].getControlRegion().y,padroes[y].getControlRegion().br().y);
					test.push_back(pictures[x].colRange(padroes[y].getTestRegion().x,padroes[y].getTestRegion().br().x));
					test[x] = test[x].rowRange(padroes[y].getTestRegion().y,padroes[y].getTestRegion().br().y);
				}

				Mat binC, binT;
				binC = 255*Mat::zeros(control[0].rows,control[0].cols,CV_8UC1);
				binT = 255*Mat::zeros(test[0].rows,test[0].cols,CV_8UC1);
				
				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					Mat tempColor, tempGray, tempReg;
					filter_strip(control[x],tempColor);
					cvtColor(tempColor,tempGray,CV_BGR2GRAY);
					region_adaptiveThreshold(tempGray,tempReg,match);
					bitwise_or(tempReg,binC,binC);
					filter_strip(test[x],tempColor);
					cvtColor(tempColor,tempGray,CV_BGR2GRAY);
					region_adaptiveThreshold(tempGray,tempReg,match);
					bitwise_or(tempReg,binT,binT);
				}

				vector<double> rel, mean_cv;
				for (int x = 0; x < PICTURES_TAKEN; x++) 
					rel.push_back(relation(control[x],binC,test[x],binT));
				mean_cv = meanAndCv(rel);
				concentrations.push_back(mean_cv[0]);
				
				mean_cv.clear();
				rel.clear();
				control.clear();
				test.clear();
			}
			
			pictures.clear();

			vector<double> prediction;
			system("cls");
			cout << "Digite o valor do que deve ser cada padrao na mesma ordem em que suas regioes foram criadas.\n Total de " << concentrations.size() << " numeros devem ser fornecidos.\n";
			for(unsigned x = 0; x < concentrations.size(); x++) {
				double val;
				cout << "> ";
				cin >> val;
				prediction.push_back(val);
			}

			system("cls");
			vector<double> curve;
			curve = calibrationCurve(concentrations,prediction);
			
			if(curve[2] > (double)0.98) {
				Ax = curve[0];
				Bx = curve[1];
				cout << "Calibracao foi bem sucedida.\n";
			}
			else if(curve[2] < (double)0.94) {
				Ax = 0;
				Bx = 0;
				cout << "Calibracao foi insatisfatoria, refaca a calibracao.\n";
			}
			else {
				Ax = curve[0];
				Bx = curve[1];
				cout << "Calibracao foi bem sucedida, porem nao esta ideal. Recomendado refaze-la.\n";
			}
		}

	}
}