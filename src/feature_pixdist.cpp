#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <dirent.h>
#include "image_processing.h"

using namespace std;
using namespace image_processing;

void getPixelDistribution(IplImage* img, int pix_dist[]) {
	for(int i=0; i<img->height+img->width; i++)
		pix_dist[i]=0;
	for(int i=0; i<img->height; i++) {
		for(int j=0;j<img->width; j++) {
			if(((unsigned char*)img->imageData)[i*img->width+j]>127) {
				pix_dist[i]++;
				pix_dist[img->height+j]++;
			}
		}
	}
}
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
	int dim=60;
	datafile<<2*dim<<endl;
	dirent *file;
	IplImage* resized_img=cvCreateImage(cvSize(dim, dim), IPL_DEPTH_8U, 1);
	while((file=readdir(dir))!=NULL) {
		if((string)file->d_name=="." || (string)file->d_name=="..")
			continue;
		IplImage* img=cvLoadImage((dir_name+"/"+(string)file->d_name).data(), CV_LOAD_IMAGE_GRAYSCALE);
		if(img!=NULL) {
			fitImage(img, resized_img);
			int pix_dist[2*dim];
			getPixelDistribution(resized_img, pix_dist);
			for(int i=0; i<2*dim; i++)
				datafile<<pix_dist[i]<<" ";
			datafile<<dir_name<<"/"<<file->d_name<<endl;
			cvReleaseImage(&img);
		}
	}
	cvReleaseImage(&resized_img);
	datafile.close();
	closedir(dir);
}