#include "stdafx.h"
#include "utils.h"
#include "opencv.h"

void errorHandler(std::string functionName, std::string errorInfo) {
	std::streambuf *backup = NULL;
	std::ofstream file;

	if (!error_log.empty()) {
		file.open(error_log, std::ofstream::out | std::ofstream::app);
		backup = std::cout.rdbuf();
		std::cout.rdbuf(file.rdbuf());
	}

	char timeString[50];

	struct tm actual_time;
	time_t raw_time;

	time(&raw_time);

	localtime_s(&actual_time, &raw_time);

	strftime(timeString,50,"%d/%m/%Y - %X\n",&actual_time);

	std::cout << timeString << "\t" << functionName << std::endl << "\t\t" << errorInfo << std::endl << std::endl;

	if(file.is_open()) {
		std::cout.rdbuf(backup);
		file.close();
		std::cout << "Um erro ocorreu. O programa sera encerrado.\n\nPor favor abra o arquivo '" << error_log << "' para verificar o que houve.\n\n";
		notificationHandler(functionName,"O programa encerrou com erro.");
		cvDestroyAllWindows();
		system("PAUSE");
		exit(-1);
	}
	else {
		std::cout << "Um erro ocorreu. O programa sera encerrado.\n\n";
		notificationHandler(functionName,"O programa encerrou com erro.");
		cvDestroyAllWindows();
		system("PAUSE");
		exit(-1);
	}
}

void notificationHandler(std::string functionName, std::string notificationInfo) {
	if(notification) {
		std::ofstream file(NOTIFICATION_FILE, std::ofstream::out | std::ofstream::app);

		char timeString[50];

		struct tm actual_time;
		time_t raw_time;

		time(&raw_time);

		localtime_s(&actual_time, &raw_time);

		strftime(timeString,50,"%d/%m/%Y - %X\n",&actual_time);

		file << timeString << "\t" << functionName << std::endl << "\t\t" << notificationInfo << std::endl << std::endl;

		file.close();
	}
}
