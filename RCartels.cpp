//============================================================================
// Name        : RiconoscimentoCartelli.cpp
// Author      : Giovanni
// Optimization: Luca
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>

#include <vector>
#include <fstream>

#include <string.h> 

#include <dirent.h> 
#include <stdio.h> 

#include <sys/stat.h>

#include "vanishing/vanishing.h"

using namespace cv;
using namespace std;

class ROI_Rect {
private:
	Rect ROI;
	vector<Point> Rect;
public:
	ROI_Rect(cv::Rect RROI, vector<Point> RRect);
	cv::Rect getROI() {
		return ROI;
	}
	vector<Point> getRect() {
		return Rect;
	}
};
ROI_Rect::ROI_Rect(cv::Rect RROI, vector<Point> RRect) {
	ROI = RROI;
	Rect = RRect;
}

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
GeomSignal::GeomSignal(string nname, Mat iimage_ROI, Rect rrect_ROI) {
	name = nname;
	image_ROI = iimage_ROI;
	rect_ROI = rrect_ROI;
}
;

bool yComparator(const Point& a, const Point& b);
bool isDuplicatedSquare(const vector<Point>& toCheck,
		const vector<vector<Point> >& squares);
bool isDuplicatedTriangle(const vector<Point>& toCheck,
		const vector<vector<Point> >& triangles);
double getDistance(const Point& a, const Point& b);

int thresh = 50, N = 11;
const char* wndname = "Square Detection Demo";
double angle(Point pt1, Point pt2, Point pt0);
void findSquares(const Mat& image, vector<vector<Point> >& squares);
void drawSquares(Mat& image, const vector<vector<Point> >& squares);
const double ERROR_LEN = 1 / 2.5;
const double DISCARD_CORNERS_PIXEL = 30;
void findGeometricSignal(Mat& img, Rect& ROI, vector<GeomSignal*>& signals);

const float PROPORZ_CARTELLO_SEGNALE = 1.3;

const float PROPORZ_LARGHEZZA_ROI = 2.5;

float focL = 1394.589220272376;
float objSIZE = 8;

double minEdge = focL * objSIZE / 300;
double minArea = minEdge * minEdge;

double maxEdge = focL * objSIZE / 30;
double maxArea = maxEdge * maxEdge;

string DIR_IMG = "./Test_img/";
string FORMA = "";

string funz(Mat frame);
bool is_dir(const char* path) ;
ofstream durationStat;
double duration=0,totDuration=0;
int cartTrovati = 0;

