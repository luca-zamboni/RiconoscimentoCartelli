
#include "RCartel.hpp"

RCartel::GeomSignal::GeomSignal(string nname, Mat iimage_ROI, Rect rrect_ROI) {
	name = nname;
	image_ROI = iimage_ROI;
	rect_ROI = rrect_ROI;
}

RCartel::RCartel::ROI_Rect::ROI_Rect(cv::Rect RROI, vector<Point> RRect) {
	ROI = RROI;
	rect = RRect;
}

RCartel::RCartel(){
	van = true;
	vani = Point(0,0);
	ERROR_LEN = 1 / 2.5;
	DISCARD_CORNERS_PIXEL = 30;
	PROPORZ_CARTELLO_SEGNALE = 1.3;
	PROPORZ_LARGHEZZA_ROI = 2.5;
	focL = 1394.589220272376;
	objSIZE = 4;
	minEdge = focL * objSIZE / 300;
	minArea = minEdge * minEdge;
	maxEdge = focL * objSIZE / 30;
	maxArea = maxEdge * maxEdge;
	MINPRECISION = 0.70;
	PCVANI = 0.4;
	duration=0,totDuration=0;
	cartTrovati = 0;
	CONST_RESIZE = 2;
}

void RCartel::cleanVanischingPoint(){
	van = true;
	vani = Point(0,0);
}

vector<int> RCartel::lookForSigns(Mat frame,vector<Mat> cartels){

	//resize(frame,frame,Size(frame.cols*2,frame.rows*2));

	Mat fG,fG2;
	Mat normal;
	cvtColor(frame, fG, CV_BGR2GRAY);
	cvtColor(frame, fG2, CV_BGR2GRAY);

	normal = frame.clone();

	/*Point angUnder(0,0);
	Point angOver(0,0);*/

	VanPoint vaniCalculator;

	if(van){
		vani = vaniCalculator.vanishingPoint(fG);
		van = false;
	}

	Point angOver(std::max((int)(vani.x - fG.cols * PCVANI),0),std::max((int)(vani.y - fG.rows * PCVANI),0));
	Point angUnder(std::min((int)(vani.x + fG.cols * PCVANI),fG.cols),
		std::min((int)(vani.y + fG.rows * PCVANI),fG.rows));
	
	fG = fG.colRange(angOver.x,angUnder.x).rowRange(angOver.y,angUnder.y);
	circle(frame,vani,10,Scalar(255,255,255),CV_FILLED,8,0);
	imshow("sd",frame);

	//resize(fG,fG,Size(fG.cols*2,fG.rows*2));
	bool ctrl= true;
	vector<int> ret = searchSign(fG,cartels);
	for(int i=0;i<ret.size();i++){
		if(ret[i]>=0){
			ctrl = false;
		}
	}
	if(ctrl){
		ret = searchSign(fG2,cartels);
		van=true;
	}
	return ret;
}

vector<int> RCartel::searchSign(Mat frame,vector<Mat> cartels){

	Mat fG = frame.clone();
	//imshow("sd",fG);

	int numRoi = 0;
	int numTri = 0;
	int numCir = 0;
	int n;

	vector<vector<Point> > squares;
	vector<ROI_Rect*> vectROI_Rect;
	vector<GeomSignal*> geomSignals;

	int N_ROI = 0;
	int N_Signal = 0;
	findSquares(fG, squares);

	vector<Point>* curr_square;
	Rect ROI;
	for (size_t i = 0; i < squares.size(); i++) {
		curr_square = &squares[i];

		ROI = cv::boundingRect(cv::Mat(*curr_square));

		Point centerROI(ROI.x + ROI.width / 2, ROI.y + ROI.height / 2);
		if (vectROI_Rect.size() == 0) {
			vectROI_Rect.push_back(new ROI_Rect(ROI, *curr_square));
		} else {
			bool flag = false;
			bool flag1 = true;
			vector<ROI_Rect*>::reverse_iterator rit_ROI_Rect;
			int ii = vectROI_Rect.size() - 1;
			for (rit_ROI_Rect = vectROI_Rect.rbegin();
					rit_ROI_Rect != vectROI_Rect.rend(); ++rit_ROI_Rect) {
				Point centerROI_rit_ROI_Rect(
						(*rit_ROI_Rect)->getROI().x
								+ (*rit_ROI_Rect)->getROI().width / 2,
						(*rit_ROI_Rect)->getROI().y
								+ (*rit_ROI_Rect)->getROI().height / 2);
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
					if((*rit_ROI_Rect)->getROI().contains(centerROI)){
						flag1 = false;
					}else{
						if (!flag)
							flag = true;
					}
				}
			}
			if (flag) {
				if(flag1){
					vectROI_Rect.push_back(new ROI_Rect(ROI, *curr_square));
				}
			}
		}
	}

	//cout << "N ROI_Rect: " << vectROI_Rect.size() << endl;
	N_ROI = vectROI_Rect.size();
	int color;
	vector<ROI_Rect*>::iterator it;
	vector<ROI_Rect*>::iterator it1;
	vector<Point> vp;

	vector<int> ret;
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

		warpPerspective(fG, perspective, matrixPerspectiveTransform,
				perspective.size(), INTER_LINEAR, BORDER_CONSTANT, 0);

		/*namedWindow("perspective", WINDOW_NORMAL);
		imshow("perspective", perspective);*/

		Rect ROI = (*it1)->getROI();

		ret.push_back(findGeometricSignal(perspective, ROI, geomSignals,cartels));
	}
	//cout << "N segnali trovati: " << geomSignals.size() << endl;

	vector<GeomSignal*>::reverse_iterator rit;
	int i = geomSignals.size() - 1;
	for (rit = geomSignals.rbegin(); rit != geomSignals.rend(); ++rit) {
		delete *rit;
		geomSignals[i] = geomSignals[geomSignals.size() - 1];
		geomSignals.pop_back();
		i--;
	}

	//imshow("Immagini", frame);

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

