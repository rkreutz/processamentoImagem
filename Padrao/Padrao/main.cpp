/*******************************************************************************
Author: Rodrigo Kreutz
Date:
Project:
Description:
********************************************************************************/

#include "stdafx.h"

//General Libraries
/*******************************************************************************/
#include <iostream>
#include <cstdlib>
#include <random>
#include <cmath>
#include "windows.h"
/*******************************************************************************/

//OpenCV Libraries
/*******************************************************************************/
#include "opencv2/core/core.hpp"
//#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/photo/photo.hpp"
//#include "opencv2/video/video.hpp"
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/contrib/contrib.hpp"
//#include "opencv2/core/core_c.h"
//#include "opencv2/highgui/highgui_c.h"
//#include "opencv2/imgproc/imgproc_c.h"
/*******************************************************************************/

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/
#define Ax 9.105
#define Bx -3.5568
#define CAMERA_SEARCH false
#define CAMERA_ID 0
#define DRAWING_CONFIG_WINDOW "Referencia - Config"
#define PROGRAM_WINDOW "Programa"
#define TYPE_CIRCLE "CIRCLE"
#define TYPE_RECT "RECT"
#define TYPE_RECT2 "RECT2"
#define TYPE_PADRAO "PADRAO"
#define MAX_DRAWING_CIRCLE_RADIUS 400
#define MAX_DRAWING_RECT_BASE 400
#define MAX_DRAWING_RECT_HEIGHT 400
#define MAX_FONT_TYPE 7
#define MAX_FONT_SCALE 30
#define MAX_TEXT_OFFSET 200
#define MAX_LEFT_TEXT_OFFSET 50
#define OUTLINE_COLOR Scalar(0,255,0)
#define OUTLINE_THICKNESS 4
#define TEXT_THICKNESS 3
#define FULL_WHEIGHT 1.0
#define THREE_FOURTHS_WHEIGHT 0.75
#define HALF_WHEIGHT 0.5
#define ONE_FOURTH_WHEIGHT 0.25
#define NO_WHEIGHT 0
#define szWidth 5
#define szHeight 29
#define deltaXGaussianBlur 55
#define deltaYGaussianBlur 1500
#define minHue 90
#define minSat 0
#define minVal 0
#define maxHue 179
#define maxSat 255
#define maxVal 255
#define maxPoints 10
#define Trans 80
#define noTrans 100
#define AUTOBUSCA_ALTURA 15
#define AUTOBUSCA_LARGURA 5
#define PRECISAO_BUSCA 1*AUTOBUSCA_ALTURA*AUTOBUSCA_LARGURA
#define MINIMAL_MATCH 0.4
#define AUTOBUSCA_PULO 1
#define APPROXIMATION 3			//Quanto maior, mais a aproximacao se distancia do valor real
#define MIN_AREA 200			//Area minima da regiao de interesse
#define MAX_AREA 400				//Area maxima da regiao de interesse
#define UNIDADE "mg"
#define BASE 133
#define HEIGHT 33
#define OFFSETX 188
#define OFFSETY 321
#define OFFSETY_2RECT 50
#define OS_64BIT "x64"
#define OS_32BIT "x86"
#define GUID_LENGHT 50
/*******************************************************************************/

//Error Constants
/*******************************************************************************/
#define ERR_NO_CAMERA -32
#define ERR_LOAD_CAMERA -33
#define ERR_INITIAL_READ_FRAME -34
#define ERR_READ_FRAME -35
#define ERR_EMPTY_IMAGES -36
#define ERROR_OS_ARCHITECTURE_NOT_WELL_DEFINED -37
#define ERROR_CANT_FIND_REGISTRY -38
#define ERROR_CANT_QUERY_REGISTRY -39
/*******************************************************************************/



//Global Variables
/*******************************************************************************/

/////////////////////////////////////
//Variaveis para o desenho do modelo.
int radius;		//radius of the outline circle.
int base,height;//square dimensions.
int offsetX;		//offset for the X component for the center of the outline circle.
int offsetY;		//offset for the Y component for the center of the outline circle.
int text_offsetX;	//offset for the X component for the text.
int text_offsetY; //offset for the Y component for the text.
int fontFace;		//Font type (Maximum 7)
int scale;		//Font scale (will be divided by 10)
int left_text;	//offset to adjust X component for the left text.
int sizeCols;	//Tamanho maximo da figura, auxiliar de offsetX
int sizeRows;	//Tamanho maximo da figura, auxiliar de offsetY
string type;
/////////////////////////////////////
//Variaveis para processamento
int _szWidth, _szHeight, deltaX , deltaY;								//Variaveis para a funcao GaussianBlur
int _minHue, _minSat, _minVal, _maxHue, _maxSat, _maxVal;	//Variaveis para a identificacao de area de cor desejada
/////////////////////////////////////

