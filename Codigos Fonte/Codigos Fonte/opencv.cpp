#include "stdafx.h"
#include "utils.h"
#include "opencv.h"

void filter_strip(cv::Mat &src, cv::Mat &dst, int szWidth, int szHeight, int deltaX, int deltaY) {
	cv::Size siz(szWidth,szHeight);
	GaussianBlur(src,dst,siz,(deltaX/100.0),(deltaY/100.0));

	std::stringstream note;
	note << "'szWidth' = " << szWidth << "\n\t\t'szHeight' = " << szHeight << "\n\t\t'deltaX' = " << deltaX << "\n\t\t'deltaY' = " << deltaY;
	notificationHandler(var2str(filter_strip()),note.str());

	return;
}

void filter_square(cv::Mat &src, cv::Mat &dst, int size, int delta) {
	cv::Size siz(size,size);
	GaussianBlur(src,dst,siz,delta/100.0);
	std::stringstream note;
	note << "'size' = " << size << "\n\t\t'delta' = " << delta;
	notificationHandler(var2str(filter_square()),note.str());
	return;
}

bool region_adaptiveThreshold(cv::Mat &src, cv::Mat &dst, std::vector<cv::Point> matchingShape, int block_sz, int C) {
	if (src.type() != CV_8UC1) {
		dst = cv::Mat::zeros(src.rows,src.cols,src.type());
		errorHandler(var2str(region_adaptiveThreshold()),"'src' deveria ser do tipo CV_8UC1.");
		return false;
	}
	
	cv::Mat temp;
	bool crescent = true;
	int var = C,maxC = 0, minC = 0;
	std::vector<std::vector<cv::Point>> contours;

	adaptiveThreshold(src,temp,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,C/100);
	findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

	zero:
		// If there is no regions you have to go back, because your threshold is too high.
		if (contours.size() == 0) {				
			crescent = false;			//We're saying that we're going back to lower thresholds.
			if (var == 0) {				//If we are at our minimal threshold there is no regions to find.
				dst = cv::Mat::zeros(src.rows,src.cols,src.type());
				notificationHandler(var2str(region_adaptiveThreshold()),"Threshold ja esta no minimo e nao ha regioes (Nada foi detectado).");
				return false;
			}
			var--;						//Decreasing threshold.
			adaptiveThreshold(src,temp,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,var/100);
			contours.clear();
			findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
			goto zero;
		}
		//If there are more than one regions, we might have a threshold too high or to low.
		else if (contours.size() > 1) {
			//If the function is ascending, we increase the threshold. Otherwise we decreased it.
			if (crescent)
				var++;
			else
				var--;
			adaptiveThreshold(src,temp,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,var/100);
			contours.clear();
			findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
			goto zero;
		}
		//In case we have only one region, we have to analyse what to do.
		else if (contours.size() == 1) {
			//If we were decreasing and we found one region, we have our maximum threshold.
			if (!crescent) {
				maxC = var;				//Maximum threshold
				//Now we can find our minimal threshold. We keep decreasing the threshold until we have regions different than 1
				while(contours.size() == 1) {
					var--;
					adaptiveThreshold(src,temp,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,var/100);
					contours.clear();
					findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
				}
				minC = var++;			//Minimum threshold
			}
			//If we are increasing the threshold and we find one region, we might be at the start of the program or at our minimal threshold
			//So to be sure we increase the threshold until we loose this region and then we do the inverse process.
			else {
				while(contours.size() == 1) {
					var++;
					adaptiveThreshold(src,temp,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,var/100);
					contours.clear();
					findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
				}
				crescent = false;			//We have to specify that we are going backwards now.
				goto zero;
			}
		}
		//If we have negative regions, something is wrong.
		else {
			dst = cv::Mat::zeros(src.rows,src.cols,src.type());
			errorHandler(var2str(region_adaptiveThreshold()),"Foi encontrado um numero de regios negativo.");
			return false;
		}

	double area, best = 0, match, bestMatch = 10;
	
	if(matchingShape.empty()) {
		dst = cv::Mat::zeros(src.rows,src.cols,src.type());
		errorHandler(var2str(region_adaptiveThreshold()),"'matchingShape' esta vazio.");
		return false;
	}

	//Goes through all our possible thresholds.
	for(int x = minC; x <= maxC; x++) {
		adaptiveThreshold(src,temp,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,x/100);
		contours.clear();
		findContours(temp,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
		area = contourArea(contours[0]);								//Area of the contour
		match = matchShapes(contours[0],matchingShape,CV_CONTOURS_MATCH_I2,0);	//Match Shape of the contour with the rectangle.
		if(area > best && match < bestMatch && match != 0) {			//Pick the best threshold, by form and size.
			best = area;
			bestMatch = match;
			var = x;
		}
	}
	//If there isn't any thresholds that were good enough, the function fails.
	if (best == 0) {
		dst = cv::Mat::zeros(src.rows,src.cols,src.type());
		notificationHandler(var2str(region_adaptiveThreshold()),"Regioes foram encontradas, mas nenhuma satisfaz as condicoes minimas.");
		return false;
	}

	adaptiveThreshold(src,dst,WHITE_GRAY,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,block_sz,var/100);
	notificationHandler(var2str(region_adaptiveThreshold()),"Funcao encontrou regiao com sucesso.");
	return true;
}

bool region_Canny(cv::Mat &src, cv::Mat &dst, int threshold1, int threshold2) {
	if (src.type() != CV_8UC1) {
		dst = cv::Mat::zeros(src.rows,src.cols,src.type());
		errorHandler(var2str(region_Canny()),"'src' deveria ser do tipo CV_8UC1.");
		return false;
	}

	std::vector<std::vector<cv::Point>> contours;

	//Draw the borders using the Canny() function
	Canny(src,dst,threshold1,threshold2);

	//Find the contours of the borders
	findContours(dst,contours,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

	//Erases 'dst'
	dst = cv::Mat::zeros(src.rows,src.cols,src.type());

	//Draw the first contour (the most probable match) and paints the interior.
	drawContours(dst,contours,0,cv::Scalar(255),CV_FILLED);

	notificationHandler( var2str(region_Canny()),"Funcao encontrou regiao com sucesso.");
	return true;
}

bool region_custom(cv::Mat &src, cv::Mat &dst, std::vector<int> values, const int rowBusca, const int colBusca, double approximation) {
	//Image must be of specified type (3 channel)
	if (src.type() != CV_8UC3) {
		dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
		errorHandler(var2str(region_custom()),"'src' deveria ser do tipo CV_8UC3.");
		return false;
	}

	//Size of the auto search must be positive. Determines size of the rectangle that will serve as our region of control.
	if (rowBusca <= 0 || colBusca <= 0) {
		dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
		errorHandler(var2str(region_custom()),"'rowBusca' e/ou 'colBusca' menores ou iguais a zero.");		
		return false;
	}

	//We have 3 channels and minValues and maxValues for each of them.
	if (values.size() < 6) {
		dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
		errorHandler(var2str(region_custom()),"Menos de 6 parametros em 'values'.");		
		return false;
	}
	
	//Depending of the size of the image the jump will be increased otherwise it takes too long.
	int searchX = 1 + src.cols/100,
		searchY = 1 + src.rows/100;

	//Saves the values passed to the function. They are the initial conditions.
	int minHue = values[0], minSat = values[1], minVal = values[2], 
		maxHue = values[3], maxSat = values[4], maxVal = values[5];

	int row,col;
	cv::Mat bin;
	std::vector<std::vector<cv::Point>> bestContour, contours;

	std::vector<cv::Point> rect;
	rect.push_back(cv::Point(0,0));
	rect.push_back(cv::Point(colBusca,0));
	rect.push_back(cv::Point(colBusca,rowBusca));
	rect.push_back(cv::Point(0,rowBusca));
	rect.push_back(cv::Point(0,0));

	//Goes through the rows and columns.
	for(row = 0; row < src.rows - rowBusca; row += searchY) {
		for(col = 0; col < src.cols - colBusca; col += searchX) {
			//Each time we change the position of the 'cursor' we go to our initial conditions.
			values[0] = minHue; values[1] = minSat; values[2] = minVal;
			values[3] = maxHue; values[4] = maxSat; values[5] = maxVal;

			//Loop to find the minimal parameters for the region to be available
			for(int u = 0; u < 6;) {
				inRange(src,cv::Scalar(values[0],values[1],values[2]),cv::Scalar(values[3],values[4],values[5]),bin);
				bin = bin(cv::Range(row,row+rowBusca),cv::Range(col,col+colBusca));

				//If the region is not fully in range of the parameters it means that we've passed the limit,
				//therefore we go back and go to the next parameter
				//Parameter greater than 3 its maxValues
				//Parameter lesser than 3 its minValues
				if(countNonZero(bin) < (rowBusca * colBusca)) {
					if(u < 3 && values[u] > 0)
						values[u]--;
					else if (u >= 3 && values[u] < 255)
						values[u]++;

					u++;
				}

				//If the region is still in range we keep going.
				else {
					if(u < 3)
						values[u]++;
					else
						values[u]--;
				}
			}
			//We process the image with the new optimized values.
			inRange(src,cv::Scalar(values[0],values[1],values[2]),cv::Scalar(values[3],values[4],values[5]),bin);
			bin = bin(cv::Range(row,row+rowBusca),cv::Range(col,col+colBusca));

			//Find the contours on the image processed.
			contours.clear();
			findContours(bin,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

			//If we have only one region detected, we have a potential region to be evaluated. The 'cursor' must be inside this region.
			if(contours.size() == 1 && pointPolygonTest(contours[0],cv::Point(col,row),false) >= 0 ) {
				approxPolyDP(contours[0],contours[0],approximation,true);
				double match = matchShapes(contours[0],rect,CV_CONTOURS_MATCH_I2,0);

				//If we dont have any other possible regions, we start with the current one, if it is not a straight line.
				if(bestContour.empty()) {
					if (match != 0)
						bestContour.push_back(contours[0]);
				}
				//Else we see if the current one is better than our previous saved one, in shape and size.
				else {
					if( match < matchShapes(bestContour[0],rect,CV_CONTOURS_MATCH_I2,0) && 
						match != 0													 &&
						contourArea(contours[0]) > contourArea(bestContour)  		   ) {
							bestContour.clear();
							bestContour.push_back(contours[0]);
					}
				}
			}
		}
	}

	//If we dont have any regions as our best region, we dont have a region.
	if (bestContour.empty()) {
		dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
		notificationHandler(var2str(region_custom()),"Nao foram encontradas nenhuma regiao.");
		return false;
	}
	//Else we return our best region
	else {
		dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
		drawContours(dst,bestContour,0,cv::Scalar(255),CV_FILLED);
		notificationHandler(var2str(region_custom()),"A funcao encontrou regiao com sucesso.");
		return true;
	}

	//We should never reach this part of the code. Just in case.
	dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
	errorHandler(var2str(region_custom()),"Erro inesperado.");	
	return false;
}

double relation(cv::Mat &control, cv::Mat &controlReg, cv::Mat &test, cv::Mat &testReg) {
	//If the regions aren't grayscale images we have an error.
	if(controlReg.type() != testReg.type() != CV_8UC1) {
		errorHandler(var2str(relation()),"Regioes (mascaras) nao sao do tipo CV_8UC1.");
		return -2;
	}

	//The images must have the same type.
	if(control.type() != test.type() ) {
		errorHandler(var2str(relation()),"Imagens do teste e do controle nao sao do mesmo tipo.");
		return -2;
	}

	//The images must be in grayscale or 3-channeled
	if(control.type() != CV_8UC1 && control.type() != CV_8UC3) {
		errorHandler(var2str(relation()),"Imagens do teste e do controle nao sao do tipo CV_8UC1 nem CV_8UC3.");
	}

	//We see if we have contours on our regions, in other words, if we actually have found regions.
	std::vector<std::vector<cv::Point>> contours;
	cv::Mat temp = controlReg.clone();
	findContours(temp,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	//If the control region wasn't found the test can't be validated.
	if(contours.size() == 0) {
		notificationHandler(var2str(relation()),"A regiao de controle nao foi encontrada, entao o teste nao pode ser validado.");
		return -1;
	}

	temp.release();
	contours.clear();
	temp = testReg.clone();
	findContours(temp,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	//If the test region wasn't found we considerate that we have concentration of 0.
	if(contours.size() == 0) {
		notificationHandler(var2str(relation()),"A regiao de teste nao foi encontrada.");
		return 0;
	}

	double testVal,controlVal;
	cv::Scalar mean,stdd;

	//Control evaluation
	meanStdDev(control,mean,stdd,controlReg);
	
	//Depending on how many channels we have to select the right value to use.
	if(control.type() == CV_8UC3)
		controlVal = mean[1] + 2*stdd[1];
	else
		controlVal = mean[0] + 2*stdd[0];

	//Test evaluation
	meanStdDev(test,mean,stdd,testReg);

	//Depending on how many channels we have to select the right value to use.
	if(test.type() == CV_8UC3)
		testVal = mean[1] + 2*stdd[1];
	else
		testVal = mean[0] + 2*stdd[0];
	
	std::stringstream str;
	str << "'controlVal' = " << controlVal << std::endl << "\t\t'testVal' = " << testVal;
	notificationHandler(var2str(relation()),str.str());
	return testVal/controlVal;
}

std::vector<double> meanAndCv(std::vector<double> values) {
	std::vector<double> result, mean, stdd;
	
	//If we have less than 3 values on our vector we return the simple mean and cv.
	if(values.size() < 3) {
		cv::meanStdDev(values,mean,stdd);
		std::stringstream note;
		note << "'mean' = " << mean[0] << "\n\t\t'stdd' = " << stdd[0];
		for(unsigned x = 0; x < values.size(); x++)
			note << "\n\t\t'values'[" << x << "] = " << values[x];
		notificationHandler(var2str(meanAndCv()),note.str());
		result.push_back(mean[0]);
		if(mean[0] != 0)
			result.push_back(100.0*stdd[0]/mean[0]);
		else
			result.push_back(-1);
		return result;
	}

	int min = 0, max = 0;
	std::stringstream note;
	note << "Valores originais:";

	//Finding the min and max value. We assume that the first value is the min and max.
	for(unsigned int x = 1; x < values.size(); x++) {
		if(values[x] < values[min])
			min = x;
		else if (values[x] > values[max])
			max = x;

		note << "\n\t\t\t'values'[" << x << "] = " << values[x];
	}

	note << "\n\n\t\tNovo vetor:";
	
	//We create a new vector of values where we eliminate the min and max values.
	for(unsigned int x = 0; x < values.size(); x++) {
		if (x != min && x != max) {
			result.push_back(values[x]);

			note << "\n\t\t\t'values'[" << x << "] = " << values[x];
		}
	}

	//Calculates mean and cv.
	cv::meanStdDev(result,mean,stdd);

	note << "\n\n\t\t'mean' = " << mean[0] << "\n\t\t'stdd' = " << stdd[0];
	result.clear();
	result.push_back(mean[0]);
	if(mean[0] != 0)
		result.push_back(100.0*stdd[0]/mean[0]);
	else
		result.push_back(-1);
	notificationHandler(var2str(meanAndCv()),note.str());
	return result;
}

std::vector<double> calibrationCurve(std::vector<double> sample, std::vector<double> prediction) {
	std::vector<double> meanSam, meanPred, stddSam, stddPred, result;
	double r2 = 0, A, B;
	
	//Vector must be of same size.
	if (sample.size() != prediction.size()) {
		errorHandler(var2str(calibrationCurve()),"Vetore de valores e o vetor de predicao nao sao do mesmo tamanho.");
		return result;
	}

	//If we have less than 2 values in our vectors we cant estipulate a curve.
	if (sample.size() < 2) {
		errorHandler(var2str(calibrationCurve()),"Vetores possuem apenas um valor. Inclua pelo menos mais um valor.");
		return result;
	}

	cv::meanStdDev(sample,meanSam,stddSam);
	cv::meanStdDev(prediction,meanPred,stddPred);
	
	//Formulae of correlation coefficient 'r'
	//r = sum(Xi - Xm)*sum(Yi - Ym)
	//    -------------------------
	//	  (n - 1)*stddv(X)*stddv(Y)
	for(unsigned int x = 0; x < sample.size(); x++)
		r2 += (sample[x]-meanSam[0])*(prediction[x]-meanPred[0])/((sample.size() - 1)*stddSam[0]*stddPred[0]);

	//r^2
	r2 = r2*r2;

	//Formulae to find the curve y = Ax + B;
	//A = n * sum(Xi*Yi) - sum(Xi)*sum(Yi)
	//    --------------------------------
	//    n * sum(Xi^2)     -    sum(Xi)^2
	double sumXY = 0, sumX = 0, sumY = 0, sumX2 = 0;
	for(unsigned int x = 0; x < sample.size(); x++) {
		sumXY += sample[x]*prediction[x];
		sumX += sample[x];
		sumY += prediction[x];
		sumX2 += sample[x]*sample[x];
	}
	A = (sample.size()*sumXY - sumX*sumY)/(sample.size()*sumX2 - sumX*sumX);

	//B = mean(Y) - A*mean(x)
	B = meanPred[0] - A*meanSam[0];

	result.push_back(A);
	result.push_back(B);
	result.push_back(r2);

	std::stringstream note;
	note << "'r2' = " << r2;
	note << "\n\t\ty = " << A << "*x + " << B;
	notificationHandler(var2str(calibrationCurve()),note.str());
	return result;
}

int search_cameras(const int maxCameras) {
	int x;
	cv::VideoCapture vid;
	
	//Keeps feeding all possible cameras until there are none to be assigned or the maximum number has been hit.
	for(x = 0; x < maxCameras; x++) {
		vid.open(x);
		if (!vid.isOpened())
			break;
		vid.release();
	}

	std::stringstream note;
	note << "'maxCameras' = " << maxCameras << "\n\t\tfoundCameras = " << x;
	notificationHandler(var2str(calibrationCurve()),note.str());

	vid.release();
	return x;
}

bool photo(cv::Mat img, std::string name, std::string imageFormat, int quality) {
	//Parameters to save the image.
	std::vector<int> parameters;

	//Transform the upper case of the name into lower case.
	transform(imageFormat.begin(),imageFormat.end(),imageFormat.begin(),tolower);

	//Editing the name to the right format.
	if(imageFormat == "jpg") {
		name += ".jpg";
		parameters.push_back(CV_IMWRITE_JPEG_QUALITY);
		if (quality < 0)
			quality = 0;
		else if (quality > 100)
			quality = 100;
		parameters.push_back(quality);
	}
	else if (imageFormat == "png") {
		name += ".png";
		parameters.push_back(CV_IMWRITE_PNG_COMPRESSION);
		if (quality < 0)
			quality = 0;
		else if (quality > 9)
			quality = 9;
		parameters.push_back(quality);
	}
	else {
		errorHandler(var2str(photo()),"Formato de imagem nao suportado ou desconhecido.");
		return false;
	}

	//Saving the image.
	if(!imwrite(name,img,parameters)) {
		errorHandler(var2str(photo()),"O programa falhou em salvar a imagem.");
		return false;
	}

	notificationHandler(var2str(photo()),"A imagem foi salva com sucesso.");
	return true;
}