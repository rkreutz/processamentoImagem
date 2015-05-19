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
#include <iomanip>
#include <cstdlib>
#include <random>
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
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
/*******************************************************************************/

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/
#define Ax 8.3812
#define Bx -2.4745
#define Aerr -0.029
#define Berr 0.2859
#define CAMERA_SEARCH true
#define CAMERA_ID 1
#define DRAWING_CONFIG_WINDOW "Referencia - Config"
#define PROGRAM_WINDOW "Programa"
#define TYPE_CIRCLE "CIRCLE"
#define TYPE_RECT "RECT"
#define TYPE_RECT2 "RECT2"
#define MAX_DRAWING_CIRCLE_RADIUS 400
#define MAX_DRAWING_RECT_BASE 400
#define MAX_DRAWING_RECT_HEIGHT 400
#define MAX_FONT_TYPE 7
#define MAX_FONT_SCALE 30
#define MAX_TEXT_OFFSET 200
#define MAX_LEFT_TEXT_OFFSET 50
#define OUTLINE_COLOR Scalar(20,100,20)
#define OUTLINE_THICKNESS 4
#define TEXT_THICKNESS 2
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
#define MIN_AREA 200		//Area minima da regiao de interesse
#define MAX_AREA 400				//Area maxima da regiao de interesse
#define UNIDADE "mg"
#define BASE 134
#define HEIGHT 34
#define OFFSETX 405
#define OFFSETY 356
#define OFFSETY_2RECT 50
#define OS_64BIT "x64"
#define OS_32BIT "x86"
#define GUID_LENGHT 50
#define PICTURES_TAKEN 15
#define TEXT_OFFSETX 17
#define TEXT_OFFSETY 15
#define LEFT_TEXT_OFFSETX 25
#define TEXT_FONT 3
#define TEXT_SCALE 13
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
#define ERR_CAMSEARCH_DEVICES_NEGATIVE -40
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
double calculation(vector<vector<Point>> region_test, Mat image_test,vector<vector<Point>> region_control, Mat image_control);

