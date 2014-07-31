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

/* Dipendenze */  

/* vanishing per calcolare il vanishing point */
#include "vanishing/vanishing.h"

/* Per fare il pattern matching per i cartelli */
#include "pattern_utils/patternutils.hpp"

using namespace cv;
using namespace std;

class RCartel{

	private:

		/*Classi ausiliarie*/
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

		int CONST_RESIZE;
		double ERROR_LEN;
		double DISCARD_CORNERS_PIXEL;
		float PROPORZ_CARTELLO_SEGNALE;
		float PROPORZ_LARGHEZZA_ROI;
		float focL;
		float objSIZE;
		double minEdge;
		double minArea;
		double maxEdge;
		double maxArea;

		/*Precisione minima del pattern matching*/
		double MINPRECISION;

		/*Percentuale di foto intorno al vanishing point*/
		double PCVANI;

		/*Parametri per debug*/
		double duration,totDuration;
		int cartTrovati;

		/*van: vanishing point già calcolato -- vani: vanishing point calcolato*/
		bool van;
		Point vani;

		bool isDuplicatedSquare(const vector<Point>& toCheck,const vector<vector<Point> >& squares);
		bool isDuplicatedTriangle(const vector<Point>& toCheck,const vector<vector<Point> >& triangles);
		double getDistance(const Point& a, const Point& b);
		double angle(Point pt1, Point pt2, Point pt0);
		int findGeometricSignal(Mat& img, Rect& ROI, vector<GeomSignal*>& signals,vector<Mat> cartels);
		void findSquares(const Mat& image, vector<vector<Point> >& squares);
		void drawSquares(Mat& image, const vector<vector<Point> >& squares);
		vector<int> searchSign(Mat frame,vector<Mat> cartels);

		struct Comp {
		    bool operator() (cv::Point a, cv::Point b) { return a.y < b.y; }
		} YComparator;

	public:
		/*Costruttore di base*/
		RCartel();

		/*
			Funzione che cerca cartelli nel frame passato in input.
			I cartelli cercati sono quelli passati con il vettore passato in inut
			La funzione restituisce l'index corrispondente all' array in input dei cartelli trovati
			Param
				Mat frame: 				frame da dare in input
				vector<Mat> cartels:	vettore di pattern di cartelli da fare pattern matching
			Return
				Ritorna un vector<int> contenente gli index dei cartelli trovati in frame
		*/
		vector<int> lookForSigns(Mat frame,vector<Mat> cartels);

		/*Pulisce il vanishing point e lo fa ricalcolare sul prossimo frame*/
		void cleanVanischingPoint();

};

#endif