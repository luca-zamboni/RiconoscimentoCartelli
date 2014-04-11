
#include "vanishing.h"

using namespace std;

Point vanishingPoint(Mat frame){

	int n;
	double *aux = (double *) malloc(frame.rows * frame.cols * sizeof(double));
	for(int k = 0; k < frame.rows ; k++ ){
		for(int y = 0; y < frame.cols ; y++ ){
			Vec3b data = frame.at<Vec3b>(k,y);
			aux[k+y*frame.rows] = (double) (data[0] + data[1] + data[2])/3;
		}
	}

	double * out = lsd(&n,aux,frame.rows,frame.cols);
	//cout << out[0] << " " << out[1] << " " << out[2] << " " << out[3] << " " << " " << out[4] << " " << out[5] << " " << out[6] << endl;
	cout << n << endl;

	Segment s(out+7);
	cout << s.x1 << " - " << out[7] << endl;
	/*for(int i=0;i<n;i++)
    {
    	Point p1(out[7*i+1]*3,out[7*i+0]);
		Point p2(out[7*i+3]*3,out[7*i+2]);
		line( frame,p1,p2, Scalar( 0, 0, 0 ),2, 8 );
    }

	imshow("Normal",frame);*/


	Point p(1,2);

	free(aux);

	return p;
}