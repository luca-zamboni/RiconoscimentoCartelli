
#include "patternutils.hpp"

double diffXorMat(Mat mat1,Mat mat2){
	
	double countUguali = 0;
	setWhiteBlack(mat1);
	setWhiteBlack(mat2);

	resize(mat1,mat1,Size(800,800));
	resize(mat2,mat2,Size(800,800));

	int m1,m2;
	for(int k = 0; k < mat1.rows ; k++ ){
		for(int y = 0; y < mat1.cols ; y++ ){
			Vec3b data = mat1.at<Vec3b>(k,y);
			Vec3b data2 = mat2.at<Vec3b>(k,y);
			m1 = (int) data[0];
			m2 = (int) data2[0];

			if(m1 == m2){
				countUguali++;
			}
			
		}
	}
	return countUguali/(mat1.rows*mat1.cols);

}



Mat setWhiteBlack(Mat mat){
	Mat ret = mat.clone();
	int n=0,m=0;
	for(int k = 0; k < ret.rows ; k++ ){
		for(int y = 0; y < ret.cols ; y++ ){
			Vec3b data = ret.at<Vec3b>(k,y);
			m += (int) (data[0] + data[1] + data[2])/3;
			n++;
		}
	}

	m = m/n;

	cv::threshold( ret, ret, m, 255 ,0 );

	return ret;
}