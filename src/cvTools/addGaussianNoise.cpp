#include <iostream>
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#include <opencv/highgui.hpp>
#include "../std.h"

using namespace std;
using namespace cv;

void addNoise(string fname) {
	Mat img=imread(fname);
	if(img.empty()) {
		cerr<<fname<<" : File read error or not an image"<<endl;
		return;
	}
	cout<<"Processing "<<fname<<"..."<<endl;
	Mat img_yuv;
	cvtColor(img, img_yuv, CV_BGR2YCrCb);
	vector<Mat> planes;
	split(img_yuv, planes);
	Mat noise(img.size(), CV_8U);
	randn(noise, Scalar::all(0), Scalar::all(235));
	GaussianBlur(noise, noise, Size(3, 3), 0.5, 0.5);
	const double brightness_gain = 0;
	const double contrast_gain = 1.7;
	addWeighted(planes[0], contrast_gain, noise, 1, -128 + brightness_gain, planes[0]);
	const double color_scale = 0.5;
	planes[1].convertTo(planes[1], planes[1].type(), color_scale, 128*(1-color_scale));
	planes[2] = Mat_<uchar>(planes[2]*color_scale + 128*(1-color_scale));
	planes[0] = planes[0].mul(planes[0], 1./255);
	merge(planes, img_yuv);
	cvtColor(img_yuv, img, CV_YCrCb2BGR);
	imwrite(fname, img);
}

int main(int argc, char* argv[]) {
	for(int i=1; i<argc; i++)
		executeFunctionForAllFile(argv[i], addNoise);
}