double RCartel::angle(Point pt1, Point pt2, Point pt0) {
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2)
			/ sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

void RCartel::findSquares(const Mat& image, vector<vector<Point> >& squares) {

	squares.clear();

	Mat mimage = image.clone();
	Mat pyr, timg, gray0(image.size(), CV_8U), gray;
	Mat cl = image.clone();

	resize(cl,cl,Size(cl.cols*CONST_RESIZE,cl.rows*CONST_RESIZE));

	pyrDown(cl, pyr, Size(cl.cols / 2, cl.rows / 2));
	pyrUp(pyr, timg, cl.size());

	vector<vector<Point> > contours;

	Canny(timg, gray, 250,300 , 5);
	dilate(gray, gray, Mat(), Point(-1, -1));
	vector<Vec4i> hierarchy;
	findContours( gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


	//imshow("Immaginisenzarumore",timg);

	vector<Point> approx;

	for (size_t i = 0; i < contours.size(); i++) {
		for(int y = 0;y<contours[i].size();y++){
			contours[i][y].x = contours[i][y].x/CONST_RESIZE;
			contours[i][y].y = contours[i][y].y/CONST_RESIZE;
		}
  		drawContours( mimage, contours, i, Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );
  		approxPolyDP(Mat(contours[i]), approx,
  				arcLength(Mat(contours[i]), true) * 0.02, true);

  		if (approx.size() == 4 /*&& contourArea(Mat(approx))*1000 > minArea*/
  				&& contourArea(Mat(approx)) < maxArea
  				/*&& isContourConvex(Mat(approx))*/){
  			double maxCosine = 0;
  
  			/*Rect ROI;
  			ROI = cv::boundingRect(cv::Mat(approx));
  			Point centerROI(ROI.x + ROI.width / 2, ROI.y + ROI.height / 2);
  
  			rectangle( mimage, ROI.tl(), ROI.br(), Scalar(255,255,255), 2, 8);*/
  
  
  			for (int j = 2; j < 5; j++) {
  				// find the maximum cosine of the angle between joint edges
  				double cosine = fabs(
  						angle(approx[j % 4], approx[j - 2], approx[j - 1]));
  				maxCosine = MAX(maxCosine, cosine);
  			}
  
  			if (maxCosine < 0.5) {
  
  				std::stable_sort(approx.begin(), approx.end(), YComparator);
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
	//imshow("Immaginisenzarumore",mimage);
}

// the function draws all the squares in the image
void RCartel::drawSquares(Mat& image, const vector<vector<Point> >& squares) {
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
	//imshow("ciao", image);
}

//controlla se un quadrato è duplicato
bool RCartel::isDuplicatedSquare(const vector<Point>& toCheck,
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

double RCartel::getDistance(const Point& a, const Point& b) {
	return sqrt(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
}

int RCartel::findGeometricSignal(Mat& img, Rect& ROI, vector<RCartel::GeomSignal*>& geomeSignals,vector<Mat> cartels) {

	/*namedWindow("Mat1", WINDOW_NORMAL);
	imshow("Mat1",img);*/
	int indexMax=-1;
	double maxVal=-1;
	for(int i = 0;i<cartels.size();i++){
		cvtColor(cartels[i], cartels[i], CV_BGR2GRAY);
		double diff = diffXorMat(cartels[i],img);
		if(max(diff,maxVal) == diff && diff > MINPRECISION){
			maxVal = diff; indexMax=i;
		}
	}
	string name = "cart"+to_string(indexMax);
	geomeSignals.push_back(new GeomSignal(name, img, ROI));
	return indexMax;
	
}