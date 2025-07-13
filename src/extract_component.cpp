#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <dirent.h>
#include "component.h"

using namespace std;
using namespace image_processing;

ListComp getComponentsUnsorted(IplImage* img) {
	IplImage* img_bin=cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);	
	cvThreshold(img, img_bin, 128, 255, CV_THRESH_BINARY);
	cvDilate(img_bin, img_bin, NULL, 4);
	ListComp comp_list=BinImg2Comp(img_bin);
	compJoinBoundingBoxOverlap(comp_list);
	compFilter(comp_list, 10, 20, 5, 5);
	cvReleaseImage(&img_bin);
	return comp_list;
}

int main(int argc, char* argv[]){
	if(argc<3) {
		cerr<<"Arguments missing"<<endl
			<<"Usage : extract_component <input_folder> <output_folder>"<<endl;
		exit(0);
	}
	DIR *dir=opendir(argv[1]);
	if(dir!=NULL) {
		int image_num=0;
		for(dirent *file; (file=readdir(dir))!=NULL; )
		{
			if((string)file->d_name=="." || (string)file->d_name=="..")
				continue;
			const char* in_name=file->d_name;
			const char* in_path=((string)argv[1]+"/"+(string)in_name).data();
			cout<<"processing "<<in_path<<" ..."<<endl;
			IplImage* img = cvLoadImage(in_path, CV_LOAD_IMAGE_GRAYSCALE);
			if(img==NULL) {
				cerr<<"File or directory "<<in_path<<" open error"<<endl;
				continue;
			}
			ListComp comp_list=getComponentsUnsorted(img);
			int comp_num=0;
			for(ListComp::iterator i=comp_list.begin(); i!=comp_list.end(); i++) {
				char out_name[100];
				sprintf(out_name, "%s/img%.5d_comp%.5d.jpg", argv[2], image_num, comp_num++);
				saveComp(out_name, img, *i);
			}
			cvReleaseImage(&img);
			image_num++;
		}
	}
}
