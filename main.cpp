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

#include "RCartel.hpp"

using namespace cv;
using namespace std;

string DIR_CARTEL = "./Cart/";
string DIR_IMG = "./Test_img/";
string FORMA = "";

bool is_dir(const char* path) ;
double duration=0,totDuration=0;
int cartTrovati = 0;

int main(int argc, char* argv[]) {

	double nciclo = 1;
	Mat frame;
	float media = 0;
	int NCICLI = 20,count=0;
	int auxFound = 0;

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
		if(NCICLI < 1 || NCICLI > 100){
			cout << "Out of range" << endl;
			return 0;
		}
	}

	DIR_IMG += FORMA + "/";
	//namedWindow("Normal", WINDOW_NORMAL);
	//namedWindow("Immaginisenzarumore", WINDOW_NORMAL);

	vector<Mat> cartel;

	struct dirent **namelist;
	int n;
	n = scandir(DIR_CARTEL.c_str(), &namelist, 0, alphasort);
	while (n--) {
		if(is_dir((DIR_CARTEL+namelist[n]->d_name).c_str())){
			continue;
		}
		frame = imread(DIR_CARTEL+namelist[n]->d_name);
		cartel.push_back(frame);
	}
	reverse(cartel.begin(),cartel.end());
	vector<int> founded(cartel.size(),0);

	DIR *d;
	struct dirent *dir;
	d = opendir(DIR_IMG.c_str());
	while ((dir = readdir(d)) != NULL){

		if(is_dir((DIR_IMG + dir->d_name).c_str())){
			continue;
		}

		count++;
		string aux(dir->d_name);
		media = 0;
		vector<int> found;
		auxFound = 0;
		// N cicli di test
		for(int nciclo=0;nciclo < NCICLI;nciclo++) {
			cout << DIR_IMG + aux << endl;
			frame = imread(DIR_IMG + aux);
			//frame = imread(DIR_IMG + "strada.jpg");
			duration = static_cast<double>(cv::getTickCount());
			totDuration  = static_cast<double>(cv::getTickCount());

			RCartel a;
			found = a.lookForSigns(frame.clone(),cartel);

			duration = (static_cast<double>(cv::getTickCount()) - duration) / getTickFrequency();
			totDuration = (static_cast<double>(cv::getTickCount()) - totDuration) / getTickFrequency();
			cout << totDuration << " total Duration " << endl << endl;
			cartTrovati=0;
			media += totDuration;
			waitKey(1000);
		}

		for(int i=0;i<found.size();i++){
			cout << found[i] << endl;
			founded[found[i]]++;
		}

		media = media / NCICLI;

	}
	closedir(d);

	int totalRiconosciuti = 0;
	cout << endl << "N-Input : \t" << count << "\tN Cicli : \t" << NCICLI << endl;
	for(int i=0;i<cartel.size();i++){
		cout << "Cart"<<to_string(i+1)<<" : \t" << founded[i] << endl;
		totalRiconosciuti += founded[i];
	}
	cout << "Riconosciuti : \t" << totalRiconosciuti << endl;

	return 0;
}

bool is_dir(const char* path) {
    struct stat buf;
    lstat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

