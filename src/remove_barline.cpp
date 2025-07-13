#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <dirent.h>
#include "component.h"

using namespace std;
using namespace image_processing;

int main(int argc, char* argv[]){
	string indir=(argc<2)?"DATA/TEXT":argv[1];
	string outdir=(argc<3)?"DATA/TEXT_BR":argv[2];
	DIR *dir=opendir(indir.data());
	if(dir!=NULL) {
		int comp_num=0;
		for(dirent *file; (file=readdir(dir))!=NULL; )
		{
			if((string)file->d_name=="." || (string)file->d_name=="..")
				continue;
			string fname=file->d_name;
			string in_path=indir+"/"+fname;
			string out_path=outdir+"/"+fname;
			cout<<"processing "<<in_path<<" ..."<<endl;
			IplImage* img = cvLoadImage(in_path.data(), CV_LOAD_IMAGE_GRAYSCALE);
			if(img==NULL) {
				cerr<<"File or directory "<<in_path<<" open error"<<endl;
				continue;
			}
			IplImage* img2=cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
			cvCanny(img, img2, 50, 200, 3);
			CvMemStorage* storage=cvCreateMemStorage(0);
			CvSeq *lines=cvHoughLines2(img2, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 127, 50, 300);
			for( int i = 0; i < lines->total; i++ ) {
				CvPoint* line=(CvPoint*)cvGetSeqElem(lines, i);
				double dy=(line[0].y-line[1].y), dx=(line[0].x-line[1].x);
				double theta=fabs(atan(dy/dx)*180/CV_PI);
				//cout<<theta<<endl;
				if(theta>80 && theta<100)
					cvLine(img, line[0], line[1], cvScalar(0), 3);
			}
			//cvNamedWindow("a", 0);
			//cvMoveWindow("a", 0, 0);
			//cvResizeWindow("a", 1280, 650);
			//cvShowImage("a", img);
			cvWaitKey();
			cvSaveImage(out_path.data(), img);
			cvReleaseImage(&img);
		}
	}
}
