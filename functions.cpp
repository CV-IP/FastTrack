/**############################################################################################
								    functions.cpp
								    Purpose: Function use in the main.cpp

								    @author Benjamin GALLOIS
										@email benjamin.gallois@upmc.fr
										@website benjamin-gallois.fr
								    @version 2.0
										@date 2017
###############################################################################################*/

/*
     .-""L_        		     .-""L_
;`, /   ( o\ 			;`, /   ( o\
\  ;    `, /   			\  ;    `, /
;_/"`.__.-"				;_/"`.__.-"

     .-""L_        		     .-""L_
;`, /   ( o\ 			;`, /   ( o\
\  ;    `, /   			\  ;    `, /
;_/"`.__.-"				;_/"`.__.-"


*/



#include "functions.h"
#include "Hungarian.h"

using namespace cv;
using namespace std;



/**
  * @Moment Computes the moment of order p, q of an image.
  * @param Mat image: binary image CV_8U
	* @param int p: order p of the moment
	* @param int q: order p of the moment
  * @return double: moment of the image
*/
double Moment(Mat image, int p, int q) {

	double m = 0;

	for (int j = 0; j<image.rows; ++j){
		for (int i = 0; i<image.cols; ++i){
			m += pow(i, p) * pow(j, q) * image.at<uchar>(j,i);
		}
	}
   return m;
}




/**
  * @CurvatureCenter Computes the center of the curvature defined as the intersection of the minor axis of the head ellipse with the minor axis of the tail ellipse.
  * @param Point3f tail: parameter of the tail x, y and angle of orientation
	* @param Point3f tail: parameter of the head x, y and angle of orientation
  * @return Point2f: coordinates of the center of the curvature
*/
Point2f CurvatureCenter(Point3f tail, Point3f head){

	Point2f center;

	Point p1 = Point(tail.x + 10*cos(tail.z + 0.5*M_PI), tail.y + 10*sin(tail.z + 0.5*M_PI)); 
	Point p2 = Point(head.x + 10*cos(head.z + 0.5*M_PI), head.y + 10*sin(head.z + 0.5*M_PI)); 

	double a = (tail.y - p1.y)/(tail.x - p1.x);
	double c = (head.y - p2.y)/(head.x - p2.x);
	double b = tail.y - a*tail.x;
	double d = head.y - c*head.x;

	if(a*b == 0){ // Determinant == 0, no unique solution
		center = Point(NAN, NAN);
	}

	else{ // Unique solution
		center = Point((b + d)/(c - a), a*((b +d)/(c - a)) + b);
	}

   return center;
}




/**
  * @Curvature Computes the radius of curvature of the fish defined as the inverse of the mean distance between each pixel of the fish and the center of the curvature.
  * @param Point2f center: center of the curvature
	* @param mat image: binary image CV_8U
  * @return double: radius of curvature
*/
double Curvature(Point2f center , Mat image){

	double d = 0;
	double count = 0;

	for(int row = 0; row<image.rows; ++row){
		for(int col = 0; col<image.cols; ++col){
			if(image.at<uchar>(row,col) == 255){ // if inside object
				d += pow(pow(center.x - float(col), 2) + pow(center.y - float(row), 2), 0.5);
				count += 1;
			}
		}
	}
	return count/d;
}



/**
  * @Modul usual modulo 2*PI of an angle.
  * @param double angle: input angle
  * @return double: output angle
*/
double Modul(double angle)
{
    return angle - 2*M_PI * floor(angle/(2*M_PI));
}




