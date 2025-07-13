#ifndef _SOM_LOADER_H
#define _SOM_LOADER_H
	
#include <iostream>
#include <fstream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "dtw.h"
#include "asm.h"
	
using namespace std;
using namespace image_processing;
	
namespace image_processing {
	class SOM {
	protected:
		int width, height, n;
		string topology, neighbour;
		FeatureVector **som;
		string **filename;
	private:
		double** createCostMatrix(ImageFeature&, ImageFeature&) const;
	public:
		SOM(const char*);
		CvPoint getCluster(FeatureVector) const;
		Sequence createSequence(ImageFeature&) const;
		MatchList search(ImageFeature&, ImageFeature&, double) const ;
		void visualize(const char*);
		~SOM();
	};
	/* FILE FORMAT :
	 * <n> <topology=hexa/rect> <width> <height> <neighbour=bubble/gaussian>
	 * <v[0][0][0]> <v[0][0][1]> ... <v[0][0][n-1]> <filename[0][0]> 
	 * <v[0][1][0]> <v[0][1][1]> ... <v[0][1][n-1]> <filename[0][1]> 
	 * ...
	 * <v[height-1][width-1][0]> <v[height-1][width-1][1]> ... <v[height-1][width-1][n-1]> <filename[height-1][width-1]> 
	 * 
	 */
	SOM::SOM(const char* map_filename) {
		ifstream in(map_filename);
		if(in==NULL) {
			cerr<<"file "<<map_filename<<" read error"<<endl;
			exit(0);
		}
		in>>n>>topology>>width>>height>>neighbour;
		som=new FeatureVector*[height];
		filename=new string*[height];
		for(int i=0; i<height; i++) {
			som[i]=new FeatureVector[width];
			filename[i]=new string[width];
			for(int j=0; j<width; j++)
				som[i][j]=new float[n];
		}
		while(!in.eof()) {
			for(int y=0; y<height; y++) {
				for(int x=0; x<width; x++) {
					for(int i=0; i<n; i++)
						in>>som[y][x][i];
					in>>filename[y][x];
				}
			}
			in.ignore(2);
		}
		in.close();
	}
	CvPoint SOM::getCluster(FeatureVector v) const {
		double min=INFINITY;
		CvPoint minpt;
		for(int y=0; y<height; y++) {
			for(int x=0; x<width; x++) {
				double dist=dTWDistance(v, som[y][x], n, n);
				if(dist<min) {
					min=dist;
					minpt=cvPoint(x, y);
				}
			}
		}
		return minpt;
	}
	Sequence SOM::createSequence(ImageFeature &f) const {
		Sequence seq(f.size());
		for(int i=0; i<f.size(); i++) {
			CvPoint pt=getCluster(f[i]);
			seq[i]=pt.y*width+pt.x;
		}
		f.seq=seq;
		return seq;
	}
	double** SOM::createCostMatrix(ImageFeature &text, ImageFeature &query) const {
		CvPoint ST[text.size()], SQ[query.size()];
		for(int i=0; i<query.size(); i++)
			SQ[i]=getCluster(query[i]);
		for(int j=0; j<text.size(); j++)
			ST[j]=getCluster(text[j]);
		double maxsomdist=sqrt((width-1)*(width-1)+(height-1)*(height-1));
		double **cost=new2D<double>(query.size(), text.size());
		for(int i=0; i<query.size(); i++)
			for(int j=0; j<text.size(); j++)
				cost[i][j]=sqrt((SQ[i].x-ST[j].x)*(SQ[i].x-ST[j].x)+(SQ[i].y-ST[j].y)*(SQ[i].y-ST[j].y))/maxsomdist;
		return cost;
	}
	MatchList SOM::search(ImageFeature &text, ImageFeature &query, double max_dist) const {
		double **cost=createCostMatrix(text, query);
		MatchList match=approximateStringMatch(cost, text.size(), query.size(), max_dist, false);
		delete2D(cost, query.size());
		return match;
	}
	void SOM::visualize(const char *fname) {
		IplImage* output=cvCreateImage(cvSize(100*width, 100*height), IPL_DEPTH_8U, 3);
		for(int y=0; y<height; y++)
			for(int x=0; x<width; x++) {
				IplImage* img=cvLoadImage(filename[y][x].data());
				if(img==NULL) {
					cerr<<"Cannot open image "<<filename[y][x]<<endl;
					exit(0);
				}
				cvSetImageROI(output, cvRect(100*x, 100*y, 100, 100));
				cvResize(img, output);
				cvResetImageROI(output);
				cvReleaseImage(&img);
			}
		cvSaveImage(fname, output);
		cvReleaseImage(&output);
	}
	SOM::~SOM() {
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++)
				delete som[i][j];
			delete som[i];
			delete filename[i];
		}
		delete som;
		delete filename;
	}
}

#endif