#include "stdafx.h"
#include "OCVMenu.h"
#include "utils.h"


OCVMenu::OCVMenu(void)
{
}


OCVMenu::~OCVMenu(void)
{
}

cv::Mat OCVMenu::displayMenu(const unsigned _x) {
	cv::Mat temp = background.clone();
	for(unsigned x = 0; x < options.size(); x++) {
		if (_x != x) {
			cv::putText(temp,options[x],cv::Point(optionsPos[x].tl().x + LETTER_BOX_BORDER, optionsPos[x].br().y - LETTER_BOX_BORDER),FONTFACE,FONTSCALE,letterColor,FONTTHICKNESS);
			if(boxColor[0] >= 0)
				cv::rectangle(temp,optionsPos[x],boxColor,2);
		}
		else {
			cv::putText(temp,options[x],cv::Point(optionsPos[x].tl().x + LETTER_BOX_BORDER, optionsPos[x].br().y - LETTER_BOX_BORDER),FONTFACE,FONTSCALE,activeColor,FONTTHICKNESS);
			if(boxColor[0] >= 0)
				cv::rectangle(temp,optionsPos[x],activeColor,2);
		}
	}
	return temp;
}

void OCVMenu::addOption(const std::string option, const cv::Point optionPos, void *function) {
	cv::Rect region(cv::Point(optionPos.x - LETTER_BOX_BORDER , optionPos.y + LETTER_BOX_BORDER),
		cv::Point(optionPos.x + LETTER_BOX_BORDER + option.length()*LETTER_HORIZONTAL_SPACE, optionPos.y - LETTER_BOX_BORDER - LETTER_VERTICAL_SPACE));
	
	for(unsigned x = 0; x < options.size(); x++) {
		if(optionsPos[x].contains(region.br()) || optionsPos[x].contains(region.tl()) || optionsPos[x].contains(cv::Point(region.tl().x+region.width,region.tl().y)) || optionsPos[x].contains(cv::Point(region.br().x - region.width,region.br().y)) )
			errorHandler(var2str(OCVMenu::addOption()),"Option added conflicts with another option's area.");
	}

	options.push_back(option);
	optionsPos.push_back(region);
	action.push_back((void (*)()) function);
}

void OCVMenu::deleteOption(const std::string option) {
	std::vector<std::string> tempOp;
	std::vector<cv::Rect> tempPos;
	std::vector<void (*)()> tempFunc;
	for (unsigned x = 0; x < options.size(); x++) {
		if(options[x] != option) {
			tempOp.push_back(options[x]);
			tempPos.push_back(optionsPos[x]);
			tempFunc.push_back(action[x]);
		}
	}
	options.clear();
	optionsPos.clear();
	action.clear();
	options = tempOp;
	optionsPos = tempPos;
	action = tempFunc;
	tempOp.clear();
	tempPos.clear();
	tempFunc.clear();
}

unsigned OCVMenu::findOption(cv::Point pos) {
	for(unsigned x = 0; x < optionsPos.size(); x++) {
		if(optionsPos[x].contains(pos))
			return x;
	}
	return -1;
}

OCVMenu& OCVMenu::operator=(OCVMenu &src) {
	activeColor = src.activeColor;
	background = src.background.clone();
	boxColor = src.boxColor;
	letterColor = src.letterColor;
	options = src.options;
	optionsPos = src.optionsPos;
	windowName = src.windowName;
	return src;
}

void buttonActivation(int event, int x, int y, int flag, void* userdata) {
	OCVMenu *Menu = (OCVMenu*)userdata;
	if(event == CV_EVENT_MOUSEMOVE) {
		cv::imshow(Menu->getWindowName(),Menu->displayMenu(Menu->findOption(cv::Point(x,y))));
	}
	if(event == CV_EVENT_LBUTTONDOWN) {
		unsigned z = Menu->findOption(cv::Point(x,y));
		if (z != -1) {
			Menu->callFunction(z);
			cv::imshow(Menu->getWindowName(),Menu->displayMenu());
		}
	}
}