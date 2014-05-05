
#ifndef VAN_H   
#define VAN_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <math.h>
#include <string.h> 
#include <dirent.h> 
#include <stdio.h>
#include <sys/stat.h>

#include "lsd/lsd.h"

using namespace std;
using namespace cv;

Point vanishingPoint(Mat frame,Point van);

#endif