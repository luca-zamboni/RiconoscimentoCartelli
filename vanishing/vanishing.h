
#ifndef VAN_H   
#define VAN_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>
#include <string.h> 
#include <stdio.h>

#include "lsd/lsd.h"

using namespace std;
using namespace cv;

class VanPoint{

	public:

		Point van;
		VanPoint();
		Point vanishingPoint(Mat frame);
		void clean();

	private:

		class WPoint{
			public:
				Point p;
				double w;
				WPoint(Point p,double w);
		};

		class Segment{
			public:
				Segment(double* points);
				void calc();
				double x1,x2,y1,y2,p,w,logNfa,xmp,ymp,slopeDeg,slNorm;
		};

		const float RATE_M_LENGHT = 0.1;
		float minLength;
		int minAlpha = 10;
		int maxAlpha = 70;
		bool ctrlSegment(Segment s,double xmax,double ymax);
		WPoint getWIntesection(Segment sx, Segment dx);

};

#endif