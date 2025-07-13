#include <iostream>
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#include <opencv/highgui.hpp>
#include "../std.h"

using namespace std;
using namespace cv;

void toSingleChannel(string fname) {
	Mat img=imread(fname, CV_LOAD_IMAGE_GRAYSCALE);
	if(img.empty()) {
		cerr<<fname<<" : File read error or not an image"<<endl;
		return;
	}
	cout<<"Processing "<<fname<<"..."<<endl;
	imwrite(removeExtension(fname)+".jpg", img);
}

int main(int argc, char* argv[]) {
	for(int i=1; i<argc; i++)
		executeFunctionForAllFile(argv[i], toSingleChannel);
}