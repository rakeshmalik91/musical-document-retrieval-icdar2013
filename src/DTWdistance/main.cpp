#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include "iostream.h"
#include <conio.h>
#include "dirent.h"
#include "cvapp.h"
using namespace std;

ImageProcessor *proc = new ImageProcessor;

int main(int argc, char *argv[]){
	int G_H = 60;	int G_W = 300;
	string ImgPath1 = "D:\\Work\\Database\\RFAI\\Training\\Words\\104.bmp";
	IplImage *src1 = cvLoadImage(ImgPath1.c_str(), 0);
	src1 = proc->ImgNormaizeDTW(src1, G_H, G_W, TRUE); 
	//proc->DisplayImg(src1, "ppr");
	float **Signal1 = proc->ImgFeatureSignal(src1);

	string ImgPath2 = "D:\\Work\\Database\\RFAI\\Training\\Words\\106.bmp";
	IplImage *src2 = cvLoadImage(ImgPath2.c_str(), 0);
	src2 = proc->ImgNormaizeDTW(src2, G_H, G_W, TRUE); 
	//proc->DisplayImg(src2, "ppr");
	float **Signal2 = proc->ImgFeatureSignal(src2);


	float dist = 0;
	for(int i = 0; i < 4; i++) 	dist += proc->DTW_Compare(Signal1[i], Signal2[i], G_W, G_W, G_H/2);
	cout<<"DTW distance between two Images is "<<dist;
	return 0;
}