int _tmain (int argc, char **argv) {
	system("TITLE Input/Output Window");
	base = BASE;
	height = HEIGHT;
	offsetX = OFFSETX;
	offsetY = OFFSETY;
	text_offsetX = TEXT_OFFSETX;
	text_offsetY = TEXT_OFFSETY;
	fontFace = TEXT_FONT;
	scale = TEXT_SCALE;
	left_text = LEFT_TEXT_OFFSETX;
	OSArch = "x64";
	VideoCapture camera;
	Mat frame;
	vector<Mat> pictures;
	int camID;
	char command;
	type = TYPE_RECT;

	//Caso a escolha de cameras esteja habilitada.
	if (CAMERA_SEARCH)
		camID = searchCam(&camera);
	else
		camID = CAMERA_ID;
	
	//system("PAUSE");
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
	namedWindow(PROGRAM_WINDOW,CV_GUI_EXPANDED);
		
	//Loop para mostrar a imagem
	while ( (command = waitKey(30)) != 27) {

		//Ler proximo frame.
		if ( !camera.read(frame) ) {
			cout << "Error " << ERR_READ_FRAME;
			destroyAllWindows();
			return (ERR_READ_FRAME);
		}

		//Junta p frame com o modelo e mostra o resultado.
		frame = drawModel(type , frame);
		//frame = drawModel(TYPE_RECT2 , frame);
		imshow(PROGRAM_WINDOW,frame);	

		//Comando para processar a imagem
		if ( command == ' ') {
			vector<Mat> control, test;

			for(int x = 0 ; x < PICTURES_TAKEN ; x++) {
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
				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					control.push_back(pictures[x].colRange(offsetX-radius,offsetX));
					control[x] = control[x].rowRange(offsetY-radius,offsetY+radius);
					test.push_back(pictures[x].colRange(offsetX,offsetX+radius));
					test[x] = test[x].rowRange(offsetY-radius,offsetY+radius);
				}
			}

			else if(type == TYPE_RECT) {
				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					control.push_back(pictures[x].colRange(offsetX-base/2,offsetX));
					control[x] = control[x].rowRange(offsetY-height/2,offsetY+height/2);
					test.push_back(pictures[x].colRange(offsetX,offsetX+base/2));
					test[x] = test[x].rowRange(offsetY-height/2,offsetY+height/2);
				}
					/*control = frame.colRange(offsetX-base/2,offsetX);
					control = control.rowRange(offsetY-height/2,offsetY+height/2);
					test = frame.colRange(offsetX,offsetX+base/2);
					test = test.rowRange(offsetY-height/2,offsetY+height/2);*/
			}

			if ( (control.empty() || test.empty()) && (control.size() < PICTURES_TAKEN || test.size() < PICTURES_TAKEN) ) {
				cout << "Error " << ERR_EMPTY_IMAGES ;
				destroyAllWindows();
				return(ERR_EMPTY_IMAGES);
			}

			/********************************************************/
			//Comeco do processamento
			system("cls");
			cout << "[          ] 0 %" << endl;
			//displayOverlay(PROGRAM_WINDOW,"[          ] 0 %");
			vector<double> concentrations;
			vector<vector<Point>> region_control, region_test;
			Mat test_HSV,control_HSV,test_binary,control_binary;
			//for (int x = 0 ; x < PICTURES_TAKEN; x++) {
			for(int x = 0; x < PICTURES_TAKEN && ((region_control.empty()) || (region_test.empty())) ; x++) {
				_szWidth = szWidth; _szHeight = szHeight; deltaX = deltaXGaussianBlur; deltaY = deltaYGaussianBlur;
				_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
				test_HSV = test[x].clone();
				control_HSV = control[x].clone();
				test_binary = Mat::zeros(test[x].rows,test[x].cols,CV_8UC1);control_binary = Mat::zeros(control[x].rows,control[x].cols,CV_8UC1);
				
				cvtColor(control_HSV,control_HSV,CV_BGR2HSV);
				cvtColor(test_HSV,test_HSV,CV_BGR2HSV);

				filter_strip(control_HSV,control_HSV);
				filter_strip(test_HSV,test_HSV);

				if(region_test.empty())
					region_test = processamento(test_HSV,test_binary);
				if(region_control.empty())
					region_control = processamento(control_HSV,control_binary);
			}
				for(int x = 0; x < PICTURES_TAKEN ; x++) {
				if( !region_control.empty() && !region_test.empty()){
					test_HSV = test[x].clone();control_HSV = control[x].clone();
					cvtColor(control_HSV,control_HSV,CV_BGR2HSV);
					cvtColor(test_HSV,test_HSV,CV_BGR2HSV);
					filter_strip(control_HSV,control_HSV);
					filter_strip(test_HSV,test_HSV);
					double concentration = calculation(region_test,test_HSV,region_control,control_HSV);
					
					//concentration = Ax*concentration + Bx;
					if (concentration >= 0)
						concentrations.push_back(concentration);
					else
						concentrations.push_back(0);
			
					/*cout << "Concentracao da amostra:\n" << concentration << " " << UNIDADE << endl ;//<< concentration1 << " " << UNIDADE << endl;
					test_binary = control_binary = Mat::zeros(frame.rows,frame.cols,CV_8UC1);
					drawContours(test_binary,region_test,0,Scalar(255),-1,8,noArray(),2,Point(offsetX,offsetY-height/2));
					drawContours(control_binary,region_control,0,Scalar(255),-1,8,noArray(),2,Point(offsetX-base/2,offsetY-height/2));
					add(control_binary,test_binary,control_binary);
					cvtColor(control_binary,control_binary,CV_GRAY2BGR);
					addWeighted(frame,0.8,control_binary,0.2,0,frame);
					imshow(PROGRAM_WINDOW,frame);*/
				}
				else {
					//cout << "Imagem vazia: Concentracao da amostra:\n 0 " << UNIDADE << endl;
					concentrations.push_back(0);
				}
				}
				region_test.clear();
					region_control.clear();
				/*system("cls");
				char c = 254;
				stringstream str;
				cout << "[";
				//str << "[";
				double ratio = (x+1)*10.0/PICTURES_TAKEN;
				for (double y = 1.000; y <= 10.000; y+=1.000) {
					if (y-ratio <= 0.5000)
						cout << c;
						//str << c;
					else
						//str << " ";
						cout << " ";
				}
				if (10.0*ratio < 100.0 && 10*ratio >= 10)
					cout << "]  " << setprecision(2) << 10*ratio << " %\n\n";
					//str <<  "]  " << setprecision(2) << 10*ratio << " %";
				else if (10.0*ratio == 100.0)
					//str << "] " << setprecision(3) << 10*ratio << " %";
					cout << "] " << setprecision(3) << 10*ratio << " %\n\n";
				else if (10.0*ratio < 10.0)
					//str << "]   " << setprecision(1) << 10*ratio << " %";
					cout << "]   " << setprecision(1) << 10*ratio << " %\n\n";

				//displayOverlay(PROGRAM_WINDOW, str.str());

				//waitKey();
			//}*/
			int minX = 0, maxX = 0;
			for (int x = 1; x < PICTURES_TAKEN; x++) {
				if (concentrations[minX] > concentrations[x]) {
					minX = x;
				}
				else if (concentrations[maxX] < concentrations[x]) {
					maxX = x;
				}
			}

			vector<double> mask;
			int zeros = 0;
			for (int x = 0; x < PICTURES_TAKEN ; x++) {
				if (x != minX && x != maxX) {
					if( concentrations[x] > 0)
						mask.push_back(concentrations[x]);
					else
						zeros++;
				}
			}
			
			vector<double> mean,stdd;
			meanStdDev(mask,mean,stdd);
			
			char c = 241;
			cout << "Concentracao da amostra:\n" << setprecision(4) << mean[0] << " " <<  c << setprecision(4) << 3*stdd[0]  << " " << UNIDADE << endl ;
			cout << "CV: " << setprecision(3) << stdd[0]*100/mean[0] << " %" << endl;
			for (int x = 0; x < PICTURES_TAKEN ; x++)
				cout << setprecision(4) << concentrations[x] << " mg\t";
			cout << endl;
			for (int x = 0; x < mask.size() ; x++)
				cout << setprecision(4) << mask[x] << " mg\t";
			cout << endl;
			if (zeros > PICTURES_TAKEN-zeros)
				cout << "E recomendavel realizar outra medicao, muitos termos nulos\n";
			else if (stdd[0]*100.0/mean[0] > 4)
				cout << "E recomendavel realizar outra medicao, CV muito alto.\n";

			stringstream str;
			str << "Concentracao: " << setprecision(4) << mean[0] << " " << UNIDADE; 

			displayOverlay(PROGRAM_WINDOW, str.str(), 5000);

			waitKey(5000);

			mean.clear();
			stdd.clear();
			mask.clear();
			concentrations.clear();
			control.clear();
			test.clear();
			/********************************************************/
		}

		pictures.clear();
	}
	destroyAllWindows();
	return 0;
}


