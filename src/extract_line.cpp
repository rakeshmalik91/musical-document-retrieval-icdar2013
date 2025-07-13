#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "component.h"

using namespace std;
using namespace image_processing;

ListComp getLines(IplImage* img) {
	IplImage* img_bin=cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);	
	cvThreshold(img, img_bin, 128, 255, CV_THRESH_BINARY);
	ListComp comp_list=BinImg2Comp(img_bin);
	compFilter(comp_list, 10, 20, 5);
	compJoinBoundingBoxOverlap(comp_list);
	cvReleaseImage(&img_bin);
	return comp_list;
}

int main(int argc, char* argv[]){
	string indir=(argc<2)?"DATA/TEXT_SR":argv[1];
	string outdir=(argc<3)?"DATA/TEXTLINE":argv[2];
	mkdir(outdir.data());
	DIR *dir=opendir(indir.data());
	if(dir!=NULL) {
		int line_no=0;
		for(dirent *file; (file=readdir(dir))!=NULL; )
		{
			if((string)file->d_name=="." || (string)file->d_name=="..")
				continue;
			string in_name=file->d_name;
			string in_path=(indir+"/"+in_name).data();
			cout<<"processing "<<in_path<<" ..."<<endl;
			IplImage* img = cvLoadImage(in_path.data(), CV_LOAD_IMAGE_GRAYSCALE);
			if(img==NULL) {
				cerr<<"File or directory "<<in_path<<" open error"<<endl;
				continue;
			}
			int *proj=horizontalProjection(img);
			double max_proj=max(proj, img->height), threshold1=0.2, threshold2=0.05;
			int ymin=0;
			for(int ymax=0; ymax<img->height; ymax++) {
				if(proj[ymax]<=threshold2)
					ymin=ymax;
				else if(proj[ymax]/max_proj<=threshold1) {
					if(ymax-ymin+1>img->height/20) {
						cvSetImageROI(img, cvRect(0, ymin, img->width, ymax-ymin+1));
						char out_path[50];
						sprintf(out_path, "%s/%4.4d", outdir.data(), line_no++);
						ofstream output(((string)out_path+".txt").data());
						output<<in_name<<" "<<0<<" "<<ymin<<endl;
						cvSaveImage(((string)out_path+".jpg").data(), img);
						cvResetImageROI(img);
						output.close();
					}
					ymin=ymax;
				}
			}
			delete proj;
			cvReleaseImage(&img);
		}
	}
}
