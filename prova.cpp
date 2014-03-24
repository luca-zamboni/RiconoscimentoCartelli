//============================================================================
// Name        : RiconoscimentoCartelli.cpp
// Author      : Giovanni
// Modified by : Luca
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>

#include <vector>
#include <fstream>

using namespace cv;
using namespace std;

bool inRange(float x, float n,float n2);
void fun(int, void*);

int const max_lowThreshold = 300;
int lowThreshold;
int ratio = 3;
int kernel_size = 3;
int rr;
float ST_RG= 0.3; // standard range

Mat frame; // frame originale catturato dalla telecamera
int main() {

	
	namedWindow("prova", WINDOW_NORMAL);
	//namedWindow("prova", CV_WINDOW_AUTOSIZE);

	createTrackbar( "Min Threshold:", "prova", &lowThreshold, max_lowThreshold, fun );
	//createTrackbar( "Min Threshold:", "prova", &lowThreshold, max_lowThreshold, fun );

	fun(1,0);

	waitKey(0);


	cvDestroyAllWindows();
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

void fun(int, void*){

	double duration;
	double nciclo = 1;
	int SIZE_SQUARE = 4;
	int MIN_AREA = 150;
	double TOLL = 0.5;

	/*VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) // check if we succeeded
		return -1;*/
	int N_ROI = 0;
	int N_Signal = 0;

	Mat frameGray; // frame in bianco e nero
	Mat normal; // visione normale su cui poter disegnare
	//cap >> frame;
	frame = imread("./Test_img/IMG_1228.JPG");

	normal = frame.clone();
	
	Mat im_gray;
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	RNG rng(12345);

	Mat drawing = Mat::zeros( normal.size(), CV_8UC3 );
	drawing = normal;

	cvtColor(normal,im_gray,CV_BGR2GRAY);

	//Size temp = im_gray.size();
	/*pyrDown(im_gray, im_gray, Size(im_gray.cols / 2, im_gray.rows / 2));
	pyrUp(im_gray, im_gray, temp);*/
	
	int th = 20;

	Canny( im_gray, im_gray, 100, 100*ratio, kernel_size );
	//Canny(im_gray, im_gray, 50, 150, 3);

	dilate(im_gray, im_gray, Mat(), Point(-1, -1));
	

	findContours( im_gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1) );
	//cout << contours.size() << endl;

	vector<Point> approx;
	vector<vector<Point> > contSquare( contours.size() );
	vector<Rect> boundRectA( contours.size() );

	int j=0;
	for(int i=0;i<contours.size();i++){

		Scalar color = Scalar( rng.uniform(255, 255), rng.uniform(255,255), rng.uniform(255,255) );
		//drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
		approxPolyDP( Mat(contours[i]), contSquare[i], 3, true );

		//cout << contSquare.size() << endl;
		//cout << contSquare[i].size() << endl;
		if(contSquare[i].size() >= SIZE_SQUARE){
        	boundRectA[j] = boundingRect( Mat(contSquare[i]) );
        	j++;
		}

	}

	vector<Rect> boundRect(j);
	for(int i=0;i<j;i++){
		boundRect[i] = boundRectA[i];
	}

	//cout << boundRect.size();

	for( int i = 0; i< boundRect.size(); i++ ){
		Scalar color = Scalar( rng.uniform(255, 255), rng.uniform(255,255), rng.uniform(255,255) );

		int x,y,h,w;
		x = boundRect[i].x;
		y = boundRect[i].y;
		w = boundRect[i].width;
		h = boundRect[i].height;
		cout << w*h << endl;;

		if(inRange((float)w/h,1,0.2)  && w*h > MIN_AREA){

			rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );

			//cout  << (float) w/h <<  endl;

			Mat aux = normal.colRange(x,x+w).rowRange(y,y+h);

			cvtColor(aux,aux,CV_BGR2GRAY);

			int n=0,m=0;
			for(int k = 0; k < aux.rows ; k++ ){
				for(int y = 0; y < aux.rows ; y++ ){
					Vec3b data = aux.at<Vec3b>(k,y);
					m += (int) (data[0] + data[1] + data[2])/3;
					n++;
				}
			}

			m = m/n;

			cv::threshold( aux, aux, m, 255 ,0 );
			
			GaussianBlur( aux, aux, cv::Size(9, 9), 2, 2);

			vector<Vec3f> circles;

			lowThreshold+=1;

			HoughCircles(aux, circles, CV_HOUGH_GRADIENT,2, 200, lowThreshold, lowThreshold/2 +1 ,0,1000);

			for( size_t rt = 0; rt < circles.size(); rt++ ){

				Point center(cvRound(circles[rt][0]), cvRound(circles[rt][1]));
				int radius = cvRound(circles[rt][2]);

				center.x += x;
				center.y += y;


				//circle( drawing, center, radius, Scalar(0,0,255), 3, 8, 0 );

				bool cond = inRange(radius*2,w,ST_RG) && inRange(radius*2,h,ST_RG);
				//cond = cond && inRange(center.x,x,ST_RG) && inRange(center.y,y,1-ST_RG);

				if(cond){

					circle( drawing, center, radius, Scalar(0,0,255), 3, 8, 0 );

					//rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );

				}

			}
		}
		imshow("prova", drawing);
	}
		
}

bool inRange(float x, float n,float range){
	return (x<n*range+n && x>n-n*range);

}





