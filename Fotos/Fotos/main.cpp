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

using namespace std;
using namespace cv;

//Constants
/*******************************************************************************/
#define CAMERA_SEARCH false
#define CAMERA_ID 1
#define PICTURES_TAKEN 15
#define PROGRAM_WINDOW "Programa"
/*******************************************************************************/

//Error Constants
/*******************************************************************************/
#define ERR_NO_CAMERA -32
#define ERR_LOAD_CAMERA -33
#define ERR_INITIAL_READ_FRAME -34
#define ERR_READ_FRAME -35
/*******************************************************************************/



//Global Variables
/*******************************************************************************/

/*******************************************************************************/

//Retorna a camera escolhida pelo usuario.
int searchCam (VideoCapture *camera);


int _tmain (int argc, char **argv) {
	int camID;
	VideoCapture camera;
	Mat frame;

	if (CAMERA_SEARCH)
		camID = searchCam(&camera);
	else
		camID = CAMERA_ID;

	camera.open(camID);
	
	//Caso a camera nao consiga ser aberta corretamente retorna um erro.
	if( !camera.isOpened() ) {
		cout << "Error " << ERR_LOAD_CAMERA;
		destroyAllWindows();
		return (ERR_LOAD_CAMERA);
	}

	if ( !camera.read(frame) ) {
		cout << "Error " << ERR_INITIAL_READ_FRAME;
		destroyAllWindows();
		return (ERR_INITIAL_READ_FRAME);
	}

	char command = 0;
	int fotos = 0;
	namedWindow(PROGRAM_WINDOW,CV_GUI_NORMAL);

	while ( (command = waitKey(30)) != 27) {

		//Ler proximo frame.
		if ( !camera.read(frame) ) {
			cout << "Error " << ERR_READ_FRAME;
			destroyAllWindows();
			return (ERR_READ_FRAME);
		}

		imshow(PROGRAM_WINDOW,frame);

		if ( command == ' ') {
			vector<Mat> pictures;

			for(int x = 0 ; x < PICTURES_TAKEN ; x++) {
				pictures.push_back(frame.clone());
				Sleep(50);
				if ( !camera.read(frame) ) {
					cout << "Error " << ERR_READ_FRAME;
					destroyAllWindows();
					return (ERR_READ_FRAME);
				}
			}

			vector<int> parameters;
			parameters.push_back(IMWRITE_JPEG_QUALITY);
			parameters.push_back(100);

			for(int x = 0 ; x < PICTURES_TAKEN ; x++) {
				stringstream str;
				str << "7mg" << fotos << ".jpg";
				if ( !imwrite(str.str(),pictures[x],parameters) ) {
					cout << "Failed to save test image.";
					return (-41);
				}
				fotos++;
			}

			pictures.clear();
		}
	}

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
