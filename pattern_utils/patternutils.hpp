
#ifndef PATTERNUTILS_HPP 
#define PATTERNUTILS_HPP

#define _USE_MATH_DEFINES

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>
#include <string.h> 
#include <stdio.h>

using namespace std;
using namespace cv;

const int RESIZE = 150;

// calcola la differenza
double diffXorMat(Mat mat1,Mat mat2);
Mat setWhiteBlack(Mat mat);
Mat cutBorderByColor(Mat mat,int color,bool there);

#endif