//Variavel para identificacao da arquitetura do sistema operacional
string OSArch;

/*******************************************************************************/

//Retorna a camera escolhida pelo usuario.
int searchCam (VideoCapture *camera);

//Cria janela para configuracoes do modelo.
void createDrawWindow (string name , string type);

//Desenha o modelo na imagem.
Mat drawModel(string type, Mat image);

//Filtra a imagem usando Gaussian Blur para modelos de strip
void filter_strip(Mat &src, Mat &dst);

//Filtra a imagem usando Gaussian Blur para modelos de squares (ou circles)
void filter_square(Mat &src, Mat &dst);

//Processa a imagem com os atributos estabelecidos pelas variaveis globais, 'dst' tera sempre 1 canal
void proc(Mat &src, Mat &dst);

//Adiciona as duas imagens de acordo com a porcentagem indicada
Mat show(Mat src1, Mat src2, int percentage); 

//Verifica se nas imagens se existe 'numElements' igual a um.
//Quando qualquer uma das imagens nao estiver de acordo ele retorna 'false'.
bool allOnes(Mat VcMat[], int VcSize, int numElements);

//Faz o ajuste das propriedades de HSV para achar a menor area possivel equivalente.
//Funcao encarregada de encontrar a area de interesse.
void ajuste( int VcSize, Range MatRow[], Range MatCol[], int *properties[], Mat &src, Mat &dst);

//Faz o processamento da concentracao da imagem e tudo mais
vector<vector<Point>> processamento(Mat HSV, Mat bin);

//Faz o calculo necessario para concentracoes
double calculation(vector<vector<Point>> region, Mat image);


