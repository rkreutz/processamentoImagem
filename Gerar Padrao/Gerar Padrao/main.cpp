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
//#include "opencv2/core/core_c.h"
//#include "opencv2/highgui/highgui_c.h"
//#include "opencv2/imgproc/imgproc_c.h"
/*******************************************************************************/

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/
#define Ax 12.76
#define Bx -6.6414
#define CAMERA_SEARCH false
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
#define minHue 130
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
#define MINIMAL_MATCH 0.6
#define AUTOBUSCA_PULO 1
#define APPROXIMATION 3			//Quanto maior, mais a aproximacao se distancia do valor real
#define MIN_AREA 200		//Area minima da regiao de interesse
#define MAX_AREA 600				//Area maxima da regiao de interesse
#define UNIDADE "mg"
#define BASE 134
#define HEIGHT 34
#define OFFSETX 405
#define OFFSETY 356
#define OFFSETY_2RECT 50
#define OS_64BIT "x64"
#define OS_32BIT "x86"
#define GUID_LENGHT 50
#define PICTURES_TAKEN 7
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
#define ERR_NO_CONTROL_REGION -40
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
	//camera.open(camID);
	
	//Caso a camera nao consiga ser aberta corretamente retorna um erro.
	/*if( !camera.isOpened() ) {
		cout << "Error " << ERR_LOAD_CAMERA;
		destroyAllWindows();
		return (ERR_LOAD_CAMERA);
	}*/

	//Ler frame da camera
	/*if ( !camera.read(frame) ) {
		cout << "Error " << ERR_INITIAL_READ_FRAME;
		destroyAllWindows();
		return (ERR_INITIAL_READ_FRAME);
	}*/
	frame = imread("7mg-scanner.png",CV_LOAD_IMAGE_UNCHANGED);
	//Criando janela para configuracao do modelo.
	sizeCols = frame.cols;
	sizeRows = frame.rows;
	//createDrawWindow(DRAWING_CONFIG_WINDOW,type);
	namedWindow(PROGRAM_WINDOW,CV_GUI_EXPANDED);
		
	//Loop para mostrar a imagem
	while ( (command = waitKey(30)) != 27) {

		//Ler proximo frame.
		/*if ( !camera.read(frame) ) {
			cout << "Error " << ERR_READ_FRAME;
			destroyAllWindows();
			return (ERR_READ_FRAME);
		}*/

		//Junta p frame com o modelo e mostra o resultado.
		//frame = drawModel(type , frame);
		//frame = drawModel(TYPE_RECT2 , frame);
		//imshow(PROGRAM_WINDOW,frame);	

		//Comando para processar a imagem
		if ( command == ' ') {
			//vector<Mat> control, test;
			Mat control, test;

			/*for(int x = 0 ; x < PICTURES_TAKEN ; x++) {
				pictures.push_back(frame.clone());
				Sleep(2);
				if ( !camera.read(frame) ) {
					cout << "Error " << ERR_READ_FRAME;
					destroyAllWindows();
					return (ERR_READ_FRAME);
				}
			}*/

			//Selecionando imagens com o controle e teste
			/*if(type == TYPE_CIRCLE) {
				for (int x = 0; x < PICTURES_TAKEN ; x++) {
					control.push_back(pictures[x].colRange(offsetX-radius,offsetX));
					control[x] = control[x].rowRange(offsetY-radius,offsetY+radius);
					test.push_back(pictures[x].colRange(offsetX,offsetX+radius));
					test[x] = test[x].rowRange(offsetY-radius,offsetY+radius);
				}
			}*/
			//else
			 if(type == TYPE_RECT) {
				/*for (int x = 0; x < PICTURES_TAKEN ; x++) {
					control.push_back(pictures[x].colRange(offsetX-base/2,offsetX));
					control[x] = control[x].rowRange(offsetY-height/2,offsetY+height/2);
					test.push_back(pictures[x].colRange(offsetX,offsetX+base/2));
					test[x] = test[x].rowRange(offsetY-height/2,offsetY+height/2);
				}*/
					control = frame.colRange(0,frame.cols/2 - 1);
					test = frame.colRange(frame.cols/2,frame.cols - 1);
			}

			/*if ( (control.empty() || test.empty()) && (control.size() < PICTURES_TAKEN || test.size() < PICTURES_TAKEN) ) {
				cout << "Error " << ERR_EMPTY_IMAGES ;
				destroyAllWindows();
				return(ERR_EMPTY_IMAGES);
			}*/

			/********************************************************/
			//Comeco do processamento
			//system("cls");
			//cout << "[          ] 0 %" << endl;
			vector<double> concentrations;
			
				_szWidth = szWidth; _szHeight = szHeight; deltaX = deltaXGaussianBlur; deltaY = deltaYGaussianBlur;
				_minHue = minHue; _minSat = minSat; _minVal = minVal; _maxHue = maxHue; _maxSat = maxSat; _maxVal = maxVal;
				Mat test_HSV;
				Mat control_HSV; 
				Mat test_binary,control_binary;
				vector<vector<Point>> region_control, region_test;
				
				/*for (int x = 0 ; region_control.empty() ; x++) {
					if(x == PICTURES_TAKEN) {
						cout << "Error " << ERR_NO_CONTROL_REGION << endl;
						return ERR_NO_CONTROL_REGION;
					}
					control_binary = Mat::zeros(control[x].rows,control[x].cols,CV_8UC1);
					control_HSV = control[x].clone();
					cvtColor(control_HSV,control_HSV,CV_BGR2HSV);
					filter_strip(control_HSV,control_HSV);
					region_control = processamento(control_HSV,control_binary);
				}*/
				control_binary = Mat::zeros(control.rows,control.cols,CV_8UC1);
				control_HSV = control.clone();
				cvtColor(control_HSV,control_HSV,CV_BGR2HSV);
				filter_strip(control_HSV,control_HSV);
				region_control = processamento(control_HSV,control_binary);

				/*for (int x = 0 ; region_test.empty() ; x++) {
					test_binary = Mat::zeros(test[x].rows,test[x].cols,CV_8UC1);
					test_HSV = test[x].clone();
					cvtColor(test_HSV,test_HSV,CV_BGR2HSV);
					filter_strip(test_HSV,test_HSV);
					region_test = processamento(test_HSV,test_binary);
				}*/
				test_binary = Mat::zeros(test.rows,test.cols,CV_8UC1);
				test_HSV = test.clone();
				cvtColor(test_HSV,test_HSV,CV_BGR2HSV);
				filter_strip(test_HSV,test_HSV);
				region_test = processamento(test_HSV,test_binary);
				
				cout << "REGIONS OK\n\n";
				/*if( !region_test.empty() ) {
					Scalar test_img_mean = Scalar(0),test_img_stdd = Scalar(0),control_img_mean = Scalar(0),control_img_stdd = Scalar(0);
					for (int x = 0; x < PICTURES_TAKEN; x++) {
						Scalar test_mean,test_stdd,control_mean,control_stdd;
						test_binary = Mat::zeros(test[x].rows,test[x].cols,CV_8UC1);
						control_binary = Mat::zeros(control[x].rows,control[x].cols,CV_8UC1);
						test_HSV = test[x].clone();
						control_HSV = control[x].clone();
						imshow("control",control_HSV);
						imshow("test",test_HSV);
						drawContours(test_binary,region_test,0,Scalar(255),-1);
						drawContours(control_binary,region_control,0,Scalar(255),-1);
						meanStdDev(test_HSV,test_mean,test_stdd,test_binary);
						meanStdDev(control_HSV,control_mean,control_stdd,control_binary);
						test_img_mean = test_img_mean + test_mean;
						test_img_stdd = test_img_stdd + test_stdd;
						control_img_mean = control_img_mean + control_mean;
						control_img_stdd = control_img_stdd + control_stdd;
					}
					test_img_mean = test_img_mean/(double (PICTURES_TAKEN));
					test_img_stdd = test_img_stdd/(double (PICTURES_TAKEN));
					control_img_mean = control_img_mean/(double (PICTURES_TAKEN));
					control_img_stdd = control_img_stdd/(double (PICTURES_TAKEN));
					default_random_engine gen;
					normal_distribution<double> H(test_img_mean[0],test_img_stdd[0]);
					normal_distribution<double> S(test_img_mean[1],test_img_stdd[1]);
					normal_distribution<double> V(test_img_mean[2],test_img_stdd[2]);
					Mat norm_dist(600,1000,CV_8UC3,Scalar(255,255,255));
					for (int y = 0; y < 600 ; y++) {
						for (int x = 0 ; x < 200 ; x++) {
							Point3_<uchar>* P = norm_dist.ptr<Point3_<uchar> >(y,x);
							P->x = 0;
							//P->x = mean[0];
							P->y = 0;
							//P->y = mean[1];
							P->z = 0;
							//P->z = mean[2];
						}
						for (int x = 800 ; x < 1000 ; x++) {
							Point3_<uchar>* P = norm_dist.ptr<Point3_<uchar> >(y,x);
							P->x = H(gen);
							//P->x = mean[0];
							P->y = S(gen);
							//P->y = mean[1];
							P->z = V(gen);
							//P->z = mean[2];
						}
					}
					
					normal_distribution<double> Hc(control_img_mean[0],control_img_stdd[0]);
					normal_distribution<double> Sc(control_img_mean[1],control_img_stdd[1]);
					normal_distribution<double> Vc(control_img_mean[2],control_img_stdd[2]);
					Mat norm_dist_c(600,1000,CV_8UC3,Scalar(255,255,255));
					for (int y = 0; y < 600 ; y++) {
						for (int x = 0 ; x < 200 ; x++) {
							Point3_<uchar>* P = norm_dist_c.ptr<Point3_<uchar> >(y,x);
							P->x = Hc(gen);
							//P->x = mean[0];
							P->y = Sc(gen);
							//P->y = mean[1];
							P->z = Vc(gen);
							//P->z = mean[2];
						}
						for (int x = 800 ; x < 1000 ; x++) {
							Point3_<uchar>* P = norm_dist_c.ptr<Point3_<uchar> >(y,x);
							P->x = 0;
							//P->x = mean[0];
							P->y = 0;
							//P->y = mean[1];
							P->z = 0;
							//P->z = mean[2];
						}
					}
					Mat present;
					GaussianBlur(norm_dist,norm_dist,Size(5,5),5);
					GaussianBlur(norm_dist_c,norm_dist_c,Size(5,5),5);
					add(norm_dist,norm_dist_c,present);
					imshow("result",present);
					waitKey();
					vector<int> parameters;
					parameters.push_back(IMWRITE_JPEG_QUALITY);
					parameters.push_back(100);
					stringstream str1,str2;
					str1 << "result.jpg";
					if ( !imwrite(str1.str(),present,parameters) ) {
						cout << "Failed to save test image.";
						return (-41);
					}
					waitKey();
					destroyWindow("result");
				}
				else {
					//cout << "Imagem vazia: Concentracao da amostra:\n 0 " << UNIDADE << endl;
					cout << "Nao foi identificada linha de teste.\n";
				}*/
			
			
		
			//control.clear();
			//test.clear();
			Scalar test_mean,test_stdd,control_mean,control_stdd;
			test_binary = Mat::zeros(test.rows,test.cols,CV_8UC1);
			control_binary = Mat::zeros(control.rows,control.cols,CV_8UC1);
			test_HSV = test.clone();
			control_HSV = control.clone();
			imshow("control",control_HSV);
			imshow("test",test_HSV);
			drawContours(test_binary,region_test,0,Scalar(255),-1);
			drawContours(control_binary,region_control,0,Scalar(255),-1);
			meanStdDev(test_HSV,test_mean,test_stdd,test_binary);
			meanStdDev(control_HSV,control_mean,control_stdd,control_binary);
			
			default_random_engine gen;
			normal_distribution<double> H((double)test_mean[0],(double)test_stdd[0]);
			normal_distribution<double> S((double)test_mean[1],(double)test_stdd[1]);
			normal_distribution<double> V((double)test_mean[2],(double)test_stdd[2]);
			Mat norm_dist(600,1000,CV_8UC3,Scalar(255,255,255));
			for (int y = 0; y < 600 ; y++) {
				for (int x = 0 ; x < 200 ; x++) {
					Point3_<uchar>* P = norm_dist.ptr<Point3_<uchar> >(y,x);
					P->x = 0;
					//P->x = mean[0];
					P->y = 0;
					//P->y = mean[1];
					P->z = 0;
					//P->z = mean[2];
				}
				for (int x = 800 ; x < 1000 ; x++) {
					Point3_<uchar>* P = norm_dist.ptr<Point3_<uchar> >(y,x);
					P->x = H(gen);
					//P->x = mean[0];
					P->y = S(gen);
					//P->y = mean[1];
					P->z = V(gen);
					//P->z = mean[2];
				}
			}
			cout << (double)test_stdd[0] << endl;
			normal_distribution<double> Hc((double)control_mean[0],(double)control_stdd[0]);
			normal_distribution<double> Sc((double)control_mean[1],(double)control_stdd[1]);
			normal_distribution<double> Vc((double)control_mean[2],(double)control_stdd[2]);
			Mat norm_dist_c(600,1000,CV_8UC3,Scalar(255,255,255));
			for (int y = 0; y < 600 ; y++) {
				for (int x = 0 ; x < 200 ; x++) {
					Point3_<uchar>* P = norm_dist_c.ptr<Point3_<uchar> >(y,x);
					P->x = Hc(gen);
					//P->x = mean[0];
					P->y = Sc(gen);
					//P->y = mean[1];
					P->z = Vc(gen);
					//P->z = mean[2];
				}
				for (int x = 800 ; x < 1000 ; x++) {
					Point3_<uchar>* P = norm_dist_c.ptr<Point3_<uchar> >(y,x);
					P->x = 0;
					//P->x = mean[0];
					P->y = 0;
					//P->y = mean[1];
					P->z = 0;
					//P->z = mean[2];
				}
			}
			Mat present;
			GaussianBlur(norm_dist,norm_dist,Size(5,5),5);
			GaussianBlur(norm_dist_c,norm_dist_c,Size(5,5),5);
			add(norm_dist,norm_dist_c,present);
			imshow("result",present);
			waitKey();
			vector<int> parameters;
			parameters.push_back(IMWRITE_JPEG_QUALITY);
			parameters.push_back(100);
			stringstream str1,str2;
			str1 << "result2mg.jpg";
			if ( !imwrite(str1.str(),present,parameters) ) {
				cout << "Failed to save test image.";
				return (-41);
			}
			waitKey();
			
			/********************************************************/
		}

		//pictures.clear();
	}
	destroyAllWindows();
	system("PAUSE");
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
						if( !best.empty() ) cout << "printed " << area << " " << temp << endl;
						//system("PAUSE");
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
		//cout << mean << endl << stdd << endl;
		return (mean[1] + 2*stdd[1]);
	}

	else
		return 0;
}