/**
  * @Orientation computes the equivalente ellipse of an object and it direction
  * @param Mat image: binary image CV_8U
  * @param bool dir: if true computes the orientation, if false the orientation is undetermined at +/- PI
  * @return vector<double>: [x, y, orientation]
*/
vector<double> Orientation(Mat image, bool dir) {

	double moo = Moment(image, 0, 0);

	double x = Moment(image, 1, 0)/moo;
	double y = Moment(image, 0, 1)/moo;

	double i = Moment(image, 2, 0)/moo - pow(x, 2);
	double j = Moment(image, 1, 1)/moo - x*y;
	double k = Moment(image, 0, 2)/moo - pow(y, 2);

	double orientation = 0.5 * atan((2*j)/(i-k)) + (i<k)*(M_PI*0.5);
	orientation += 2*M_PI*(orientation<0);
	orientation = 2*M_PI - orientation;
	double orientationDeg = (orientation*180)/M_PI;


	if(dir == true){ // Orientation computation
		//Two methods available here to compute the direction

		//////////////////////With a rotation of image/////////////////////////////////////
		Mat rotate;
		Point center = Point(image.cols*0.5, image.rows*0.5);
		Mat rotMatrix = getRotationMatrix2D(center, -orientationDeg, 1);
		Rect bbox = RotatedRect(center, image.size(), -orientationDeg).boundingRect();
		rotMatrix.at<double>(0,2) += bbox.width*0.5 - center.x; //add an off set
	    rotMatrix.at<double>(1,2) += bbox.height*0.5 - center.y;// to rotate without cropping the frame
		warpAffine(image, rotate, rotMatrix, bbox.size());

		vector<double> tmpMat;
		vector<double> tmp;


		for (int j = 0; j<rotate.cols; ++j){
		double s = 0;
			for (int i = 0; i<rotate.rows; ++i){
			  s += rotate.at<uchar>(i, j);
			}
			tmpMat.push_back((double)s);
		}

		double ccMax = *max_element(tmpMat.begin(), tmpMat.end())/100;
		for (unsigned int it = 0; it < tmpMat.size(); ++it){
			int cc = tmpMat.at(it);
			for (int jt = 0; jt < cc/ccMax; ++jt){
			  tmp.push_back((double)(it+1));
			}
		}


		double mean = accumulate(tmp.begin(), tmp.end(), 0)/(double)tmp.size();


		double sd = 0 , skew = 0;

		for(unsigned int it = 0; it < tmp.size(); ++it){
	        sd += pow(tmp.at(it) - mean, 2);
					skew += pow(tmp.at(it) - mean, 3);
		}

		sd = pow(sd/((double)tmp.size()-1), 0.5);
		skew *= (1/(((double)tmp.size() - 1)*pow(sd, 3)));


		if(skew > 0){
			orientation -= M_PI;
			orientation = Modul(orientation);
		}
	}

	vector<double> param {x, y, orientation};

	return param;
}




/**	
  * @BackgroundExtraction extracts the background of a film with moving object by projection
  * @param vector<String> files: array with the path of images
  @param double n: number of frames to average 
  * @return Mat: background image of a movie
*/
UMat BackgroundExtraction(vector<String> files, double n){

    UMat background;
    UMat img0;
    imread(files[0], IMREAD_GRAYSCALE).copyTo(background);
    imread(files[0], IMREAD_GRAYSCALE).copyTo(img0);
    background.convertTo(background, CV_32F);
    img0.convertTo(img0, CV_32FC1);
    Rect registrationFrame(0, 0, 200, 50);
    int step = files.size()/n;
    //Mat tmp = imread(files[0], IMREAD_GRAYSCALE);
    UMat tmp;
    UMat cameraFrameReg;
    Mat H;
    UMat identity = UMat::ones(background.rows, background.cols, CV_32F);

	for(unsigned int i = 1; i < files.size(); i += step){
        //tmp = imread(files[i], IMREAD_GRAYSCALE);
        imread(files[i], IMREAD_GRAYSCALE).copyTo(tmp);
		tmp.convertTo(tmp, CV_32FC1);
        cameraFrameReg = tmp(registrationFrame);
		img0 = img0(registrationFrame);
        Point2d shift = phaseCorrelate(cameraFrameReg, img0);
        H = (Mat_<float>(2, 3) << 1.0, 0.0, shift.x, 0.0, 1.0, shift.y);
		warpAffine(tmp, tmp, H, tmp.size());
		accumulate(tmp, background);
	}
    //background /= (n-1);
    multiply(background, identity, background, 1/(n-1));
    background.convertTo(background, CV_8U);

	return background;
}




/**
  * @Registration makes the registration of a movie by phase correlation
  * @param Mat imageReference: reference image for the registration, one channel
	* @param Mat frame: image to register
*/
void Registration(UMat imageReference, UMat frame){

	//Rect registrationFrame(0, 0, 500, 500);
	frame.convertTo(frame, CV_32FC1);
	imageReference.convertTo(imageReference, CV_32FC1);
	//imageReference = imageReference(registrationFrame);
	//Mat frameReg = frame(registrationFrame);

	Point2d shift = phaseCorrelate(frame, imageReference);
    Mat H = (Mat_<float>(2, 3) << 1.0, 0.0, shift.x, 0.0, 1.0, shift.y);
	warpAffine(frame, frame, H, frame.size());
}




/**
  * @Binarisation binarizes the image by an Otsu threshold
  * @param Mat frame: image to binarized
	* @param char backgroundColor: 'b' if the background is black, 'w' is the background is white
*/
void Binarisation(UMat frame, char backgroundColor, int value){

    frame.convertTo(frame, CV_8U);

	if(backgroundColor == 'b'){
		//threshold(frame, frame, 0, 255, CV_THRESH_BINARY| CV_THRESH_OTSU);
        threshold(frame, frame, value, 255, CV_THRESH_BINARY);
	}

	if(backgroundColor == 'w'){
        threshold(frame, frame, value, 255, CV_THRESH_BINARY_INV);
	}
}




