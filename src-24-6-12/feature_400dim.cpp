#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <dirent.h>
#include "400dimfeature.h"

using namespace std;


int main(int argc, char* argv[]) {
	if(argc<3) {
		cerr<<"Arguments missing"<<endl
			<<"Usage : feature <input_directory> <output_data_file>"<<endl;
		exit(0);
	}
	const string dir_name=argv[1];
	DIR *dir=opendir(dir_name.data());
	if(dir==NULL) {
		cerr<<"Input directory "<<dir_name<<" open error"<<endl;
		exit(0);
	}
	const string datafile_name=argv[2];
	ofstream datafile(datafile_name.data());
	if(datafile==NULL) {
		cerr<<"Output file "<<datafile_name<<" write error"<<endl;
		exit(0);
	}
	datafile<<400<<endl;
	dirent *file;
	while((file=readdir(dir))!=NULL) {
		if((string)file->d_name=="." || (string)file->d_name=="..")
			continue;
		IplImage* img=cvLoadImage((dir_name+"/"+(string)file->d_name).data(), CV_LOAD_IMAGE_GRAYSCALE);
		if(img!=NULL) {
			FeatureVector f=dimension400feature(img);
			for(int i=0; i<400; i++)
				datafile<<f[i]<<" ";
			delete f;
			cvReleaseImage(&img);
			datafile<<dir_name<<"/"<<file->d_name<<endl;
		}
	}
	datafile.close();
	closedir(dir);
}