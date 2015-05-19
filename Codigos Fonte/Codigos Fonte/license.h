#ifndef LICENSE_H
#define LICENSE_H

#include "windows.h"
#include <cmath>

#define GUID_LENGTH 50

enum OS_E {
	OS_32BIT,
	OS_64BIT
};

enum LICENSE_ERROR_E {
	INVALID_OSARCH = -1,
	UNKNOWN = -2,
	NO_ARCHIVE = -3,
	PATH_NOT_FOUND = -4
};

//Function to enable the program. Veryfies if the program has a valid license.
bool enableKey(int lic, unsigned OSArch);

//Functions to generate a license.
int generateKey(char *guid);
int generateKey(unsigned OSArch);

//Algorithm to transform the hardware/software information.
int transformLic(unsigned char *reg, int size);

//Finds guid and transforms it in to a vector of char.
char* guid2char_WIN(unsigned OSArch);

#endif /*LICENSE_H*/