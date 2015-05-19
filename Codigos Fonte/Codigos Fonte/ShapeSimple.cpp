#include "stdafx.h"
#include "ShapeSimple.h"
#include "utils.h"


ShapeSimple::ShapeSimple(void)
{
	bordersColor = cv::Scalar(255);
	bordersSize = 1;
}

ShapeSimple::ShapeSimple(std::vector<cv::Point> _cr, std::vector<cv::Point> _tr)
{
	createRegions(_cr,_tr);
	bordersColor = cv::Scalar(255);
	bordersSize = 1;
}

ShapeSimple::ShapeSimple(const ShapeSimple &src)
{
	control = src.control;
	test = src.test;
	bordersColor = src.bordersColor;
	bordersSize = src.bordersSize;
	borders = src.borders.clone();
	C = src.C;
	T = src.T;
}


ShapeSimple::~ShapeSimple(void)
{
	borders.release();
}

ShapeSimple &ShapeSimple::operator= (ShapeSimple &src) {
	control = src.control;
	test = src.test;
	bordersColor = src.bordersColor;
	bordersSize = src.bordersSize;
	borders = src.borders.clone();
	C = src.C;
	T = src.T;
	return src;
}

bool ShapeSimple::printBorders (cv::Mat &src, cv::Mat &dst) {
	//If either 'borders' or source doesn't exist, it fails.
	if (borders.empty() || src.empty()) {
		dst = cv::Mat::zeros(src.rows,src.cols,src.type());
		errorHandler(var2str(ShapeSimple::printBorders()),"Nao foi possivel plotar a forma na imagem. A imagem da forma ou a imagem em si nao existem.");
		return false;
	}

	//If 'borders' isn't the same type and size of 'src', it won't print.
	if (borders.type() != src.type() || borders.size != src.size) {
		dst = cv::Mat::zeros(src.rows,src.cols,src.type());
		errorHandler(var2str(ShapeSimple::printBorders()),"Tamanho e/ou tipo da imagem e da imagem da forma nao sao iguais.");
		return false;
	}

	cv::add(src,borders,dst);
	return true;
}

void ShapeSimple::deleteRegions() {
	control = test = cv::Rect(cv::Point(0,0),cv::Point(0,0));
	C = T = cv::Point(0,0);
	borders.release();
	notificationHandler(var2str(ShapeSimple::deleteRegions()),"Regioes deletadas com sucesso.");
}

bool ShapeSimple::createRegions(std::vector<cv::Point> _cr, std::vector<cv::Point> _tr) {		
	if(_cr.empty() || _tr.empty()) {
		errorHandler(var2str(ShapeSimple::createRegions()),"'_cr' e/ou '_tr' estao vazios.");
		return false;
	}

	if(_cr[0] != _cr[_cr.size()-1] || _tr[0] != _tr[_tr.size()-1]) {
		errorHandler(var2str(ShapeSimple::createRegions()),"As regioes nao sao fechadas.");
		return false;
	}

	std::vector<cv::Point> controlRegion, testRegion;
	control = cv::boundingRect(_cr);
	controlRegion.push_back(cv::Point(control.x,control.y));
	controlRegion.push_back(cv::Point(control.x + control.width,control.y));
	controlRegion.push_back(cv::Point(control.x + control.width,control.y + control.height));
	controlRegion.push_back(cv::Point(control.x,control.y + control.height));
	controlRegion.push_back(cv::Point(control.x,control.y));
	test = cv::boundingRect(_tr);
	testRegion.push_back(cv::Point(test.x,test.y));
	testRegion.push_back(cv::Point(test.x + test.width,test.y));
	testRegion.push_back(cv::Point(test.x + test.width,test.y + test.height));
	testRegion.push_back(cv::Point(test.x,test.y + test.height));
	testRegion.push_back(cv::Point(test.x,test.y));

	for(unsigned int x = 0; x < testRegion.size(); x++) {
		if (cv::pointPolygonTest(controlRegion,testRegion[x],false) > 0) {
			errorHandler(var2str(ShapeSimple::createRegions()),"As regioes estao sobrepostas.");
			return false;
		}
	}

	cv::Point meanCon = meanPoint(controlRegion),meanTest = meanPoint(testRegion);

	double x = cv::abs<int>(meanCon.x - meanTest.x);
	double y = cv::abs<int>(meanCon.y - meanTest.y);

	if(x >= y) {
		if(meanCon.x > meanTest.x) {
			C = cv::Point(control.x + (control.width - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1), control.y + (control.height - 1)/2 + LETTER_HEIGHT/2);
			T = cv::Point(test.x - bordersSize/2 - (LETTER_DIST_BORDER - 1) - LETTER_WIDTH, test.y + (test.height - 1)/2 + LETTER_HEIGHT/2);
		}
		else {
			T = cv::Point(test.x + (test.width - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1), test.y + (test.height - 1)/2 + LETTER_HEIGHT/2);
			C = cv::Point(control.x - bordersSize/2 - (LETTER_DIST_BORDER - 1) - LETTER_WIDTH, control.y + (control.height - 1)/2 + LETTER_HEIGHT/2);
		}
	}
	else {
		if(meanCon.y > meanTest.y) {
			C = cv::Point(control.x + (control.width - 1)/2 - LETTER_WIDTH/2, control.y + (control.height - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1) + LETTER_HEIGHT);
			T = cv::Point(test.x + (test.width - 1)/2 - LETTER_WIDTH/2, test.y - bordersSize/2 - LETTER_DIST_BORDER - 3);
		}
		else {
			T = cv::Point(test.x + (test.width - 1)/2 - LETTER_WIDTH/2, test.y + (test.height - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1) + LETTER_HEIGHT);
			C = cv::Point(control.x + (control.width - 1)/2 - LETTER_WIDTH/2, control.y - bordersSize/2 - LETTER_DIST_BORDER - 3);
		}
	}

	notificationHandler(var2str(ShapeSimple::createRegions()),"Regioes criadas com sucesso.");
	return true;
}