/**
  * @ObjectPosition Computes positions of multiples objects of size between min and max size by finding contours
  * @param Mat frame: binary image CV_8U
	* @param int minSize: minimal size of the object
	* @param int maxSize: maximal size of the object
  * @return vector<vector<Point3f>>: {head parameters, tail parameters, global parameter}, {head/tail parameters} = {x, y, orientation}, {global parameter} = {curvature, 0, 0}
*/
vector<vector<Point3f>> ObjectPosition(Mat frame, int minSize, int maxSize){

	vector<vector<Point> > contours;
	vector<Point3f> positionHead;
	vector<Point3f> positionTail;
    vector<Point3f> positionFull;
	vector<Point3f> globalParam;
	Mat dst;

	findContours(frame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);



	for (unsigned int i = 0; i < contours.size(); i++){

			if(contourArea(contours[i]) > minSize && contourArea(contours[i]) < maxSize){ // Only select objects minArea << objectArea <<maxArea
				
				dst = Mat::zeros(frame.size(), CV_8U);
				drawContours(dst, contours, i, Scalar(255, 255, 255),CV_FILLED,8); // Draw the fish in a temporary black image,avoid select a part of another fish if two fish are very close
				
				Rect roiFull = boundingRect(contours[i]);
				Mat RoiFull = dst(roiFull);

				vector<double> parameter;
				parameter = Orientation(RoiFull, true); // In RoiFull coordinates: x, y, orientation
				double angleFull = parameter.at(2);



				Point center = Point(0.5*RoiFull.cols, 0.5*RoiFull.rows);
				Mat rotMatrix = getRotationMatrix2D(center, -(parameter.at(2)*180)/M_PI, 1);
				Mat rotate;
				Rect bbox = RotatedRect(center, RoiFull.size(), -(parameter.at(2)*180)/M_PI).boundingRect();
				rotMatrix.at<double>(0,2) += bbox.width*0.5 - center.x; //add an off set
    			rotMatrix.at<double>(1,2) += bbox.height*0.5 - center.y;// to rotate without cropping the frame
				warpAffine(RoiFull, rotate, rotMatrix, bbox.size());

				Mat p = (Mat_<double>(3,1) << parameter.at(0), parameter.at(1), 1);
				
				Mat pp = rotMatrix * p; // center of mass of fish in  the rotated coordinate system


				// Head ellipse
				Rect roiHead(pp.at<double>(0,0), 0, rotate.cols-pp.at<double>(0,0), rotate.rows);
				Mat RoiHead = rotate(roiHead);
				vector<double> parameterHead = Orientation(RoiHead, false); // In RoiHead coordinates: xHead, yHead, orientationHead

				// Tail ellipse
				Rect roiTail(0, 0, pp.at<double>(0,0), rotate.rows);
				Mat RoiTail = rotate(roiTail);
				vector<double> parameterTail = Orientation(RoiTail, false); // In RoiHead coordinates: xHead, yHead, orientationHead


				invertAffineTransform(rotMatrix, rotMatrix);

				p = (Mat_<double>(3,1) << parameterHead.at(0) + roiHead.tl().x,parameterHead.at(1) + roiHead.tl().y, 1);

				pp = rotMatrix * p;

				double xHead = pp.at<double>(0,0) + roiFull.tl().x;
				double yHead = pp.at<double>(1,0) + roiFull.tl().y;

				double angleHead = parameterHead.at(2) - M_PI*(parameterHead.at(2) > M_PI);
				angleHead = Modul(angleHead + angleFull + M_PI*(abs(angleHead) > 0.5*M_PI)); // Computes the direction
				 


				p = (Mat_<double>(3,1) << parameterTail.at(0) + roiTail.tl().x, parameterTail.at(1) + roiTail.tl().y, 1);

				pp = rotMatrix * p;


				double xTail = pp.at<double>(0,0) + roiFull.tl().x;
				double yTail = pp.at<double>(1,0) + roiFull.tl().y;

				double angleTail = parameterTail.at(2) - M_PI*(parameterTail.at(2) > M_PI);
				angleTail = Modul(angleTail + angleFull + M_PI*(abs(angleTail) > 0.5*M_PI)); // Computes the direction



				//Curvature
				double curv = 1/1e-16;
				Point2f radiusCurv = CurvatureCenter(Point3f(xTail, yTail, angleTail), Point3f(xHead, yHead, angleHead));
				if(radiusCurv.x != NAN){ // 
					curv = Curvature(radiusCurv, RoiFull);
				}


				positionHead.push_back(Point3f(xHead, yHead, angleHead));
				positionTail.push_back(Point3f(xTail, yTail, angleTail));
                positionFull.push_back(Point3f(parameter.at(0) + roiFull.tl().x, parameter.at(1) + roiFull.tl().y, parameter.at(2)));
				globalParam.push_back(Point3f(curv, 0, 0));
			}
		}

        vector<vector<Point3f>> out = {positionHead, positionTail, positionFull, globalParam};
		return out;
}