int _tmain (int argc, char **argv) {
	base = BASE;
	height = HEIGHT;
	offsetX = OFFSETX;
	offsetY = OFFSETY;
	OSArch = "x64";
	VideoCapture camera;
	Mat frame;
	int camID;
	char command;
	type = TYPE_PADRAO;

	//Caso a escolha de cameras esteja habilitada.
	if (CAMERA_SEARCH)
		camID = searchCam(&camera);
	else
		camID = CAMERA_ID;

	/* Para gerar numeros randomicos com distribuicao normal
	default_random_engine gen;
	normal_distribution<int> dist(mean,stdd);
	dist(gen);
	*/

	/* Retrieve MachineGuid
	HKEY key = 0;
	char buf[GUID_LENGHT];
	DWORD dwBufSize = sizeof(buf);
	const char* subkey = "Software\\Microsoft\\Cryptography";
	REGSAM desiredSam;

	//OS Architecture must be assigned to use proper flags.
	if(OSArch == OS_64BIT)
		desiredSam = KEY_WOW64_64KEY|KEY_READ;
	else if(OSArch == OS_32BIT)
		desiredSam = KEY_READ;
	else {

		cout << "Error " << ERROR_OS_ARCHITECTURE_NOT_WELL_DEFINED << endl;
		return ERROR_OS_ARCHITECTURE_NOT_WELL_DEFINED;

	}

	//Open the registry folder
	if( RegOpenKeyExA(HKEY_LOCAL_MACHINE,subkey,0,KEY_WOW64_64KEY|KEY_READ,&key) == ERROR_SUCCESS) {
		//Query the registry value
		if(RegQueryValueExA(key,"MachineGuid",0, 0, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS) {
			cout << "Registry GUID value: " << buf << endl;
			
			//Guid2lic() {
				unsigned char lic[GUID_LENGHT];
				int trueSize = 0;
				for (int j = 0 ; j < sizeof(buf) ; j++) {
					if (buf[j] >= 48 && buf[j] <= 57) {
						lic[trueSize] = buf[j] - 48;
						trueSize++;
					}	
					else if (buf[j] >= 97 && buf[j] <= 102) {
						lic[trueSize] = buf[j] - 87;
						trueSize++;
					}	
					else if (buf[j] >= 65 && buf[j] <= 70) {
						lic[trueSize] = buf[j] - 55;
						trueSize++;
					}	
				}
			//}

		}
		else {
			cout << "Error " << ERROR_CANT_QUERY_REGISTRY << endl;
			return ERROR_CANT_QUERY_REGISTRY;
		}

	}
	else {
		cout << "Error " << ERROR_CANT_FIND_REGISTRY;
		return ERROR_CANT_FIND_REGISTRY;
	}

	RegCloseKey(key);

	system("PAUSE");*/

	//Abre a camera selecionada.
	camera.open(camID);
	
	//Caso a camera nao consiga ser aberta corretamente retorna um erro.
	if( !camera.isOpened() ) {
		cout << "Error " << ERR_LOAD_CAMERA;
		destroyAllWindows();
		return (ERR_LOAD_CAMERA);
	}

	//Ler frame da camera
	if ( !camera.read(frame) ) {
		cout << "Error " << ERR_INITIAL_READ_FRAME;
		destroyAllWindows();
		return (ERR_INITIAL_READ_FRAME);
	}

	//Criando janela para configuracao do modelo.
	sizeCols = frame.cols;
	sizeRows = frame.rows;
	createDrawWindow(DRAWING_CONFIG_WINDOW,type);
	namedWindow(PROGRAM_WINDOW,CV_GUI_NORMAL);
		
	//Loop para mostrar a imagem
	while ( (command = waitKey(30)) != 27) {

		//Ler proximo frame.
		if ( !camera.read(frame) ) {
			cout << "Error " << ERR_READ_FRAME;
			destroyAllWindows();
			return (ERR_READ_FRAME);
		}

		//Junta p frame com o modelo e mostra o resultado.
		frame = drawModel(type, frame);
		imshow(PROGRAM_WINDOW,frame);	

		//Comando para processar a imagem
		if ( command == ' ') {
			Mat control,control1;
			Mat test,test1;
			vector<Mat> control_padrao, test_padrao;
			vector<vector<Mat>> all_control, all_test;
			vector<Mat> pictures;

			for(int i = 0; i < 7; i++) {
				pictures.push_back(frame.clone());
				Sleep(2);
				if ( !camera.read(frame) ) {
					cout << "Error " << ERR_READ_FRAME;
					destroyAllWindows();
					return (ERR_READ_FRAME);
				}
			}

			//Selecionando imagens com o controle e teste
			if(type == TYPE_CIRCLE) {
				control = frame.colRange(offsetX-radius,offsetX);
				control = control.rowRange(offsetY-radius,offsetY+radius);
				test = frame.colRange(offsetX,offsetX+radius);
				test = test.rowRange(offsetY-radius,offsetY+radius);
			}

			else if(type == TYPE_RECT) {
				control = frame.colRange(offsetX-base/2,offsetX);
				control = control.rowRange(offsetY-height/2,offsetY+height/2);
				test = frame.colRange(offsetX,offsetX+base/2);
				test = test.rowRange(offsetY-height/2,offsetY+height/2);

				/*control1 = frame.colRange(offsetX-base/2,offsetX);
				control1 = control1.rowRange(offsetY-height/2 - OFFSETY_2RECT,offsetY+height/2 - OFFSETY_2RECT);
				test1 = frame.colRange(offsetX,offsetX+base/2);
				test1 = test1.rowRange(offsetY-height/2 - OFFSETY_2RECT,offsetY+height/2 - OFFSETY_2RECT);*/
			}

			else if(type == TYPE_PADRAO) {
				for (int i = 0; i < 4 ; i++) {
					for (int j = 0; j < 7 ; j++) {
					control = pictures[j].colRange(offsetX-((1 - i%2)*base/2)+((i%2)*270),offsetX+((i%2)*270)+((i%2)*base/2));
					if (i < 2)
						control = control.rowRange(offsetY-height/2,offsetY+height/2);
					else
						control = control.rowRange(offsetY-height/2+60,offsetY+height/2+60);
					control_padrao.push_back(control.clone());
					test = pictures[j].colRange(offsetX-((i%2)*base/2)+((i%2)*270),offsetX+((1 - i%2)*base/2)+((i%2)*270));
					if (i < 2)
						test = test.rowRange(offsetY-height/2,offsetY+height/2);
					else
						test = test.rowRange(offsetY-height/2+60,offsetY+height/2+60);
					test_padrao.push_back(test.clone());
					}
					all_control.push_back(control_padrao);
					all_test.push_back(test_padrao);
					control_padrao.clear();
					test_padrao.clear();
				}
			}

			if ( all_test.empty() || all_control.empty() ) {
				cout << "Error " << ERR_EMPTY_IMAGES ;
				destroyAllWindows();
				return(ERR_EMPTY_IMAGES);
			}

			/********************************************************/
			vector<vector<double>> conc;
			vector<double> conc_part;
			Mat frame_binary = Mat::zeros(frame.rows,frame.cols,CV_8UC1);
			//Comeco do processamento
			for (int i = 0; i < 4; i++) {
			
			_szWidth = szWidth; _szHeight = szHeight; deltaX = deltaXGaussianBlur; deltaY = deltaYGaussianBlur;
			_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
			Mat test_HSV;
			Mat control_HSV;// = all_control[i][1].clone();
			
			Mat test_binary = Mat::zeros(all_test[i][0].rows,all_test[i][0].cols,CV_8UC1),control_binary = Mat::zeros(all_control[i][0].rows,all_control[i][0].cols,CV_8UC1);
			vector<vector<Point>> region_control, region_test;

			for(int j = 0; region_control.empty() ; j++) {
				if( j == 7) {
					cout << "Error: no region found";
					return -30;
				}
				control_HSV = all_control[i][j].clone();
				cvtColor(control_HSV,control_HSV,CV_BGR2HSV);
				filter_strip(control_HSV,control_HSV);
				region_control = processamento(control_HSV,control_binary);
			}

			for(int j = 0; region_test.empty() ; j++) {
				if( j == 7) {
					cout << "Error: no region found" << i;
					return -30;
				}
				test_HSV = all_test[i][j].clone();
				cvtColor(test_HSV,test_HSV,CV_BGR2HSV);
				filter_strip(test_HSV,test_HSV);
				region_test = processamento(test_HSV,test_binary);
			}

			if( !region_control.empty() && !region_test.empty()){ 
				for (int j = 0; j < 7; j++) {
				test_HSV = all_test[i][j].clone();
				control_HSV = all_control[i][j].clone();
				cvtColor(test_HSV,test_HSV,CV_BGR2HSV);
				cvtColor(control_HSV,control_HSV,CV_BGR2HSV);
				filter_strip(control_HSV,control_HSV);
				filter_strip(test_HSV,test_HSV);
				double concentration = calculation(region_test,test_HSV)/calculation(region_control,control_HSV);
				conc_part.push_back(concentration);
				}
				
				test_binary = control_binary = Mat::zeros(frame.rows,frame.cols,CV_8UC1);
				if ( i == 0 ) {
					drawContours(test_binary,region_test,0,Scalar(255),-1,8,noArray(),2,Point(offsetX,offsetY-height/2));
					drawContours(control_binary,region_control,0,Scalar(255),-1,8,noArray(),2,Point(offsetX-base/2,offsetY-height/2));
				}
				else if (i == 1) {
					drawContours(test_binary,region_test,0,Scalar(255),-1,8,noArray(),2,Point(offsetX+270-base/2,offsetY-height/2));
					drawContours(control_binary,region_control,0,Scalar(255),-1,8,noArray(),2,Point(offsetX+270,offsetY-height/2));
				}
				else if (i == 2) {
					drawContours(test_binary,region_test,0,Scalar(255),-1,8,noArray(),2,Point(offsetX,offsetY-height/2+60));
					drawContours(control_binary,region_control,0,Scalar(255),-1,8,noArray(),2,Point(offsetX-base/2,offsetY-height/2+60));
				}
				else if (i == 3) {
					drawContours(test_binary,region_test,0,Scalar(255),-1,8,noArray(),2,Point(offsetX+270-base/2,offsetY-height/2+60));
					drawContours(control_binary,region_control,0,Scalar(255),-1,8,noArray(),2,Point(offsetX+270,60+offsetY-height/2));
				}
				//drawContours(test1_binary,region_test1,0,Scalar(255),-1,8,noArray(),2,Point(offsetX,offsetY-height/2 - OFFSETY_2RECT));
				//drawContours(control1_binary,region_control1,0,Scalar(255),-1,8,noArray(),2,Point(offsetX-base/2,offsetY-height/2 - OFFSETY_2RECT));
				add(control_binary,frame_binary,frame_binary);
				add(test_binary,frame_binary,frame_binary);
				//add(control_binary,test1_binary,control_binary);
				//add(control_binary,control1_binary,control_binary);
				
			}
			else{
				cout << "nao achou uma regiao " << UNIDADE << endl;
				conc_part.push_back(0);
			}
			
			region_control.clear();
			region_test.clear();
			conc.push_back(conc_part);
			for (int j = 0; j<7; j++) {
				cout << conc_part[j] << endl;
			}
			//system("PAUSE");
			conc_part.clear();
			}
			cvtColor(frame_binary,frame_binary,CV_GRAY2BGR);
			addWeighted(frame,0.8,frame_binary,0.2,0,frame);
			imshow(PROGRAM_WINDOW,frame);

			vector<double> prev;
			prev.push_back(2.000000000);
			prev.push_back(4.000000000);
			prev.push_back(7.000000000);
			prev.push_back(15.00000000);
			vector<double> media,stdd,mediaPrev,stddPrev;
			meanStdDev(prev,mediaPrev,stddPrev);
			vector<double> corr; 
			for (int i = 0 ; i < 4 ; i++) {
				int Max,Min;
				Max = Min = 0;
				for (int j = 1; j < 7 ; j++) {
					if(conc[i][j] < conc[i][Min])
						Min = j;
					else if (conc[i][j] > conc[i][Max])
						Max = j;
				}
				vector<double> mediaCon,stddCon;
				vector<double> mask;
				for(int j = 0; j < 7; j++) {
					if(conc[i][j] != 0 && j != Min && j != Max)
						mask.push_back(conc[i][j]);
				}
				if ( !mask.empty()) {
					meanStdDev(mask,mediaCon,stddCon);
					cout << mediaCon[0] << endl << "CV: " << stddCon[0]*100/mediaCon[0] << " %" << endl << endl;
				}
				else
					cout << "concentracoes nao foram encontradas\n";
				corr.push_back(mediaCon[0]);
				mask.clear();
			}
			meanStdDev(corr,media,stdd);

			double r = 0;
			for(int i = 0 ; i < 4 ; i++)
				r = r + (corr[i]-media[0])*(prev[i]-mediaPrev[0]);
			r = r/((4)*stdd[0]*stddPrev[0]);
			double r2 = r*r;

			if(r2 >= 0.99) {
				cout << "Calibracao OK    R2 = " << r2 << endl << endl;
				double B = 0, A = 0, sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
				for (int i = 0; i < 4 ; i++) {
					sumX = sumX + corr[i];
					sumY = sumY + prev[i];
					sumXY = sumXY + (corr[i]*prev[i]);
					sumX2 = sumX2 + (corr[i]*corr[i]);
				}
				B = (4.0*sumXY - sumX*sumY)/(4.0*sumX2 - sumX*sumX);
				A = mediaPrev[0] - B*media[0];
				cout << "A = " << A << endl << "B = " << B << endl << endl;
			}
			else if(r2 >= 0.98) {
				cout << "E recomendavel realizar outra calibracao    R2 = " << r2 << endl << endl;
				double B = 0, A = 0, sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
				for (int i = 0; i < 4 ; i++) {
					sumX = sumX + corr[i];
					sumY = sumY + prev[i];
					sumXY = sumXY + (corr[i]*prev[i]);
					sumX2 = sumX2 + (corr[i]*corr[i]);
				}
				B = (4.0*sumXY - sumX*sumY)/(4.0*sumX2 - sumX*sumX);
				A = mediaPrev[0] - B*media[0];
				cout << "A = " << A << endl << "B = " << B << endl << endl;
			}
			else
				cout << "Refaca a calibracao    R2 = " << r2 << endl << endl;

			waitKey();

			conc.clear();
			test_padrao.clear();
			control_padrao.clear();
			all_test.clear();
			all_control.clear();
			prev.clear();
			media.clear();
			stdd.clear();
			/********************************************************/
		}
	}
	destroyAllWindows();
	return 0;
}