int searchCam (VideoCapture *camera) {
	//Pesquisa todas as cameras possiveis
	int x,y=0;

	VideoCapture a; 
	y = a.listCameras();

	//Caso nao tenha cameras que possam ser usadas ocorre um erro.
	if (y == 0) {
		cout << "Error " << ERR_NO_CAMERA << endl;
		destroyAllWindows();
		exit (ERR_NO_CAMERA);
	}

	//Mostra as opcoes.
	else if (y == 1) 
		cout << "Existe " << y << " camera disponivel.\n";
	else if (y >= 2)
		cout << "Existem " << y << " cameras disponiveis.\n";
	else {
		cout << "Error " << ERR_CAMSEARCH_DEVICES_NEGATIVE;
		exit (ERR_CAMSEARCH_DEVICES_NEGATIVE);
	}
	cout << "\nEscolha uma camera para usar.\n> ";

	//Processa a escolha do usuario.
	escolha_camera:
	cin >> x;
	if(x < 0 || x >= y) {
		cout << "\nID de camera invalido, tente novamente.\n> ";
		goto escolha_camera;
	}

	//Retorna a camera desejada.
	system("cls");
	return x;
}

void createDrawWindow (string name , string type) {
	//Janela para as configuracoes do modelo com barras para altera-las
	namedWindow(name,CV_WINDOW_FREERATIO);
	
	//Se o modelo e um retangulo
	if (type == TYPE_RECT) {
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
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2),Point(offsetX+base/2,offsetY+height/2),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
		//Draws the vertical middle line of the rectangle.
		line(drawing,Point(offsetX,(offsetY-height/2)),Point(offsetX,(offsetY+height/2)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
		//Write text.
		putText(drawing,"C",Point((offsetX-base/2-text_offsetX-left_text),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);
		putText(drawing,"T",Point((offsetX+base/2+text_offsetX),(offsetY+text_offsetY)),fontFace,scale/10.0,OUTLINE_COLOR,TEXT_THICKNESS);

	}

	else if (type == TYPE_RECT2) {
		//Draws a rectangle in drawing.
		rectangle(drawing,Point(offsetX-base/2,offsetY-height/2 - OFFSETY_2RECT),Point(offsetX+base/2,offsetY+height/2 - OFFSETY_2RECT),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
		//Draws the vertical middle line of the rectangle.
		line(drawing,Point(offsetX,(offsetY-height/2 - OFFSETY_2RECT)),Point(offsetX,(offsetY+height/2-OFFSETY_2RECT)),OUTLINE_COLOR,OUTLINE_THICKNESS);
		
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
	Size siz(_szHeight,_szHeight);
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
				//cout << COLS << " x " << ROWS << endl;
		
				findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
				//cout << "Found " << contours.size() << " contours." << endl;

				if(contours.size() == 1 && pointPolygonTest(contours[0],Point2f(COLS,ROWS),false) >= 0) {
					approxPolyDP(contours[0],contours[0],APPROXIMATION,true);
					double area = contourArea(contours[0]);
					temp = matchShapes(rect,contours[0],CV_CONTOURS_MATCH_I2,0);
					//cout << temp << endl;
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

double calculation(vector<vector<Point>> region_test, Mat image_test,vector<vector<Point>> region_control, Mat image_control) {
	double test,control,errorCorrection;
	if( !region_test.empty() && !region_control.empty()) {
		Mat bin = Mat::zeros(image_test.rows,image_test.cols,CV_8UC1);
		drawContours(bin,region_test,0,Scalar(255),-1);
		Scalar mean,stdd;
		meanStdDev(image_test,mean,stdd,bin);
		//cout << mean << endl << stdd << endl;
		test = (mean[1] + 2*stdd[1]);
		errorCorrection = mean[2];
		bin.release();
		bin = Mat::zeros(image_test.rows,image_test.cols,CV_8UC1);
		drawContours(bin,region_control,0,Scalar(255),-1);
		meanStdDev(image_control,mean,stdd,bin);
		//cout << mean << endl << stdd << endl;
		control = (mean[1] + 2*stdd[1]);
		errorCorrection = mean[2] - errorCorrection;
		errorCorrection = Aerr*errorCorrection + Berr;
		return ((test/control));
	}
	else
		return 0;
}