#pragma once

#ifndef OCVMENU_H
#define OCVMENU_H

#include "opencv.h"

#define FONTFACE 4
#define FONTSCALE 1.6
#define FONTTHICKNESS 2
#define LETTER_VERTICAL_SPACE 35
#define LETTER_HORIZONTAL_SPACE 30
#define LETTER_BOX_BORDER 8

class OCVMenu
{
	std::string windowName;
	cv::Mat background;
	cv::Scalar boxColor;
	cv::Scalar letterColor;
	cv::Scalar activeColor;
	std::vector<std::string> options;
	std::vector<cv::Rect> optionsPos;
	std::vector<void (*)()> action;
public:
	OCVMenu(void);
	~OCVMenu(void);

	inline void setWindowName(const std::string _name) {windowName = _name;};
	inline void loadBackground(const cv::Mat &_background) {background = _background.clone();};
	inline void setBoxColor(const cv::Scalar &_boxColor) {boxColor = _boxColor;};
	inline void setLetterColor(const cv::Scalar &_letterColor) {letterColor = _letterColor;};
	inline void setActiveColor(const cv::Scalar &_activeColor) {activeColor = _activeColor;};
	inline void callFunction(const unsigned x) {if(action[x] != NULL) (*action[x])();};
	
	std::string getWindowName() {return windowName;};

	cv::Mat displayMenu(const unsigned _x = -1);

	void addOption(const std::string option, const cv::Point optionPos, void *function = NULL);
	void deleteOption(const std::string option);
	unsigned findOption(cv::Point pos);

	OCVMenu& operator=(OCVMenu &src);
};

void buttonActivation(int event, int x, int y, int flag, void* userdata);

#endif /*OCVMENU_H*/