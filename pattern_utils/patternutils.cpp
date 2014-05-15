
#include "patternutils.hpp".

Mat cutBorderByColorRec(Mat mat,int color,bool there,int minSize);

double diffXorMat(Mat ma1,Mat ma2){
	
	double countUguali = 0;

	Mat mat1 = ma1.clone();
	Mat mat2 = ma2.clone();

	//cvtColor(mat1, mat1, CV_BGR2GRAY);
	//cvtColor(mat2, mat2, CV_BGR2GRAY);

	resize(mat1,mat1,Size(RESIZE,RESIZE));
	resize(mat2,mat2,Size(RESIZE,RESIZE));


	mat1 = setWhiteBlack(mat1);
	mat2 = setWhiteBlack(mat2);


	mat1 = cutBorderByColor(mat1,0,false);
	mat1 = cutBorderByColor(mat1,0,true);
	mat2 = cutBorderByColor(mat2,0,false);
	mat2 = cutBorderByColor(mat2,0,true);
	resize(mat1,mat1,Size(RESIZE,RESIZE));
	resize(mat2,mat2,Size(RESIZE,RESIZE));
	
	/*namedWindow("Mat1", WINDOW_NORMAL);
	namedWindow("Mat2", WINDOW_NORMAL);
	imshow("Mat1",ma2);
	imshow("Mat2",mat2);*/


	int m1,m2;
	for(int k = 0; k < RESIZE; k++ ){
		for(int y = 0; y < RESIZE; y++ ){
			m1 = (int)mat1.at<uchar>(k,y);
			m2 = (int)mat2.at<uchar>(k,y);

			if(m1 == m2){
				countUguali++;
			}
			
		}
	}

	return countUguali/pow(RESIZE,2);

}

Mat setWhiteBlack(Mat mat){
	int n=0,m=0;
	for(int k = 0; k < mat.rows ; k++ ){
		for(int y = 0; y < mat.cols ; y++ ){
			Vec3b data = mat.at<Vec3b>(k,y);
			m += (int) (data[0] + data[1] + data[2])/3;
			n++;
		}
	}
	m = m/n;
	cv::threshold( mat, mat,  m, 255, 0);
	return mat;
}

Mat cutBorderByColor(Mat mat,int color,bool there){
	int minSize = mat.rows / 2;
	Mat a = mat.clone();
	Mat b = cutBorderByColorRec(mat,color,there,minSize);
	if(b.rows <= minSize || b.cols <= minSize){
		mat = a;
		return mat;
	}else{
		mat = b;
		return mat;
	}
}

Mat cutBorderByColorRec(Mat mat,int color,bool there,int minSize){
	if(mat.cols <= minSize || mat.rows <= minSize){
		return mat;
	}
	bool oneBlack = false;
	int m1;
	for(int y = 0; y < mat.cols && !oneBlack  ; y++ ){
		m1 = (int)mat.at<uchar>(y,0);
		if(m1 == color){
			oneBlack = true;
		}
	}
	for(int y = 0; y < mat.rows && !oneBlack ; y++ ){
		m1 = (int)mat.at<uchar>(0,y);
		if(m1 == color){
			oneBlack = true;
		}
	}
	for(int y = 0; y < mat.cols && !oneBlack ; y++ ){
		m1 = (int)mat.at<uchar>(y,mat.rows);
		if(m1 == color){
			oneBlack = true;
		}
	}
	for(int y = 0; y < mat.rows && !oneBlack ; y++ ){
		m1 = (int)mat.at<uchar>(mat.cols,y);
		if(m1 == color){
			oneBlack = true;
		}
	}
	if(oneBlack^there){
		return cutBorderByColorRec(mat.colRange(1,mat.cols-1).rowRange(1,mat.rows-1),color,there,minSize);
	}else{
		return mat;
	}
}