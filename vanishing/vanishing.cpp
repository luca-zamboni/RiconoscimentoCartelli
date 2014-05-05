
#include "vanishing.h"

using namespace std;

class Segment{
	private:
		void calc(){
			xmp = (x1+x2) / 2;
			ymp = (y1+y2) / 2;
			slopeDeg = atan((y2-y1)/(x2-x1)) * 180 / M_PI;
			slNorm = (y2-y1)/(x2-x1);
		}
	public:
		double x1,x2,y1,y2,p,w,logNfa,xmp,ymp,slopeDeg,slNorm;
		Segment(double* points){
			x1 = 3*points[1];
			y1 = points[0];
			x2 = 3*points[3];
			y2 = points[2];
			w =  sqrt(pow( x2 - x1 ,2) + pow( y2 - y1 ,2));
			p = points[5];
			logNfa = points[6];
			calc();
		}
};

class WPoint{
	public:
		Point p;
		double w;
		WPoint(Point p,double w){
			this->p = p;
			this->w = w;
		}
};

const float RATE_M_LENGHT = 0.03;
float minLength;
int minAlpha = 10;
int maxAlpha = 70;

bool ctrlSegment(Segment s,double xmax,double ymax);
WPoint getWIntesection(Segment sx, Segment dx);

Point vanishingPoint(Mat frame,Point van){

	minLength = frame.cols * RATE_M_LENGHT;

	int n;
	double *aux = (double *) malloc(frame.rows * frame.cols * sizeof(double));
	for(int k = 0; k < frame.rows ; k++ ){
		for(int y = 0; y < frame.cols ; y++ ){
			Vec3b data = frame.at<Vec3b>(k,y);
			aux[k+y*frame.rows] = (double) (data[0] + data[1] + data[2])/3;
		}
	}

	double * out = lsd(&n,aux,frame.rows,frame.cols);
	vector <Segment> segsx;
	vector <Segment> segdx;
	double xm = frame.rows / 2;
	for(int i=0;i<n;i++){
		Segment s(out+(7*i));
		if(ctrlSegment(s,frame.cols,frame.rows)){
			if(s.slNorm < 0){
				segsx.push_back(s);
			}else{
				segdx.push_back(s);
			}
		}
	}
	
	vector <WPoint> wpnts;
	for(Segment sdx : segdx){
		for(Segment ssx : segsx){
			wpnts.push_back(getWIntesection(ssx,sdx));
		}
	}

	double sommaw = 0;
	for(int i=0;i<wpnts.size();i++){
		sommaw += wpnts[i].w;
    }

    Point vani(0,0);
    for(int i=0;i<wpnts.size();i++){
		vani.x += wpnts[i].p.x * wpnts[i].w / sommaw;
		vani.y += wpnts[i].p.y * wpnts[i].w / sommaw;
    }

    for(Segment sdx : segdx){
		line( frame,Point(sdx.x1,sdx.y1),Point(sdx.x2,sdx.y2),Scalar( 0, 0, 0 ),11,8 );
	}
	for(Segment ssx : segsx){
		line( frame,Point(ssx.x1,ssx.y1),Point(ssx.x2,ssx.y2),Scalar( 0, 0, 0 ),11,8 );
	}
	
    circle( frame,
         vani,
         frame.rows/32.0,
         Scalar( 0, 255, 255 ),
         -1,
         8 );

	imshow("Normal",frame);

	free(aux);
	free(out);

	return vani;
}

WPoint getWIntesection(Segment sx, Segment dx){

	double qsx = sx.slNorm * sx.x1 - sx.y1;
	double qdx = dx.slNorm * dx.x1 - dx.y1;

	double sotto = (-sx.slNorm)-(-dx.slNorm);
	double x = (qsx - qdx)/sotto;
	double y = ((-sx.slNorm)*qdx - (-dx.slNorm)*qsx)/sotto;

	double w = pow(sx.w * dx.w,2);

	return WPoint(Point(-x,-y),w);

}

bool ctrlSegment(Segment s,double xmax,double ymax){

	if(s.x1 > xmax || s.x2 > xmax)
		return false;
	
	if(s.w < minLength)
		return false;

	if(abs(s.slopeDeg) < minAlpha || abs(s.slopeDeg) > maxAlpha)
		return false;

	return true;

}
