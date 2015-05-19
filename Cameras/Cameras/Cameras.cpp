// Cameras.cpp : Define o ponto de entrada para a aplicação de console.
//

#include "stdafx.h"

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

using namespace cv;
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	int x,y;
	VideoCapture cap;

	for (x=0 ; x<10; x++) {
		cap.open(x);

		if(!cap.isOpened ()) {
			y = x;
			x += 10;
		}
		else 
			cap.release();
	}

	if (y == 0) {
		cout << "Nao existem cameras disponiveis.\n";
		system("PAUSE");
		return 0;
	}

	cout << "Existem " << y << " cameras disponiveis.\n\nID das cameras:\n";
	for(x=0;x<y;x++) cout << x << endl;
	cout << "\nEscolha uma camera para usar.\n> ";

	loop1:
	cin >> x;
	if(x < 0 || x >= y) {
		cout << "\nID de camera invalido, tente novamente.\n> ";
		goto loop1;
	}

	cap.open(x);

	if(!cap.isOpened() ) {
		cout << "Nao foi possivel inicializar a camera " << x << endl;
		system("PAUSE");
		return -1;
	}

	namedWindow("Camera",CV_WINDOW_AUTOSIZE);
	

	while(1){
		Mat frame;

		if(!cap.read(frame)) {
			cout << "Nao foi possivel ler o frame da camera\n";
			system("PAUSE");
			return -1;
		}

		imshow("Camera", frame);
		char c = waitKey(30);
		if(c == 27) {
			cout << "ESC foi pressionado\n";
			destroyAllWindows();
			break;
		}

		else if (c == ' ') {
			cout << "Foto tirada\n";
		}
	}

	system("PAUSE");
	return 0;
}