bool ShapeSimple::prepareImage(const cv::Mat &image, bool withText) {
	if(image.type() != CV_8UC1 && image.type() != CV_8UC3) {
		errorHandler(var2str(ShapeSimple::prepareImage()),"Imagem nao e do tipo CV_8UC1 nem CV_8UC3.");
		return false;
	}

	if(control.area() == 0 || test.area() == 0) {
		borders = cv::Mat::zeros(image.rows,image.cols,image.type());
		return false;
	}

	borders = cv::Mat::zeros(image.rows,image.cols,image.type());

	cv::rectangle(borders,control,bordersColor,bordersSize);
	cv::rectangle(borders,test,bordersColor,bordersSize);

	if(withText) {
		cv::putText(borders,"C",C,TEXT_FONT,TEXT_SCALE,bordersColor,TEXT_THICKNESS);
		cv::putText(borders,"T",T,TEXT_FONT,TEXT_SCALE,bordersColor,TEXT_THICKNESS);
	}

	return true;
}

void ShapeSimple::swapRegions() {
	cv::Rect temp = control;
	control = test;
	test = temp;
	cv::Point temp2 = C;
	C = T;
	T = temp2;
	notificationHandler(var2str(ShapeSimple::swapRegions()),"As regioes foram trocadas.");
}

void ShapeSimple::setBordersAspect(cv::Scalar color,int size) {
	bordersColor = color;

	if(size <= 0) {
		errorHandler(var2str(ShapeSimple::setBordersAspect()),"Tamanho da borda deve ser um valor positivo nao nulo.");
		exit(-1);
	}

	double x = cv::abs<int>(C.x - T.x);
	double y = cv::abs<int>(C.y - T.y);

	if(x >= y) {
		if(C.x > T.x) {
			C.x = C.x - bordersSize/2 + size/2;
			T.x = T.x + bordersSize/2 - size/2;
		}
		else {
			T.x = T.x - bordersSize/2 + size/2;
			C.x = C.x + bordersSize/2 - size/2;
		}
	}
	else {
		if(C.y > T.y) {
			C.y = C.y - bordersSize/2 + size/2;
			T.y = T.y + bordersSize/2 - size/2;
		}
		else {
			T.y = T.y - bordersSize/2 + size/2;
			C.y = C.y + bordersSize/2 - size/2;
		}
	}

	bordersSize = size;
	notificationHandler(var2str(ShapeSimple::setBordersAspect()),"Aspecto da borda trocada com sucesso.");
}

