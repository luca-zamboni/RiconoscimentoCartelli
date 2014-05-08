
#include "vanishing.h"

VanPoint::WPoint::WPoint(Point p,double w){
	this->p = p;
	this->w = w;
}

void VanPoint::Segment::calc(){
	xmp = (x1+x2) / 2;
	ymp = (y1+y2) / 2;
	slopeDeg = atan((y2-y1)/(x2-x1)) * 180 / M_PI;
	slNorm = (y2-y1)/(x2-x1);
}

VanPoint::Segment::Segment(double* points){
	x1 = 3*points[1];
	y1 = points[0];
	x2 = 3*points[3];
	y2 = points[2];
	w =  sqrt(pow( x2 - x1 ,2) + pow( y2 - y1 ,2));
	p = points[5];
	logNfa = points[6];
	calc();
}

VanPoint::VanPoint(){
	van = Point(0,0);
}

Point VanPoint::vanishingPoint(Mat frame){

	minLength = frame.cols * RATE_M_LENGHT;
	Mat mframe = frame.clone();

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
		if(ctrlSegment(s,frame.cols+4,frame.rows)){
			if(s.slNorm < 0){
				segsx.push_back(s);
			}else{
				segdx.push_back(s);
			}
		}
	}

	vector<Segment> AB;
	for(Segment sdx : segdx){
		AB.push_back(sdx);
	}
	for(Segment ssx : segsx){
		AB.push_back(ssx);
	}

	vector <WPoint> wpnts;
	for(Segment ab : AB){
		for(Segment ba : AB){
			if(ba.x1!=ab.x1){
				wpnts.push_back(getWIntesection(ab,ba));
			}
		}
	}

	/*for(Segment sdx : segdx){
		for(Segment ssx : segsx){
			wpnts.push_back(getWIntesection(ssx,sdx));
		}
	}*/

	double sommaw = 0;
	for(int i=0;i<wpnts.size();i++){
		sommaw += wpnts[i].w;
    }

    Point vani(0,0);
    for(int i=0;i<wpnts.size();i++){
		vani.x += wpnts[i].p.x * wpnts[i].w / sommaw;
		vani.y += wpnts[i].p.y * wpnts[i].w / sommaw;
    }

    if(van.x!=0 && van.y!=0 ){
	    vani.x = (vani.x+van.x)/2;
	    vani.y = (vani.y+van.y)/2;
	}

    for(Segment sdx : segdx){
		line(mframe,Point(sdx.x1,sdx.y1),Point(sdx.x2,sdx.y2),Scalar( 0, 0, 0 ),11,8);
	}
	for(Segment ssx : segsx){
		line(mframe,Point(ssx.x1,ssx.y1),Point(ssx.x2,ssx.y2),Scalar( 0, 0, 0 ),11,8);
	}
	
    circle(mframe,vani,mframe.rows/32.0, Scalar(0,255,255),-1,8);

	imshow("Normal",mframe);

	free(aux);
	free(out);

	van = vani;
	return vani;
}

VanPoint::WPoint VanPoint::getWIntesection(Segment sx, Segment dx){

	double qsx = sx.slNorm * sx.x1 - sx.y1;
	double qdx = dx.slNorm * dx.x1 - dx.y1;

	double sotto = (-sx.slNorm)-(-dx.slNorm);
	double x = (qsx - qdx)/sotto;
	double y = ((-sx.slNorm)*qdx - (-dx.slNorm)*qsx)/sotto;

	double w = pow(sx.w * dx.w,2);
	
	if((sx.slNorm<0 && dx.slNorm>0) || (sx.slNorm>0 && dx.slNorm<0)){
		w = w*2;
	}

	return WPoint(Point(-x,-y),w);

}

bool VanPoint::ctrlSegment(VanPoint::Segment s,double xmax,double ymax){

	if(s.x1 > xmax ||s.x2 > xmax)
		return false;
	
	if(s.w < minLength)
		return false;

	if(abs(s.slopeDeg) < minAlpha || abs(s.slopeDeg) > maxAlpha)
		return false;

	return true;

}

void VanPoint::clean(){
	van = Point(0,0);
}
