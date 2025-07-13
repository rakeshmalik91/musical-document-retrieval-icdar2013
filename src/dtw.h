#ifndef _DTW_H
#define _DTW_H
	
#include "image_processing.h"
#include "component.h"

using namespace std;
using namespace image_processing;
	
namespace image_processing {
	vector<CvPoint> warpingPath(float **DTW, int cols, int rows) {
		vector<CvPoint> path;
		for(int i=rows-1, j=cols-1; i>0 && j>0;) {
			if(i==1)
				j--;
			else if(j==1)
				i--;
			else {
				if(DTW[j][i-1]==min(DTW[j][i-1], DTW[j-1][i], DTW[j-1][i-1]))
					i--;
				else if(DTW[j-1][i]==min(DTW[j][i-1], DTW[j-1][i], DTW[j-1][i-1]))
					j--;
				else
					i--, j--;
				path.push_back(cvPoint(i, j));
			}
		}
		return path;
	}
	double dTWDistance(FeatureVector v1, FeatureVector v2, int n1, int n2, double **d2=NULL) {
		float **DTW=new2D<float>(n1+1, n2+1);
		for(int i=0; i<=n1; i++)
			DTW[i][0]=INFINITY;
		for(int i=0; i<=n2; i++)
			DTW[0][i]=INFINITY;
		DTW[0][0]=0;
		for(int i=1; i<=n1; i++)
			for(int j=1; j<=n2; j++) {
				double d;
				if(d2==NULL)
					d=v1[i-1]-v2[j-1];
				else
					d=d2[i-1][j-1];
				DTW[i][j]=fabs(d)+min(DTW[i-1][j], DTW[i][j-1], DTW[i-1][j-1]);
			}
		float dist=DTW[n1][n2];
		int K=warpingPath(DTW, n1+1, n2+1).size();
		delete2D(DTW, n1+1);
		return dist/K;
	}
	FeatureVector projectionProfile(int **m, int height, int width, int Nfactor) {
		FeatureVector hist=new float[width];
		for(int j = 0; j < width; j++)
			hist[j] = 0;
		for(int i = 0; i < height; i++){			
			for(int j = 0; j < width; j++){
				if( m[i][j] == 1 )
					hist[j]++;
			}
		}
		for(int j = 0; j < width; j++)
			hist[j]/= Nfactor;
		return(hist);
	}
	FeatureVector upperProfile(int **Mat, int height, int width, int Nfactor) {
		FeatureVector hist = new float[width];
		for(int j = 0; j < width; j++)
			hist[j] = 0;
		for(int j = 0; j < width; j++) {			
			for(int i = 0; i < height; i++) {
				if(Mat[i][j] == 1) {
					hist[j] = height - i - 1;
					break;
				}
			}
		}
		for(int j = 0; j < width; j++)
			hist[j]/=Nfactor;
		return(hist);
	}
	FeatureVector lowerProfile(int **Mat, int height, int width, int Nfactor) {
		FeatureVector hist = new float[width];
		for(int j = 0; j < width; j++)
			hist[j] = 0;
		for(int j = 0; j < width; j++) {			
			for(int i = height-1; i >= 0; i--) {
				if( Mat[i][j] == 1) {
					hist[j] = i;
					break;
				}
			}
		}
		for(int j = 0; j < width; j++)
			hist[j]/=Nfactor;
		return(hist);
	}
	FeatureVector transitionFG(int **Mat, int height, int width, int Nfactor) {
		FeatureVector hist = new float[width];
		for(int j = 0; j < width; j++)
			hist[j] = 0;
		for(int j = 0; j < width; j++) {			
			for( int i = 1; i < height; i++) {
				if( Mat[i][j] == 1 && Mat[i-1][j] == 0)
					hist[j]++;
			}
		}
		for(int j = 0; j < width; j++)
			hist[j]/=Nfactor;
		return(hist);
	}
	typedef FeatureVector* DTWSignal;
	DTWSignal imgFeatureSignal(IplImage *src) {
		int **m=new2D<int>(src->height, src->width);
		Img2BinMat(src, m);
		DTWSignal sig=new FeatureVector[4];
		sig[0]=projectionProfile(m, src->height, src->width, src->height);
		sig[1]=upperProfile(m, src->height, src->width, src->height);
		sig[2]=lowerProfile(m, src->height, src->width, src->height);
		sig[3]=transitionFG(m, src->height, src->width, src->height);
		delete2D(m, src->height);
		return sig;
	}
	IplImage* imgNormaizeDTW(IplImage *img, int height, int width, bool Norm) {
		IplImage *src = cvCloneImage(img);
		cvThreshold( src, src, 128, WHITE, CV_THRESH_OTSU); 
		negative(src);
		ListPix CntrPix = ContourPixelAll(src, FALSE);
		MBoundingBox ImgBox = BoundingBox(CntrPix);
		cropImage(src, ImgBox.TopLftV.x, ImgBox.TopLftV.y, ImgBox.HW.width, ImgBox.HW.height);
		resizeImage(src, width, height);
		cvThreshold( src, src, 128, WHITE, CV_THRESH_OTSU);
		return(src);
	}
	double dTWDistance(IplImage* src1, IplImage* src2, int G_H, int G_W) {
		IplImage* src1n=imgNormaizeDTW(src1, G_H, G_W, true);
		DTWSignal sig1=imgFeatureSignal(src1n);
		IplImage* src2n=imgNormaizeDTW(src2, G_H, G_W, true);
		DTWSignal sig2=imgFeatureSignal(src2n);
		double dist=0.0, **d=new2D<double>(G_W, G_W);
		for(int i=0; i<G_W; i++)
			for(int j=0; j<G_W; j++)
				for(int k=0; k<4; k++)
					d[i][j]+=(sig1[k][i]-sig2[k][j])*(sig1[k][i]-sig2[k][j]);
		for(int i=0; i<4; i++)
			dist+=dTWDistance(sig1[i], sig2[i], G_W, G_W, d);
		delete2D(d, G_W);
		delete2D(sig1, 4);
		delete2D(sig2, 4);
		cvReleaseImage(&src1n);
		cvReleaseImage(&src2n);
		return dist/4;
	}
}

#endif