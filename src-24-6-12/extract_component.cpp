#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <dirent.h>
#include "component.h"

using namespace std;


int main(int argc, char* argv[]){
	if(argc<3) {
		cerr<<"Arguments missing"<<endl
			<<"Usage : extract_component <input_folder/file> <output_folder>"<<endl;
		exit(0);
	}
	DIR *dir=opendir(argv[1]);
	if(dir==NULL) {
		const char* in_path=argv[1];
		cout<<"processing "<<in_path<<" ..."<<endl;
		IplImage* img = cvLoadImage(in_path, CV_LOAD_IMAGE_GRAYSCALE);
		if(img==NULL) {
			cerr<<"File or directory "<<in_path<<" open error"<<endl;
			exit(0);
		}
		ListComp comp_list=getComponents(img);
		int comp_num=0;
		for(ListComp::iterator i=comp_list.begin(); i!=comp_list.end(); i++) {
			int x=i->B_Box.TopLftV.x;
			int y=i->B_Box.TopLftV.y;
			int width=i->B_Box.BotRgtV.x-x;
			int height=i->B_Box.BotRgtV.y-y;
			cvSetImageROI(img, cvRect(x, y, width, height));
			char out_name[100];
			sprintf(out_name, "%s/img_comp%.5d.jpg", argv[2], comp_num++);
			cvSaveImage(out_name, img);
			cvResetImageROI(img);
		}
		cvReleaseImage(&img);
	} else {
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
			ListComp comp_list=getComponents(img);
			int comp_num=0;
			for(ListComp::iterator i=comp_list.begin(); i!=comp_list.end(); i++) {
				int x=i->B_Box.TopLftV.x;
				int y=i->B_Box.TopLftV.y;
				int width=i->B_Box.BotRgtV.x-x;
				int height=i->B_Box.BotRgtV.y-y;
				cvSetImageROI(img, cvRect(x, y, width, height));
				char out_name[100];
				sprintf(out_name, "%s/img%.5d_comp%.5d.jpg", argv[2], image_num, comp_num++);
				cvSaveImage(out_name, img);
				cvResetImageROI(img);
			}
			cvReleaseImage(&img);
			image_num++;
		}
	}
}