/**
  * @CostFunc computes the cost function and use a global optimization association to associate target between frame. Method adapted from: "An
							effective and robust method for tracking multiple fish in video image based on fish head detection" YQ Chen et al.
							Use the Hungarian method implemented by Cong Ma, 2016 "https://github.com/mcximing/hungarian-algorithm-cpp" adapted from the matlab
							implementation by Markus Buehren "https://fr.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-assignment-problem".
  * @param vector<Point3f> prevPos: sorted vector of object parameters,vector of points (x, y, orientation).
	* @param vector<Point3f> pos: non-sorted vector of object parameters,vector of points (x, y, orientation) that we want to sort accordingly to prevPos to identify each object.
	* @param double length: maximal displacement of an object between two frames.
	* @param double angle: maximal difference angle of an object direction between two frames.
	* @return vector<int>: the assignment vector of the new index position.
*/
vector<int> CostFunc(vector<Point3f> prevPos, vector<Point3f> pos, const double LENGTH, const double ANGLE, const double WEIGHT, const double LO){


	int n = prevPos.size();
	int m = pos.size();
	double c = -1;
	vector<vector<double>> costMatrix(n, vector<double>(m));

	for(int i = 0; i < n; ++i){

		Point3f prevCoord = prevPos.at(i);
		for(int j = 0; j < m; ++j){
			Point3f coord = pos.at(j);
            double d = pow(pow(prevCoord.x - coord.x, 2) + pow(prevCoord.y - coord.y, 2), 0.5);
            if(d < LO){
                c = WEIGHT*(d/LENGTH) + (1 - WEIGHT)*((abs(Modul(prevCoord.z - coord.z + M_PI) - M_PI))/(ANGLE)); //cost function
				costMatrix[i][j] = c;
			}
            else if (d > LO){
				costMatrix[i][j] = 2e53;
			}
			
		}
	}

	// Hungarian algorithm to solve the assignment problem O(n**3)
	HungarianAlgorithm HungAlgo;
	vector<int> assignment;
	HungAlgo.Solve(costMatrix, assignment);

	return assignment;
}




/**
  * @Reassignment Resamples a vector accordingly to a new index.  
  * @param vector<Point3f> output: output vector of size n
  * @param vector<Point3f> input: input vector of size m <= n
  * @param vector<int> assignment: vector with the new index that will be used to resample the input vector
  * @return vector<Point3f>: output vector of size n.
*/
vector<Point3f> Reassignment(vector<Point3f> output, vector<Point3f> input, vector<int> assignment){

	vector<Point3f> tmp = output;
	unsigned int n = output.size();
	unsigned int m = input.size();


	if(m == n){ // Same number of targets in two consecutive frames
		for(unsigned int i = 0; i < n; i++){
			tmp.at(i) = input.at(assignment.at(i));
		}
	}

	else if(m > n){// More target in current frame than in previous one
		for(unsigned int i = 0; i < n; i++){
			tmp.at(i) = input.at(assignment.at(i));
		}
	}

	else if(m < n){// Fewer target in current frame than in the previous one
		for(unsigned int i = 0; i < n; i++){
			if(assignment.at(i) != -1){
				tmp.at(i) = input.at(assignment.at(i));
			}
		}
	}

	else{
		cout << "association error" << '\n';
	}

	input = tmp;

	return input;
}




/**
  * @Reassignment Resamples a vector accordingly to a new index.  
  * @param vector<Point3f> output: output vector of size n
  * @param vector<Point3f> input: input vector of size m <= n
  * @param vector<int> assignment: vector with the new index that will be used to resample the input vector
  * @return vector<Point3f>: output vector of size n.
*/
vector<Point3f> Prevision(vector<Point3f> past, vector<Point3f> present){

	double l = 0;
	for(unsigned int i = 0; i < past.size(); i++){
		if(past.at(i) != present.at(i)){
			l = pow(pow(past.at(i).x - present.at(i).x, 2) + pow(past.at(i).y - present.at(i).y, 2), 0.5);
			break;
		}
	}



	for(unsigned int i = 0; i < past.size(); i++){
		if(past.at(i) == present.at(i)){
			present.at(i).x += l*cos(present.at(i).z);
			present.at(i).y -= l*sin(present.at(i).z);
		}
	}
	return present;
}





/**
  * @Color computes a color map.
  * @param int number: number of colors
	
  * @return vector<Point3f>: RGB color
*/
vector<Point3f> Color(int number){

	double a, b, c;
	vector<Point3f> colorMap;
	srand (time(NULL));
	for (int j = 0; j<number; ++j){
		a = rand() % 255;
		b = rand() % 255;
		c = rand() % 255;

		colorMap.push_back(Point3f(a, b, c));
	}

	return colorMap;
}