int searchCam (VideoCapture *camera) {
	//Pesquisa todas as cameras possiveis
	int x,y;
	for (x=0 ; x<10; x++) {
		//Abre a camera.
		camera->open(x);

		//Se falhar para abrir a camera 'y' sera nosso maximo de cameras.
		if(!camera->isOpened ()) {
			y = x;
			x += 10;
		}
		//Se nao falhar continua procurando.
		else 
			camera->release();
	}

	//Caso nao tenha cameras que possam ser usadas ocorre um erro.
	if (y == 0) {
		cout << "Error " << ERR_NO_CAMERA << endl;
		destroyAllWindows();
		exit (ERR_NO_CAMERA);
	}

	//Mostra as opcoes.
	cout << "Existem " << y << " cameras disponiveis.\n\nID das cameras:\n";
	for(x=0;x<y;x++) cout << x << endl;
	cout << "\nEscolha uma camera para usar.\n> ";

	//Processa a escolha do usuario.
	escolha_camera:
	cin >> x;
	if(x < 0 || x >= y) {
		cout << "\nID de camera invalido, tente novamente.\n> ";
		goto escolha_camera;
	}

	//Retorna a camera desejada.
	return x;
}

void createDrawWindow (string name , string type) {
	//Janela para as configuracoes do modelo com barras para altera-las
	namedWindow(name,CV_WINDOW_FREERATIO);
	
	//Se o modelo e um retangulo
	if (type == TYPE_RECT || type == TYPE_PADRAO) {
		createTrackbar("Base",name,&base,MAX_DRAWING_RECT_BASE);		//Base do retangulo
		createTrackbar("Height",name,&height,MAX_DRAWING_RECT_HEIGHT);	//Altura do retangulo
	}
	//Se o modelo e um circulo
	else if (type == TYPE_CIRCLE) {
		createTrackbar("Radius",name,&radius,MAX_DRAWING_CIRCLE_RADIUS);//Raio do ciruclo
	}

	createTrackbar("offsetX",name,&offsetX,sizeCols);					//Offset X do centro
	createTrackbar("offsety",name,&offsetY,sizeRows);					//Offset Y do centro
	createTrackbar("Text X",name,&text_offsetX,MAX_TEXT_OFFSET);		//Offset X do texto
	createTrackbar("Text Y",name,&text_offsetY,MAX_TEXT_OFFSET);		//Offset Y do texto
	createTrackbar("left_text",name,&left_text,MAX_TEXT_OFFSET);		//Offset X adicional do texto esquerdo
	createTrackbar("fontFace",name,&fontFace,MAX_FONT_TYPE);			//Fonte do texto
	createTrackbar("scale",name,&scale,MAX_FONT_SCALE);					//Tamanho do texto

	return;
}

