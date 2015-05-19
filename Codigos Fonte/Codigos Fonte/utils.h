#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#define var2str(x) #x
#define NOTIFICATION_FILE "events.log"

extern std::string error_log;	//<Error log file name. Must be defined on the main.cpp of the program.
extern bool notification;		//<Notification enabler. Must be defined on the main.cpp of the program.

//!Function that prints the error to the error_log file.
void errorHandler(std::string functionName, std::string errorInfo);

//!Function that prints the notifications of the program.
void notificationHandler(std::string functionName, std::string errorInfo);

#endif /*UTILS_H*/