int main(int argc, char* argv[]) {

	double nciclo = 1;
	Mat frame;
	float media = 0,mtri=0,mcirc=0,msqua=0;
	int NCICLI = 20,tri=0,squa=0,circ=0,count=0;
	string NOT_FOUND = "null";

	if(argc!=3){
		FORMA = "C";
		NCICLI = 2;
	}
	else{
		FORMA = argv[1];
		NCICLI = atoi(argv[2]);
		if(FORMA != "C" && FORMA != "S" && FORMA != "T" && FORMA != "V"){
			cout << " usage : ./app [Form] [Time] " << endl;
			return 0;
		}
		if(NCICLI < 2 || NCICLI > 100){
			cout << "Out of range" << endl;
			return 0;
		}
		
	}

	DIR_IMG += FORMA + "/";
	namedWindow("Normal", WINDOW_NORMAL);
	//namedWindow("Immaginisenzarumore", WINDOW_NORMAL);

	remove("./FileOutput/GlobalStat.txt");
	ofstream statFile("./FileOutput/GlobalStat.txt", ios::app);

	remove("./FileOutput/DurationStat.txt");
	durationStat.open("./FileOutput/DurationStat.txt", ios::app);

	string filename = "";
	DIR *d;
	struct dirent *dir;
	d = opendir(DIR_IMG.c_str());
	while ((dir = readdir(d)) != NULL){

		if(is_dir((DIR_IMG + dir->d_name).c_str())){
			continue;
		}

		count++;

		string aux(dir->d_name);
		string outFilename = "./FileOutput/"+FORMA+"/"+aux.substr(0,aux.size()-3)+"txt";

		media = 0;
		string found = NOT_FOUND;
		string auxFound = NOT_FOUND;

		remove(outFilename.c_str());
		ofstream oneStatFile(outFilename.c_str(), ios::app);

		// N cicli di test
		for(int nciclo=0;nciclo < NCICLI;nciclo++) {

			frame = imread(DIR_IMG + aux);
			//frame = imread(DIR_IMG + "strada.jpg");
			duration = static_cast<double>(cv::getTickCount());
			totDuration  = static_cast<double>(cv::getTickCount());

			found = funz(frame.clone());

			duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
			totDuration = (static_cast<double>(cv::getTickCount()) - totDuration) / getTickFrequency();
			cout << totDuration << " total Duration " << endl;
			cout << cartTrovati << " Cartelli trovati " << endl << endl;
			cartTrovati=0;

			if(found != NOT_FOUND){
				auxFound = found;
			}
			oneStatFile << found << " \t\t- " << totDuration << ";" << endl;
			media += totDuration;
			waitKey(1000);

		}

		if(auxFound != NOT_FOUND){
			if(auxFound == "Triang"){
				tri++;
				mtri += totDuration;
			}
			if(auxFound == "Circle"){
				circ++;
				mcirc += totDuration;
			}
			if(auxFound == "Square"){
				squa++;
				msqua += totDuration;
			}
		}

		media = media / NCICLI;
		oneStatFile << "Media : " << media << endl;
		oneStatFile.close();

	}
	closedir(d);

	if(tri!=0) mtri = mtri / tri;
	if(squa!=0) msqua = msqua / squa;
	if(circ!=0) mcirc = mcirc / circ;

	statFile << "N-Input : \t" << count << "\t Riconosciuti : " << (tri+circ+squa) << endl;
	statFile << "Triangl : \t" << tri << "\t t-medio : \t" << mtri << endl;
	statFile << "Circles : \t" << circ << "\t t-medio : \t" << mcirc << endl;
	statFile << "Squares : \t" << squa << "\t t-medio : \t" << msqua << endl;
	statFile << "N Cicli : \t" << NCICLI;

	cout << "N-Input : \t" << count << "\t Riconosciuti : " << (tri+circ+squa) << endl;
	cout << "Triangl : \t" << tri << "\t t-medio : \t" << mtri << endl;
	cout << "Circles : \t" << circ << "\t t-medio : \t" << mcirc << endl;
	cout << "Squares : \t" << squa << "\t t-medio : \t" << msqua << endl;
	cout << "N Cicli : \t" << NCICLI << endl;

	statFile.close();
	durationStat.close();

	cvDestroyAllWindows();
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

string funz(Mat frame){

	int numRoi = 0;
	int numTri = 0;
	int numCir = 0;

	double *out;
	int n;

	vector<vector<Point> > squares;
	vector<ROI_Rect*> vectROI_Rect;
	vector<GeomSignal*> geomSignals;

	int N_ROI = 0;
	int N_Signal = 0;
	Mat frameGray; // frame in bianco e nero
	Mat normal;
	string ret = "null";

	cvtColor(frame, frameGray, CV_BGR2GRAY);

	duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
	cout << duration << " Before lsd" << endl;
	duration = static_cast<double>(cv::getTickCount());

	Point p = vanishingPoint(frameGray,Point(0,0));
	cout << p << endl;

	duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
	cout << duration << " Lsd time" << endl;
	duration = static_cast<double>(cv::getTickCount());

	normal = frame.clone();
	Mat fG = frameGray.clone();
	findSquares(fG, squares);
	const vector<Point>* curr_square;
	Rect ROI;
	for (size_t i = 0; i < squares.size(); i++) {
		curr_square = &squares[i];

		ROI = cv::boundingRect(cv::Mat(*curr_square));
		Point centerROI(ROI.x + ROI.width / 2, ROI.y + ROI.height / 2);

		// aggiungo una nuova ROI solo se il centro di essa (che sara' oggetto di indagine)
		// non e' gia' contenuto in un'altra ROI
		if (vectROI_Rect.size() == 0) {
			//la aggiungo
			vectROI_Rect.push_back(new ROI_Rect(ROI, *curr_square));
		} else {
			// sel il flag è true ROI verrà aggiunta in memoria sempre che flag1=true
			bool flag = false;
			bool flag1 = true;
			// scorro tutto il vettore di ROI_Rect in modo inverso
			vector<ROI_Rect*>::reverse_iterator rit_ROI_Rect;
			int ii = vectROI_Rect.size() - 1;
			for (rit_ROI_Rect = vectROI_Rect.rbegin();
					rit_ROI_Rect != vectROI_Rect.rend(); ++rit_ROI_Rect) {
				Point centerROI_rit_ROI_Rect(
						(*rit_ROI_Rect)->getROI().x
								+ (*rit_ROI_Rect)->getROI().width / 2,
						(*rit_ROI_Rect)->getROI().y
								+ (*rit_ROI_Rect)->getROI().height / 2);

				// se ROI ha semiperimetro minore di quella in memoria
				if((ROI.width + ROI.height) < ((*rit_ROI_Rect)->getROI().width
						+ (*rit_ROI_Rect)->getROI().height)){
					if(ROI.contains(centerROI_rit_ROI_Rect)){
						delete vectROI_Rect[ii];
						vectROI_Rect[ii] = vectROI_Rect[vectROI_Rect.size()
								- 1];
						vectROI_Rect.pop_back();
						if (!flag)
							flag = true;
					}else{
						if (!flag)
							flag = true;
					}
				}else{
					// la ROI_in memoria  pi piccola

					if((*rit_ROI_Rect)->getROI().contains(centerROI)){
						// questa ROI_in memoria contiene il centro della ROI
						// ROI non andra aggiunta in NESSUN caso - setto a false il flag1
						flag1 = false;
					}else{
						 // la ROI_in memoria non contiene il centro di ROI
						if (!flag)
							flag = true;
					}
				}

				// decremento l'indice posizionale dell'array
				ii--;
			}
			if (flag) {
				if(flag1){
					// aggiungo la ROI
					vectROI_Rect.push_back(new ROI_Rect(ROI, *curr_square));
				}
			}
		}

	}

	//cout << "N ROI_Rect: " << vectROI_Rect.size() << endl;
	N_ROI = vectROI_Rect.size();
	int color;
	vector<ROI_Rect*>::iterator it;
	for (it = vectROI_Rect.begin(); it != vectROI_Rect.end(); ++it) {
		color = rand() % 256;
		Point centerROI_((*it)->getROI().x + (*it)->getROI().width / 2,
				(*it)->getROI().y + (*it)->getROI().height / 2);

		vector<Point> vp = (*it)->getRect();
		const Point* p = &vp[0];
		int n = (int) (*it)->getRect().size();
		polylines(normal, &p, &n, 1, true, Scalar(color, color, color), 2,
		CV_AA);

		cv::rectangle(normal, (*it)->getROI(),
				cv::Scalar(255, 255, 0), 2);
		circle(normal, centerROI_, 2, cv::Scalar(255, 255, 0), 2); // draw dot in the center of ROI
	}

	vector<ROI_Rect*>::iterator it1;
	vector<Point> vp;
	for (it1 = vectROI_Rect.begin(); it1 != vectROI_Rect.end(); ++it1) {

		vp = (*it1)->getRect();
		int l1 = (int)getDistance(vp[0], vp[1]);
		int l2 = (int)getDistance(vp[1], vp[2]);
		int l3 = (int)getDistance(vp[2], vp[3]);
		int l4 = (int)getDistance(vp[3], vp[0]);

		int maxEdgeLength = MAX(MAX(l1,l2), MAX(l3,l4));

		Mat perspective(maxEdgeLength, maxEdgeLength, CV_8U, Scalar(0));

		vector<Point2f> pointSquareMovedImage(4);
		pointSquareMovedImage[0] = vp[0];
		pointSquareMovedImage[1] = vp[1];
		pointSquareMovedImage[2] = vp[2];
		pointSquareMovedImage[3] = vp[3];

		vector<Point2f> pointSquareFrontImage(4);
		pointSquareFrontImage[0] = Point2f(0, 0);
		pointSquareFrontImage[1] = Point2f(maxEdgeLength, 0);
		pointSquareFrontImage[2] = Point2f(maxEdgeLength, maxEdgeLength);
		pointSquareFrontImage[3] = Point2f(0, maxEdgeLength);

		Mat matrixPerspectiveTransform;
		matrixPerspectiveTransform = getPerspectiveTransform(
				pointSquareMovedImage, pointSquareFrontImage);

		warpPerspective(frameGray, perspective, matrixPerspectiveTransform,
				perspective.size(), INTER_LINEAR, BORDER_CONSTANT, 0);

		/*namedWindow("perspective", WINDOW_NORMAL);
		imshow("perspective", perspective);*/

		Rect ROI = (*it1)->getROI();
		findGeometricSignal(perspective, ROI, geomSignals);
	}

	duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
	cout << duration << " Lavorazione quadrati con forme geometrihe" << endl;
	duration = static_cast<double>(cv::getTickCount());
	//cout << "N segnali trovati: " << geomSignals.size() << endl;
	N_Signal = geomSignals.size();
	unsigned int nTriangle = 0;
	unsigned int nCircle = 0;

	vector<GeomSignal*>::reverse_iterator rit;
	int i = geomSignals.size() - 1;
	for (rit = geomSignals.rbegin(); rit != geomSignals.rend(); ++rit) {
		if ((*rit)->getName() == "Triangle") {
			nTriangle++;
		} else if ((*rit)->getName() == "Circle") {
			nCircle++;
		}

		ret = (*rit)->getName();
		
		putText(normal, (*rit)->getName(),
				Point((*rit)->getRectROI().x,
						(*rit)->getRectROI().y
								+ (*rit)->getRectROI().height),
				cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 0), 2,
				true);



		delete *rit;
		geomSignals[i] = geomSignals[geomSignals.size() - 1];
		geomSignals.pop_back();

		// decremento l'indice posizionale dell'array
		i--;
	}

	//imshow("Immagini", normal);


	numRoi += N_ROI;
	numCir += nCircle;
	numTri += nTriangle;

	// clear data arrays
	squares.clear();
	if (vectROI_Rect.size() > 0) {
		vector<ROI_Rect*>::iterator it;
		for (it = vectROI_Rect.begin(); it != vectROI_Rect.end(); ++it) {
			delete *it;
		}
		vectROI_Rect.clear();
	}
	if (geomSignals.size() > 0) {
		geomSignals.clear();
	}
	return ret;
}


