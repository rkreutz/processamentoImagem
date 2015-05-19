#include "stdafx.h"
#include "callbacks.h"
#include "utils.h"

void drawingRegion(int event,int x, int y,int flag, void* userdata) {
	cv::Mat *image, *auxiliar;
	ShapeSimple *shape;
	std::vector<cv::Point> *region;
	Drawing *arguments = (Drawing*)userdata;
	image = arguments->image;
	shape = arguments->shape;
	region = arguments->region;
	auxiliar = arguments->auxiliar;

	if(image->empty()) {
		errorHandler(var2str(MOUSE_CALLBACKS::drawingRegion()),"Imagem passada como argumento para o desenho esta vazia.");
		exit(-1);
	}

	if (event == CV_EVENT_LBUTTONDOWN) {
		if(region->empty()) {
			region->push_back(cv::Point(x,y));
		}
		else {
			cv::line(*image,(*region)[region->size()-1],cv::Point(x,y),shape->getBorderColor(),shape->getBorderSize());
			*auxiliar = (image->clone());
			region->push_back(cv::Point(x,y));
		}
		notificationHandler(var2str(MOUSE_CALLBACKS::drawingRegion()),"Botao esquerdo pressionado.");
	}
	
	else if (event == CV_EVENT_MOUSEMOVE) {
		if(!region->empty()) {
			*image = (auxiliar->clone());
			cv::line(*image,(*region)[region->size()-1],cv::Point(x,y),shape->getBorderColor(),shape->getBorderSize());
		}
	}
	
	else if (event == CV_EVENT_RBUTTONDOWN) {
		if(region->size() > 1) {
			*image = (auxiliar->clone());
			cv::line(*image,(*region)[region->size()-1],(*region)[0],shape->getBorderColor(),shape->getBorderSize());
			*auxiliar = (image->clone());
			cv::Point fim = (*region)[0];
			region->push_back(fim);
			shape->createRegions(*region);
			region->clear();
		}
		notificationHandler(var2str(MOUSE_CALLBACKS::drawingRegion()),"Botao direito pressionado.");
	}
}