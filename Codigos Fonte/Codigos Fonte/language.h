#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "stdafx.h"
#include <map>
#include <string>


///Enumerator for texts to be used
enum E_TEXT
{
	trademark,
	ending,
	license_success,
	license_error1,
	license_error2,
	license_error3,
	license_error4,
	license_error5
};

//Table for all languages output. Must be defined at main.cpp
extern std::map<int,std::string> texts;

//Function to initialize the table. Returns number of languages supported.
void initialize_table(std::string lan);

#endif /*LANGUAGE_H*/