double angle(Point pt1, Point pt2, Point pt0) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2)
			/ sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

void findSquares(const Mat& image, vector<vector<Point> >& squares) {
	squares.clear();

	Mat pyr, timg, gray0(image.size(), CV_8U), gray;


	//imshow("Immagini",image);
	duration = static_cast<double>(cv::getTickCount());
	pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
	pyrUp(pyr, timg, image.size());
	vector<vector<Point> > contours;

	duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
	cout << duration << " time di resize" << endl;
	duration = static_cast<double>(cv::getTickCount());

	Canny(timg, gray, 155, 255, 5);
	dilate(gray, gray, Mat(), Point(-1, -1));

	duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
	cout << duration << " time di canny" << endl;
	duration = static_cast<double>(cv::getTickCount());

	//imshow("Immaginisenzarumore",gray);

	// find contours and store them all as a list
	findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		approxPolyDP(Mat(contours[i]), approx,
				arcLength(Mat(contours[i]), true) * 0.02, true);

		if (approx.size() == 4 && contourArea(Mat(approx)) > minArea
				&& /*aggiunto->*/contourArea(Mat(approx)) < maxArea
				&& isContourConvex(Mat(approx))) {
			double maxCosine = 0;

			for (int j = 2; j < 5; j++) {
				// find the maximum cosine of the angle between joint edges
				double cosine = fabs(
						angle(approx[j % 4], approx[j - 2], approx[j - 1]));
				maxCosine = MAX(maxCosine, cosine);
			}

			if (maxCosine < 0.5) {

				std::stable_sort(approx.begin(), approx.end(), yComparator);
				if (approx[0].x > approx[1].x) {
					Point tmp = approx[0];
					approx[0] = approx[1];
					approx[1] = tmp;
				}
				if (approx[2].x < approx[3].x) {
					Point tmp = approx[2];
					approx[2] = approx[3];
					approx[3] = tmp;
				}

				// controllo che i lati opposti del quadrato siano pressapoco lunghi uguali
				// in particolare controllo che tra lati opposti
				// (lato1 e 3, lato2 e 4) il rapporto tra il minimo e il massimo
				// sia superiore ad 1/3
				double edgeLength[4] = { 0.0, 0.0, 0.0, 0.0 };
				edgeLength[0] = getDistance(approx[0], approx[1]);
				edgeLength[1] = getDistance(approx[1], approx[2]);
				edgeLength[2] = getDistance(approx[2], approx[3]);
				edgeLength[3] = getDistance(approx[3], approx[0]);

				bool a, b, c = false;

				if (edgeLength[0] > edgeLength[2]) {
					a = edgeLength[2] / edgeLength[0] > ERROR_LEN;
				} else {
					a = edgeLength[0] / edgeLength[2] > ERROR_LEN;
				}

				if (edgeLength[1] > edgeLength[3]) {
					b = edgeLength[3] / edgeLength[1] > ERROR_LEN;
				} else {
					b = edgeLength[1] / edgeLength[3] > ERROR_LEN;
				}

				// controllo che la differenza tra lato 1 e 2 sia
				// inferiore a 1/3 il massimo tra i due
				c = fabs(edgeLength[0] - edgeLength[1])
						< (MAX(edgeLength[0], edgeLength[1])) * ERROR_LEN;

				if (a && b && c) {
					// controlla che nn sia un duplicato di qualche altro square
					if (!isDuplicatedSquare(approx, squares)) {
						squares.push_back(approx);
					}
				}
			}
		}
	}
}

