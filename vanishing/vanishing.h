
#ifndef VAN_H   
#define VAN_H

#include <iostream>
#include <opencv2/opencv.hpp>

#include <vector>
#include <fstream>

#include <string.h> 

#include <dirent.h> 
#include <stdio.h> 

#include <sys/stat.h>

#include "lsd/lsd.h"

using namespace std;
using namespace cv;

Point vanishingPoint(Mat frame);

class Segment{
	public:
		double x1,x2,y1,y2,p,w,logNfa;
		Segment(double* points){
			x1 = points[0];
			y1 = points[1];
			x2 = points[2];
			y2 = points[3];
			w = points[4];
			p = points[5];
			logNfa = points[6];
		}
};

#endif