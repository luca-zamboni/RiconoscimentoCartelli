#ifndef RCARTEL_HPP 
#define RCARTEL_HPP 

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <string.h> 
#include <dirent.h> 
#include <stdio.h> 
#include <sys/stat.h>

#include "vanishing/vanishing.h"
#include "pattern_utils/patternutils.hpp"

using namespace cv;
using namespace std;

class RCartel{

	private:
		class ROI_Rect {
			private:
				Rect ROI;
				vector<Point> rect;
			public:
				ROI_Rect(cv::Rect RROI, vector<Point> RRect);
				cv::Rect getROI() {
					return ROI;
				}
				vector<Point> getRect() {
					return rect;
				}
		};

		class GeomSignal {
			private:
				string name;
				Mat image_ROI;
				Rect rect_ROI;
			public:
				GeomSignal(string nname, Mat iimage_ROI, Rect rrect_ROI);
				string getName() {
					return name;
				}
				Mat getImageROI() {
					return image_ROI;
				}
				Rect getRectROI() {
					return rect_ROI;
				}
		};
		int thresh = 50, N = 11;
		const double ERROR_LEN = 1 / 2.5;
		const double DISCARD_CORNERS_PIXEL = 30;
		const float PROPORZ_CARTELLO_SEGNALE = 1.3;
		const float PROPORZ_LARGHEZZA_ROI = 2.5;
		float focL = 1394.589220272376;
		float objSIZE = 4;
		double minEdge = focL * objSIZE / 300;
		double minArea = minEdge * minEdge;
		double maxEdge = focL * objSIZE / 30;
		double maxArea = maxEdge * maxEdge;

		double MINPRECISION = 0.70;
		double PCVANI = 0.3;
		double duration=0,totDuration=0;
		int cartTrovati = 0;
		bool van = true;
		Point vani;

		bool yComparator(const Point& a, const Point& b);
		bool isDuplicatedSquare(const vector<Point>& toCheck,const vector<vector<Point> >& squares);
		bool isDuplicatedTriangle(const vector<Point>& toCheck,const vector<vector<Point> >& triangles);
		double getDistance(const Point& a, const Point& b);
		double angle(Point pt1, Point pt2, Point pt0);
		int findGeometricSignal(Mat& img, Rect& ROI, vector<GeomSignal*>& signals,vector<Mat> cartels);
		void findSquares(const Mat& image, vector<vector<Point> >& squares);
		void drawSquares(Mat& image, const vector<vector<Point> >& squares);
		vector<int> searchSign(Mat frame,vector<Mat> cartels);

	public:
		RCartel();
		vector<int> lookForSigns(Mat frame,vector<Mat> cartels);

};

#endif