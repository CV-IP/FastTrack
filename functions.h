/**############################################################################################
								    main.cpp
								    Purpose: functions header for the functions.cpp

								    @author Benjamin GALLOIS
										@email benjamin.gallois@upmc.fr
										@website benjamin-gallois.fr
								    @version 2.0
										@date 2017
###############################################################################################*/

/*
     .-""L_        		     .-""L_
;`, /   ( o\ 					;`, /   ( o\
\  ;    `, /   				\  ;    `, /
;_/"`.__.-"						;_/"`.__.-"

     .-""L_        		     .-""L_
;`, /   ( o\ 					;`, /   ( o\
\  ;    `, /   				\  ;    `, /
;_/"`.__.-"						;_/"`.__.-"


*/


#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <math.h>
#include <numeric>
#include <stdlib.h>

using namespace cv;
using namespace std;


double Moment(Mat image, int p, int q);


Point2f CurvatureCenter(Point3f tail, Point3f head);


double Curvature(Point2f center , Mat image);


double Modul(double angle);


vector<double> Orientation(Mat image, bool dir);


vector<Point3f> Reassignment(vector<Point3f> inputPrev, vector<Point3f> input, vector<int> assignment);


Mat BackgroundExtraction(vector<String> files, double n);


void Registration(Mat imageReference, Mat frame);


void Binarisation(Mat frame, char backgroundColor, int value);


vector<vector<Point3f>> ObjectPosition(Mat frame, int minSize, int maxSize);


vector<int> CostFunc(vector<Point3f> prevPos, vector<Point3f> pos, const double LENGHT, const double ANGLE, const double WEIGHT, const double LO);

vector<Point3f> Prevision(vector<Point3f> past, vector<Point3f> present);


vector<Point3f> Color(int number);
#endif
