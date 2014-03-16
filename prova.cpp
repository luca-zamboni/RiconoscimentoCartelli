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

bool beetween(float x, float n,float n2);

Mat frame; // frame originale catturato dalla telecamera
int main() {

	// to measure the execution time
	double duration;
	double nciclo = 1;
	int SIZE_SQUARE = 4;
	double TOLL = 0.5;

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened()) // check if we succeeded
		return -1;
	int N_ROI = 0;
	int N_Signal = 0;

	while (nciclo <= 100) {

		Mat frameGray; // frame in bianco e nero
		Mat normal; // visione normale su cui poter disegnare
		cap >> frame;
		frame = imread("./Img_ForTest/finalResult.jpg");

		if(frame.empty()){
			break;
		}

		normal = frame.clone();
		
		Mat im_gray;
		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		RNG rng(12345);

		Mat drawing = Mat::zeros( normal.size(), CV_8UC3 );
		drawing = normal;

		cvtColor(normal,im_gray,CV_BGR2GRAY);

		Size temp = im_gray.size();
		pyrDown(im_gray, im_gray, Size(im_gray.cols / 2, im_gray.rows / 2));
		pyrUp(im_gray, im_gray, temp);
		
		int th = 20;
		Canny(im_gray, im_gray, 0, 50 , 3);

		dilate(im_gray, im_gray, Mat(), Point(2, 2));
		

		findContours( im_gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1) );
		//cout << contours.size() << endl;

		vector<Point> approx;
		vector<vector<Point> > contSquare( contours.size() );
		vector<Rect> boundRectA( contours.size() );

		int j=0;
		for(int i=0;i<contours.size();i++){

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

		for( int i = 0; i< boundRect.size(); i++ ){
			Scalar color = Scalar( rng.uniform(255, 255), rng.uniform(255,255), rng.uniform(255,255) );
			//drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
			//rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );

			int x,y,h,w;
			x = boundRect[i].x;
			y = boundRect[i].y;
			w = boundRect[i].width;
			h = boundRect[i].height;



				

			Mat aux;
			cvtColor(normal.colRange(x,x+w).rowRange(y,y+h),aux,CV_BGR2GRAY);

			GaussianBlur( aux, aux, cv::Size(9, 9), 2, 2 );

			vector<Vec3f> circles;

			HoughCircles(aux, circles, CV_HOUGH_GRADIENT,6, 350, 200, 100 );

			for( size_t rt = 0; rt < circles.size(); rt++ ){

				Point center(cvRound(circles[rt][0]), cvRound(circles[rt][1]));
				int radius = cvRound(circles[rt][2]);
				center.x += x;
				center.y += y;
Point po(x,y);
				circle( drawing, po, 4, Scalar(0,0,255), 3, 8, 0 );

				if(beetween(radius*2,w*0.3,w*0.7) && beetween(radius*2,h*0.3,h*0.7) &&
					beetween(center.x,x+w/2*0.3,x+w/2*0.7) && beetween(center.y,y+h/2*0.3,x+h/2*0.7)){
					circle( drawing, center, radius, Scalar(0,0,255), 3, 8, 0 );
					rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
				}

			}

		}

		namedWindow("prova", WINDOW_NORMAL);
		imshow("prova", drawing);

		if (waitKey(30) >= 0)
			break;

		nciclo++;
	}


	cvDestroyAllWindows();
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

bool beetween(float x, float n,float n2){

	return (x>n && x<n2);

}