Mat drawModel(string type, Mat image) {
	//Initialize drawing to all black.
	Mat	drawing = Mat::zeros(image.rows,image.cols,CV_8UC3);
	
	
	if (type == TYPE_CIRCLE) {
		//Draws a circle in drawing.
		circle(drawing,Point(offsetX,offsetY),radius,OUTLINE_COLOR,OUTLINE_THICKNESS);

		//Draws the vertical middle line of the circle.
		line(drawing,Point(offsetX,(offsetY-radius)),Point(offsetX,(offsetY+radius)),OUTLINE_COLOR,OUTLINE_THICKNESS);

		//Write text.
		putText(drawing,"C",Point((offsetX-radius-text_offsetX-left_text),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+radius+text_offsetX),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
	}
	
	else if (type == TYPE_RECT) {
		//Draws a rectangle in drawing.
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2),Point(offsetX+base/2,offsetY+height/2),Scalar(0,255,0),OUTLINE_THICKNESS);
		
		//Draws the vertical middle line of the rectangle.
		line(drawing,Point(offsetX,(offsetY-height/2)),Point(offsetX,(offsetY+height/2)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
		//Write text.
		putText(drawing,"C",Point((offsetX-base/2-text_offsetX-left_text),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+base/2+text_offsetX),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);

	}

	else if (type == TYPE_RECT2) {
		//Draws a rectangle in drawing.
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2 - OFFSETY_2RECT),Point(offsetX+base/2,offsetY+height/2 - OFFSETY_2RECT),Scalar(0,255,0),OUTLINE_THICKNESS);
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2 - OFFSETY_2RECT),Point(offsetX+base/2,offsetY+height/2 - OFFSETY_2RECT),Scalar(0,255,0),OUTLINE_THICKNESS);
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2 - OFFSETY_2RECT),Point(offsetX+base/2,offsetY+height/2 - OFFSETY_2RECT),Scalar(0,255,0),OUTLINE_THICKNESS);
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2 - OFFSETY_2RECT),Point(offsetX+base/2,offsetY+height/2 - OFFSETY_2RECT),Scalar(0,255,0),OUTLINE_THICKNESS);

		//Draws the vertical middle line of the rectangle.
		line(drawing,Point(offsetX,(offsetY-height/2 - OFFSETY_2RECT)),Point(offsetX,(offsetY+height/2-OFFSETY_2RECT)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		line(drawing,Point(offsetX,(offsetY-height/2 - OFFSETY_2RECT)),Point(offsetX,(offsetY+height/2-OFFSETY_2RECT)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		line(drawing,Point(offsetX,(offsetY-height/2 - OFFSETY_2RECT)),Point(offsetX,(offsetY+height/2-OFFSETY_2RECT)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		line(drawing,Point(offsetX,(offsetY-height/2 - OFFSETY_2RECT)),Point(offsetX,(offsetY+height/2-OFFSETY_2RECT)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
		//Write text.
		putText(drawing,"C",Point((offsetX-base/2-text_offsetX-left_text),(offsetY+text_offsetY-OFFSETY_2RECT)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+base/2+text_offsetX),(offsetY+text_offsetY-OFFSETY_2RECT)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);

	}

	else if (type == TYPE_PADRAO) {
		//Draws a rectangle in drawing.
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2),Point(offsetX+base/2,offsetY+height/2),Scalar(0,255,0),OUTLINE_THICKNESS);
		rectangle(drawing,Point(offsetX-base/2+270,offsetY-height/2),Point(offsetX+base/2+270,offsetY+height/2),Scalar(0,255,0),OUTLINE_THICKNESS);
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2+60),Point(offsetX+base/2,offsetY+height/2+60),Scalar(0,255,0),OUTLINE_THICKNESS);
		rectangle(drawing,Point(offsetX-base/2+270,offsetY-height/2+60),Point(offsetX+base/2+270,offsetY+height/2+60),Scalar(0,255,0),OUTLINE_THICKNESS);
		
		//Draws the vertical middle line of the rectangle.
		line(drawing,Point(offsetX,(offsetY-height/2)),Point(offsetX,(offsetY+height/2)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		line(drawing,Point(offsetX+270,(offsetY-height/2)),Point(offsetX+270,(offsetY+height/2)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		line(drawing,Point(offsetX,(offsetY-height/2+60)),Point(offsetX,(offsetY+height/2+60)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		line(drawing,Point(offsetX+270,(offsetY-height/2+60)),Point(offsetX+270,(offsetY+height/2+60)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
		//Write text.
		putText(drawing,"C",Point((offsetX-base/2-text_offsetX-left_text),(offsetY+text_offsetY-OFFSETY_2RECT)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+base/2+text_offsetX),(offsetY+text_offsetY-OFFSETY_2RECT)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);

	}
	
	//Add the actual frame with the drawing, forming a new image.
	addWeighted(image,FULL_WHEIGHT,drawing,THREE_FOURTHS_WHEIGHT,NO_WHEIGHT,image);


	return image;
}

void filter_strip(Mat &src, Mat &dst) {
	Size siz(_szWidth,_szHeight);
	GaussianBlur(src,dst,siz,(deltaX/100.0),(deltaY/100.0));
	return;
}

void filter_square(Mat &src, Mat &dst) {
	//_szWidht = _szHeight
	Size siz(_szWidth,_szWidth);
	GaussianBlur(src,dst,siz,7);
}

void proc(Mat &src, Mat &dst) {
	inRange(src,Scalar(_minHue,_minSat,_minVal),Scalar(_maxHue,_maxSat,_maxVal),dst);
	return;
}

//Porcentagem de acordo com o 'percentagem'*'src1' e (1 - 'percentagem')*'src2'.
Mat show(Mat src1, Mat src2, int percentage) {
	Mat result;
	addWeighted(src1,percentage/100.0,src2,(100-percentage)/100.0,0.0,result);
	return result;
}

//Vetor de matrizes: para multiplas imagens, areas,...
//'VcSize' corresponde a quantas imagens ha no vetor
//'numElements' e a quantidade de elementos que sao iguais a 1 dentro da matriz.
bool allOnes(Mat VcMat[], int VcSize, int numElements) {
	for(int z = 0; z < VcSize; z++) {
		if(countNonZero(VcMat[z]) < numElements) {
			return false;
		}
	}
	return true;
}

//Vetor de matrizes: para multiplas imagens, areas,...
//'VcSize' corresponde a quantas imagens ha no vetor
//'MatRow' e a abrangencia das linhas de uma determinada imagem.
//'MatCol' e a abrangencia das colunas de uma determinada imagem.
//'properties' ponteiro para as propriedades; essa funcao suporta ate 6 propriedades sendo que as 3 primeiras devem
//ser as minimas e as 3 ultimas as maximas; otimizado para minHue,minSat,minVal,maxHue,maxSat,maxVal, nessa ordem
//'src' e a imagem em HSV
//'dst' e a imagem em binario, 1 canal
void ajuste( int VcSize, Range MatRow[], Range MatCol[], int *properties[], Mat &src, Mat &dst){
	Mat VcMat[maxPoints];

	if (VcSize == 0) {
		cout << "VcSize = 0\n";
		return;
	}
	
	int u = 0;
	//Os minimos primeiro
	while(u < 3) {
		//Incrementa a primeira propriedade
		*properties[u] += 1;
		proc(src,dst);

		//Seleciona todas as imagens
		for (int y = 0; y < VcSize; y++) {
			VcMat[y] = dst(MatRow[y],MatCol[y]);
		}

		//Verifica se as imagens estao de acordo
		if(!allOnes(VcMat,VcSize,PRECISAO_BUSCA)) {
			*properties[u] -= 1;
			proc(src,dst);
			for (int y = 0; y < VcSize; y++) {
				VcMat[y] = dst(MatRow[y],MatCol[y]);
			}
			u += 1;
		}
	}
	//mesmos passos da anterior, mas para os valores maximos
	while (u < 6) {
		*properties[u] -= 1;
		proc(src,dst);
		for (int y = 0; y < VcSize; y++) {
			VcMat[y] = dst(MatRow[y],MatCol[y]);
		}
		if(!allOnes(VcMat,VcSize,PRECISAO_BUSCA)) {
			*properties[u] += 1;
			proc(src,dst);
			for (int y = 0; y < VcSize; y++) {
				VcMat[y] = dst(MatRow[y],MatCol[y]);
			}
			u += 1;
		}
	}
}

vector<vector<Point>> processamento(Mat HSV, Mat bin) {
	int *properties[6];
	Range MatRow[maxPoints];
	Range MatCol[maxPoints];
	double match = MINIMAL_MATCH,temp;
	vector<vector<Point>> best;
	vector<vector<Point>> contours;
	vector<Point> rect;
	rect.push_back(Point(1,1));
	rect.push_back(Point(1+AUTOBUSCA_LARGURA,1));
	rect.push_back(Point(1+AUTOBUSCA_LARGURA,1+AUTOBUSCA_ALTURA));
	rect.push_back(Point(1,1+AUTOBUSCA_ALTURA));
	rect.push_back(Point(1,1));
	properties[0] = &_minHue; properties[1] = &_minSat; properties[2] = &_minVal; properties[3] = &_maxHue; properties[4] = &_maxSat; properties[5] = &_maxVal;
	
	for( int ROWS = 0 ; ROWS < HSV.rows - AUTOBUSCA_ALTURA; ROWS+=AUTOBUSCA_PULO) {
		for( int COLS = 0 ; COLS < HSV.cols - AUTOBUSCA_LARGURA; COLS+=AUTOBUSCA_PULO) {
				_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
				proc(HSV,bin);
				MatRow[0] = Range(ROWS,ROWS+AUTOBUSCA_ALTURA);
				MatCol[0] = Range(COLS,COLS+AUTOBUSCA_LARGURA);
				
				ajuste(1,MatRow,MatCol,properties,HSV,bin);
				cout << COLS << " x " << ROWS << endl;
		
				findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
				cout << "Found " << contours.size() << " contours." << endl;

				if(contours.size() == 1 && pointPolygonTest(contours[0],Point2f(COLS,ROWS),false) >= 0) {
					approxPolyDP(contours[0],contours[0],APPROXIMATION,true);
					double area = contourArea(contours[0]);
					temp = matchShapes(rect,contours[0],CV_CONTOURS_MATCH_I2,0);
					cout << temp << endl;
					/*mt = Mat::zeros(mt.rows,mt.cols,CV_8UC1);
					drawContours(mt,contours,0,Scalar(255),-1);
					imshow("Detection",mt);
					cvtColor(mt,mtRGB,CV_GRAY2BGR);
					show(Trans);*/
					if(match > temp && area > MIN_AREA && temp != 0 && area < MAX_AREA) {
						match = temp;
						best.clear();
						best.push_back(contours[0]);
						//if( !best.empty() ) cout << "printed" << endl;
					}
					//waitKey();
				}
			}
		}

	return best;
}

double calculation(vector<vector<Point>> region, Mat image) {
	if( !region.empty() ) {
		Mat bin = Mat::zeros(image.rows,image.cols,CV_8UC1);
		drawContours(bin,region,0,Scalar(255),-1);
		Scalar mean,stdd;
		meanStdDev(image,mean,stdd,bin);
		cout << mean << endl << stdd << endl;
		return (mean[1] + 2*stdd[1]);
	}

	else
		return 0;
}