cv::Point ShapeSimple::meanPoint(std::vector<cv::Point> point) {
	if(point.size() == 0) {
		errorHandler(var2str(ShapeSimple::meanPoint()),"Vetor vazio.");
		return cv::Point(0,0);
	}
	
	int x=0,y=0;
	for (unsigned int j = 0; j < point.size(); j++) {
		x += point[j].x;
		y += point[j].y;
	}
	x = x/point.size();
	y = y/point.size();

	return cv::Point(x,y);
}

std::ostream& operator<< (std::ostream& out,const ShapeSimple &src) {
	out << "\toperator<< for ShapeSimple\t";
	return out;
}

bool ShapeSimple::createRegions(std::vector<cv::Point> region) {
	if(region.empty()) {
		errorHandler(var2str(ShapeSimple::createRegions()),"'region' esta vazio.");
		return false;
	}

	if(region[0] != region[region.size()-1]) {
		errorHandler(var2str(ShapeSimple::createRegions()),"A regiao nao e fechada.");
		return false;
	}



	if(control.area() == 0) {
		control = cv::boundingRect(region);
		notificationHandler(var2str(ShapeSimple::createRegions()),"Regiao de controle criada com sucesso.");
		return true;
	}

	std::vector<cv::Point> controlRegion, testRegion;
	test = cv::boundingRect(region);
	controlRegion.push_back(cv::Point(control.x,control.y));
	controlRegion.push_back(cv::Point(control.x + control.width,control.y));
	controlRegion.push_back(cv::Point(control.x + control.width,control.y + control.height));
	controlRegion.push_back(cv::Point(control.x,control.y + control.height));
	controlRegion.push_back(cv::Point(control.x,control.y));
	testRegion.push_back(cv::Point(test.x,test.y));
	testRegion.push_back(cv::Point(test.x + test.width,test.y));
	testRegion.push_back(cv::Point(test.x + test.width,test.y + test.height));
	testRegion.push_back(cv::Point(test.x,test.y + test.height));
	testRegion.push_back(cv::Point(test.x,test.y));
	
	for(unsigned int x = 0; x < testRegion.size(); x++) {
		if (cv::pointPolygonTest(controlRegion,testRegion[x],false) > 0) {
			errorHandler(var2str(ShapeSimple::createRegions()),"As regioes estao sobrepostas.");
			return false;
		}
	}

	cv::Point meanCon = meanPoint(controlRegion),meanTest = meanPoint(testRegion);

	double x = cv::abs<int>(meanCon.x - meanTest.x);
	double y = cv::abs<int>(meanCon.y - meanTest.y);

	if(x >= y) {
		if(meanCon.x > meanTest.x) {
			C = cv::Point(control.x + (control.width - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1), control.y + (control.height - 1)/2 + LETTER_HEIGHT/2);
			T = cv::Point(test.x - bordersSize/2 - (LETTER_DIST_BORDER - 1) - LETTER_WIDTH, test.y + (test.height - 1)/2 + LETTER_HEIGHT/2);
		}
		else {
			T = cv::Point(test.x + (test.width - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1), test.y + (test.height - 1)/2 + LETTER_HEIGHT/2);
			C = cv::Point(control.x - bordersSize/2 - (LETTER_DIST_BORDER - 1) - LETTER_WIDTH, control.y + (control.height - 1)/2 + LETTER_HEIGHT/2);
		}
	}
	else {
		if(meanCon.y > meanTest.y) {
			C = cv::Point(control.x + (control.width - 1)/2 - LETTER_WIDTH/2, control.y + (control.height - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1) + LETTER_HEIGHT);
			T = cv::Point(test.x + (test.width - 1)/2 - LETTER_WIDTH/2, test.y - bordersSize/2 - LETTER_DIST_BORDER - 3);
		}
		else {
			T = cv::Point(test.x + (test.width - 1)/2 - LETTER_WIDTH/2, test.y + (test.height - 1) + bordersSize/2 + (LETTER_DIST_BORDER - 1) + LETTER_HEIGHT);
			C = cv::Point(control.x + (control.width - 1)/2 - LETTER_WIDTH/2, control.y - bordersSize/2 - LETTER_DIST_BORDER - 3);
		}
	}

	notificationHandler(var2str(ShapeSimple::createRegions()),"Regioes e identificadores setados com sucesso.");
	return true;
}
