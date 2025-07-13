#ifndef _IMAGE_PROCESSING_H
#define _IMAGE_PROCESSING_H
	
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cctype>
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#include <opencv/highgui.hpp>
#include <opencv/ml.h>
#include <dirent.h>
#include "std.h"

using namespace std;
	
namespace image_processing {
	typedef vector<int> Sequence;
	
	typedef float* FeatureVector;
	double euclideanDistance(FeatureVector v1, FeatureVector v2, int n) {
		double sqdist=0;
		for(int i=0; i<n; i++)
			sqdist+=(v1[i]-v2[i])*(v1[i]-v2[i]);
		return sqrt(sqdist);
	}
	
	class ImageFeature {
	protected:
		int dim;
		vector<FeatureVector> sample;
		vector<string> lbl;
	public:
		Sequence seq;
		ImageFeature(int dim) : dim(dim) {}
		ImageFeature(string);
		void add(FeatureVector, string);
		int size() const;
		int dimension() const;
		FeatureVector& operator[](int);
		string& label(int);
		~ImageFeature();
	};
	void ImageFeature::add(FeatureVector v, string l="") {
		sample.push_back(v);
		lbl.push_back(l);
	}
	/* File Format
	 * <n=dimension>
	 * <vec1.comp1> <vec1.comp2> ... <vec1.compn> <vec1.filename>
	 * <vec2.comp1> <vec2.comp2> ... <vec2.compn> <vec2.filename>
	 * ...
	 */
	ImageFeature::ImageFeature(string filename) {
		ifstream in(filename.data());
		if(in==NULL) {
			cerr<<"File "<<filename<<" read error"<<endl;
			exit(0);
		}
		in>>dim;
		while(!in.eof()) {
			FeatureVector v=new float[dim];
			for(int i=0; i<dim; i++)
				in>>v[i];
			sample.push_back(v);
			string l;
			in>>l;
			lbl.push_back(l);
			for(char c=in.get(); !isgraph(c) && c!=EOF; c=in.get());
			if(!in.eof())
				in.unget();
		}
		in.close();
	}
	int ImageFeature::size() const {
		return sample.size();
	}
	int ImageFeature::dimension() const {
		return dim;
	}
	FeatureVector& ImageFeature::operator[](int i) {
		return sample[i];
	}
	string& ImageFeature::label(int i) {
		return lbl[i];
	}
	ImageFeature::~ImageFeature() {
		for(int i=0; i<sample.size(); i++)
			delete sample[i];
	}

	void Img2BinMat(IplImage *img, int **matrix) {
		unsigned char *data=(unsigned char *)(img->imageData);
		for(int i=0;i< img->height;i++) {
			for( int j=0;j<img->width;j++) {
				if( data[j] < 127 )
					matrix[i][j]=0;			
				else 
					matrix[i][j]=1;			
			}
			data+=img->widthStep;
		}
	}
	void cropImage( IplImage* &src, int mincol,int minrow, int width,int height) {
		IplImage *tgt = cvCreateImage( cvSize(width, height), src->depth , src->nChannels );
		cvZero(tgt);
		
		int ncol= src->width*src->nChannels;	
		unsigned char *tgtdata= reinterpret_cast<unsigned char *>(tgt->imageData);
		unsigned char *srcdata= reinterpret_cast<unsigned char *>(src->imageData);
	
		for (int i=0; i<src->height; i++){
			if( i >= minrow && i < minrow + height){
				int k = 0;
				for (int j=0; j<ncol; j+= src->nChannels){
					if( j >= mincol*src->nChannels && j < (mincol + width)*src->nChannels){
						tgtdata[k]=srcdata[j+0];
						tgtdata[k+1]=srcdata[j+1];
						tgtdata[k+2]=srcdata[j+2];
						k+=tgt->nChannels;
					}
				}
				tgtdata+=tgt->widthStep;
			}		
			srcdata+=src->widthStep;
		}
		cvReleaseImage(&src);
		src=tgt;
	}
	void resizeImage( IplImage* &src, int width, int height) {
		IplImage *tgt = cvCreateImage( cvSize(width, height), src->depth, src->nChannels );
		cvResize( src, tgt, CV_INTER_LINEAR );
		cvReleaseImage(&src);
		src=tgt;
	}
	void negative(IplImage* &src){
		IplImage *tgt = cvCreateImage( cvGetSize(src), src->depth, src->nChannels );
		unsigned char *data1=(unsigned char *)(src->imageData);
		unsigned char *data2=(unsigned char *)(tgt->imageData);
		int ncol= src->width * src->nChannels;
		for (int i=0; i<src->height; i++) {
			for (int j=0; j<ncol; j++) {
				data2[j] = 255 - data1[j];			
			}
			data2+=tgt->widthStep;
			data1+=src->widthStep;
		}
		cvReleaseImage(&src);
		src=tgt;
	}
	void fitImage(IplImage* src, IplImage* dst) {
		double srcar=(double)src->width/(double)src->height;
		double dstar=(double)dst->width/(double)dst->height;
		CvSize size;
		if(srcar>dstar)
			size=cvSize(dst->width, dst->width/srcar);
		else
			size=cvSize(dst->height*srcar, dst->height);
		cvZero(dst);
		IplImage* tmp=cvCreateImage(size, dst->depth, dst->nChannels);
		cvResize(src, tmp, CV_INTER_NN);
		cvSetImageROI(dst, cvRect(0, 0, size.width, size.height));
		cvCopyImage(tmp, dst);
		cvResetImageROI(dst);
		cvReleaseImage(&tmp);
	}
	int* verticalProjection(IplImage* img) {
		int* proj=new int[img->width];
		for(int col=0; col<img->width; col++) {
			proj[col]=0;
			for(int row=0; row<img->height; row++)
				proj[col]+=(((uchar*)img->imageData)[img->widthStep*row+col]>=127)?1:0;
		}
		return proj;
	}
	int* horizontalProjection(IplImage* img) {
		int* proj=new int[img->height];
		for(int row=0; row<img->height; row++) {
			proj[row]=0;
			for(int col=0; col<img->width; col++)
				proj[row]+=(((uchar*)img->imageData)[img->widthStep*row+col]>=127)?1:0;
		}
		return proj;
	}
}
	
#endif