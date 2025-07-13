#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include "std.h"

using namespace std;

IplImage *text, *query;
string textname, queryname;
string twn, qwn;

void mouse_callback(int event, int x, int y, int flags, void* param) {
	static int x0, y0;
	if(event==CV_EVENT_LBUTTONUP && !(x0==x && y0==y)) {
		cvRectangle(text, cvPoint(x0, y0), cvPoint(x, y), cvScalar(127), 3);
		cvShowImage(twn.data(), text);
		ofstream gt(("DATA/GROUND_TRUTH/"+textname+".txt").data(), ios::app);
		gt<<queryname<<" "<<x0<<" "<<y0<<" "<<x<<" "<<y<<endl;
		gt.close();
		cout<<textname<<" "<<queryname<<" "<<x0<<" "<<y0<<" "<<x<<" "<<y<<endl;
	} else if(event==CV_EVENT_LBUTTONDOWN) {
		x0=x;
		y0=y;
	}
}

int main(int argc, char* argv[]){
	int start=0;
	if(argc==2)
		start=atoi(argv[1]);
	int i=0;
	string textdirname="DATA/TEXT_SR";
	DIR* textdir=opendir(textdirname.data());
	for(dirent *textfile; (textfile=readdir(textdir))!=NULL; ) {
		if((string)textfile->d_name=="." || (string)textfile->d_name=="..")
			continue;
		if(i++<start)
			continue;
		string querydirname="DATA/QUERY";
		DIR* querydir=opendir(querydirname.data());
		for(dirent *queryfile; (queryfile=readdir(querydir))!=NULL; ) {
			if((string)queryfile->d_name=="." || (string)queryfile->d_name=="..")
				continue;
			textname=removeExtension(textfile->d_name);
			queryname=removeExtension(queryfile->d_name);
			text=cvLoadImage((textdirname+"/"+(string)textfile->d_name).data(), CV_LOAD_IMAGE_GRAYSCALE);
			query=cvLoadImage((querydirname+"/"+(string)queryfile->d_name).data(), CV_LOAD_IMAGE_GRAYSCALE);
			twn="Text "+textname;
			qwn="Query "+queryname;
			cvNamedWindow(qwn.data(), 0);
			cvNamedWindow(twn.data(), 0);
			cvMoveWindow(qwn.data(), 0, 0);
			cvMoveWindow(twn.data(), 200, 0);
			cvResizeWindow(qwn.data(), 180, 30);
			cvResizeWindow(twn.data(), 1100, 700);
			cvSetMouseCallback(twn.data(), mouse_callback);
			cvShowImage(twn.data(), text);
			cvShowImage(qwn.data(), query);
			cvWaitKey(0);
			cvReleaseImage(&text);
			cvReleaseImage(&query);
			cvDestroyWindow(qwn.data());
			cvDestroyWindow(twn.data());
		}
		closedir(querydir);
	}
	closedir(textdir);
}