// the function draws all the squares in the image
void drawSquares(Mat& image, const vector<vector<Point> >& squares) {
	for (size_t i = 0; i < squares.size(); i++) {
		const Point* p = &squares[i][0];
		int n = (int) squares[i].size();
		polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, CV_AA);
	}
	//***********
	const Point* p = &squares[squares.size() - 1][0];
	int n = (int) squares[squares.size() - 1].size();
	polylines(image, &p, &n, 1, true, Scalar(255, 0, 0), 3, CV_AA);
	//***********
	//imshow(wndname, image);
}
bool yComparator(const Point& a, const Point& b) {
	return a.y < b.y;
}
bool isDuplicatedSquare(const vector<Point>& toCheck,
		const vector<vector<Point> >& squares) {
	for (size_t i = 0; i < squares.size(); i++) {
		const vector<Point>* curr_square = &squares[i];

		//se almeno 3 vertici distano  meno di DISCARD_CORNERS_PIXEL ritorna true
		int result0 = 0, result1 = 0, result2 = 0, result3 = 0;
		if (getDistance(toCheck[0], curr_square[0][0]) <= DISCARD_CORNERS_PIXEL)
			result0 = 1;
		if (getDistance(toCheck[1], curr_square[0][1]) <= DISCARD_CORNERS_PIXEL)
			result1 = 1;
		if (getDistance(toCheck[2], curr_square[0][2]) <= DISCARD_CORNERS_PIXEL)
			result2 = 1;
		if (getDistance(toCheck[3], curr_square[0][3]) <= DISCARD_CORNERS_PIXEL)
			result3 = 1;
		if (result0 + result1 + result2 + result3 >= 3) {
			return true;
		}
	}
	return false;
}
bool isDuplicatedTriangle(const vector<Point>& toCheck,
		const vector<vector<Point> >& triangles) {
	for (size_t i = 0; i < triangles.size(); i++) {
		const vector<Point>* curr_triangle = &triangles[i];

		// se i rispettivi 3 vertici distano tra loro
		// meno di DISCARD_CORNERS_PIXEL ritorna true
		if ((getDistance(toCheck[0], curr_triangle[0][0])
				<= DISCARD_CORNERS_PIXEL)
				&& (getDistance(toCheck[1], curr_triangle[0][1])
						<= DISCARD_CORNERS_PIXEL)
				&& (getDistance(toCheck[2], curr_triangle[0][2])
						<= DISCARD_CORNERS_PIXEL)) {
			return true;
		}
	}
	return false;
}
double getDistance(const Point& a, const Point& b) {
	return sqrt(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
}

void findGeometricSignal(Mat& img, Rect& ROI, vector<GeomSignal*>& geomeSignals) {

	cartTrovati ++;

	//duration = static_cast<double>(cv::getTickCount());

	Mat result = img.clone();
	int lato_img = img.size().width;
	Mat img_find_circles = img.clone();

	GaussianBlur(img_find_circles, img_find_circles, cv::Size(3, 3), 0.5);

	vector<Vec3f> circles;
	HoughCircles(img_find_circles, circles, CV_HOUGH_GRADIENT, 2, 50, 80, 40, 0, 1000); 

	if (circles.size() > 0) {
		vector<Vec3f>::reverse_iterator rit_c;
		int i = circles.size() - 1;
		for (rit_c = circles.rbegin(); rit_c != circles.rend(); ++rit_c) {
			if (lato_img / (*rit_c)[2] <= PROPORZ_CARTELLO_SEGNALE) {
				circles[i] = circles[circles.size() - 1];
				circles.pop_back();
			}
			i--;
		}
	}


	Mat pyr, timg, gray0(img.size(), CV_8U), gray;
	// down-scale and upscale the image to filter out the noise
	pyrDown(img, pyr, Size(img.cols / 2, img.rows / 2));
	pyrUp(pyr, timg, img.size());
	vector<vector<Point> > contours;

	vector<vector<Point> > triangles;

	/***  Mia modifica che migliore i triangoli ***/

	int n=0,m=0;
	for(int k = 0; k < timg.rows ; k++ ){
		for(int y = 0; y < timg.rows ; y++ ){
			Vec3b data = timg.at<Vec3b>(k,y);
			m += (int) (data[0] + data[1] + data[2])/3;
			n++;
		}
	}

	m = m/n;

	cv::threshold( timg, timg, m, 255 ,0 );

	/******/

	Canny(timg, gray, 155, 255, 3);
	//namedWindow("tri", WINDOW_NORMAL);

	// find contours and store them all as a list
	findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


	//imshow("tri", timg);
	vector<Point> approx;

	// test each contour
	for (size_t i = 0; i < contours.size(); i++) {
		// approximate contour with accuracy proportional
		// to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx,
				arcLength(Mat(contours[i]), true) * 0.05, true);

		if (approx.size() == 3) {

			double maxCosine = 0;

			for (int j = 2; j < 4; j++) {
				// find the maximum cosine of the angle between joint edges
				double cosine = fabs(
						angle(approx[j % 4], approx[j - 2], approx[j - 1]));
				maxCosine = MAX(maxCosine, cosine);
			}

			if (maxCosine > 0.3 && maxCosine < 0.7) {
				std::stable_sort(approx.begin(), approx.end(), yComparator);
				if (approx[1].x > approx[2].x) {
					Point tmp = approx[1];
					approx[1] = approx[2];
					approx[2] = tmp;
				}

				double edgeLength[3] = { 0.0, 0.0, 0.0 };
				edgeLength[0] = getDistance(approx[0], approx[1]);
				edgeLength[1] = getDistance(approx[1], approx[2]);
				edgeLength[2] = getDistance(approx[2], approx[0]);

				if (lato_img / edgeLength[0] > PROPORZ_CARTELLO_SEGNALE
						&& lato_img / edgeLength[1] > PROPORZ_CARTELLO_SEGNALE
						&& lato_img / edgeLength[2] > PROPORZ_CARTELLO_SEGNALE) {
					if (fabs(edgeLength[0] - edgeLength[1])
							< (MAX(edgeLength[0], edgeLength[1]) * ERROR_LEN)
							&& fabs(edgeLength[1] - edgeLength[2])
									< (MAX(edgeLength[1], edgeLength[2])
											* ERROR_LEN)
							&& fabs(edgeLength[0] - edgeLength[2])
									< (MAX(edgeLength[0], edgeLength[2])
											* ERROR_LEN)) {
						//controlla che nn sia un duplicato di qualche altro triangle
						if (!isDuplicatedTriangle(approx, triangles)) {
							triangles.push_back(approx);
						}
					}
				}
			}
		}

	}

	int raggioIntorno = img.size().width / (2.1 * 2 * 1.732050808);
	Point center_img(img.size().width / 2, img.size().height / 2);

	double distance_from_center_img;

	if (circles.size() > 0) {
		vector<Vec3f>::reverse_iterator rit_c;
		int i = circles.size() - 1;
		for (rit_c = circles.rbegin(); rit_c != circles.rend(); ++rit_c) {
			distance_from_center_img = getDistance(center_img,
					Point((*rit_c)[0], (*rit_c)[1]));
			if (distance_from_center_img > raggioIntorno) {
				circles[i] = circles[circles.size() - 1];
				circles.pop_back();
			}
			i--;
		}
	}
	if (triangles.size() > 0) {
		vector<vector<Point> >::reverse_iterator rit_t;
		int i = triangles.size() - 1;
		for (rit_t = triangles.rbegin(); rit_t != triangles.rend(); ++rit_t) {
			Moments mom = moments(*rit_t);
			//position of mass  center converted to integer
			Point centerT(mom.m10 / mom.m00, mom.m01 / mom.m00);
			distance_from_center_img = getDistance(center_img, centerT);
			if (distance_from_center_img > raggioIntorno) {
				triangles[i] = triangles[triangles.size() - 1];
				triangles.pop_back();
			}
			i--;
		}
	}

	char name = 'N';
	double minArea = -1;
	double area;

	if (circles.size() > 0) {
		vector<Vec3f>::const_iterator it_c = circles.begin();
		while (it_c != circles.end()) {

			area = ((*it_c)[2]) * 3.1415926535;
			if (area < minArea || minArea == -1) {
				minArea = area;
				name = 'C';
			}

			++it_c;
		}
	}
	if (triangles.size() > 0) {
//		drawContours(result, triangles, -1, cv::Scalar(161, 0, 244), 1); // with a thickness of 1
		vector<vector<Point> >::const_iterator it_t = triangles.begin();
		while (it_t != triangles.end()) {
			area = contourArea((*it_t), false);
			if (area < minArea || minArea == -1) {
				minArea = area;
				name = 'T';
			}

			++it_t;
		}
	}

	if (name == 'C') {
		geomeSignals.push_back(new GeomSignal("Circle", img, ROI));
	} else if (name == 'T') {
		geomeSignals.push_back(new GeomSignal("Triang", img, ROI));
	} else if (name == 'N') {
		// none to add
	}

	/*duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
	cout << duration << " time di ricerca forme geometriche" << endl;
	duration = static_cast<double>(cv::getTickCount());*/

	//imshow("result", result);
}

bool is_dir(const char* path) {
    struct stat buf;
    lstat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
