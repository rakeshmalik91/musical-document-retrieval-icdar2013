#ifndef _STD_H
#define _STD_H

#include <iostream>
#include <dirent.h>
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <list>
	
using namespace std;
using namespace cv;
	
namespace std {
	template<typename T> T min(T x, T y, T z) {
		T m=(x<y)?x:y;
		return (m<z)?m:z;
	}
	template<typename T> T max(T x, T y, T z) {
		T m=(x>y)?x:y;
		return (m>z)?m:z;
	}
	template<typename T> T min(T arr[], size_t size) {
		T m=arr[0];
		for(size_t i=1; i<size; i++)
			if(arr[i]<m)
				m=arr[i];
		return m;
	}
	template<typename T> T max(T arr[], size_t size) {
		T m=arr[0];
		for(size_t i=1; i<size; i++)
			if(arr[i]>m)
				m=arr[i];
		return m;
	}
	template<typename T> T interchange(T& x, T& y) {
		T z=x;
		x=y;
		y=z;
	}
	int numberOfOccuranceFromFirst(string s, char c, char delim='\0') {
		int n=0;
		for(int i=0; i<s.length() && s[i]!=delim; i++)
			if(s[i]==c)
				n++;
		return n;
	}
	string removeExtension(string file) {
		int i;
		for(i=file.length()-1; i>=0; i--)
			if(file[i]=='.')
				break;
		if(i>0)
			return file.substr(0, i);
		else
			return file;
	}
	string getExtension(string file) {
		int i;
		for(i=file.length()-1; i>=0; i--)
			if(file[i]=='.')
				break;
		if(i>0)
			return file.substr(i+1, file.length());
		else
			return "";
	}
	void executeFunctionForAllFile(string fname, void (*func)(string)) {
		DIR* dir=opendir(fname.data());
		if(dir!=NULL) {
			for(dirent *ent; (ent=readdir(dir))!=NULL; ) {
				if((string)ent->d_name=="." || (string)ent->d_name=="..")
					continue;
				executeFunctionForAllFile(fname+"/"+(string)ent->d_name, func);
			}
			closedir(dir);
		} else
			(*func)(fname);
	}
	void sortBubbleDesc(Mat mat, int *index=NULL, int n=0) {
		if(index)
			for(int i=0; i<mat.size().area(); i++)
				index[i]=i;
		if(n<1 || n>=mat.size().area())
			n=mat.size().area()-1;
		for(int i=0; i<n; i++)
			for(int j=mat.size().area()-1; j>=i+1; j--)
				if(((float*)mat.data)[j-1]<((float*)mat.data)[j]) {
					interchange(((float*)mat.data)[j-1], ((float*)mat.data)[j]);
					if(index)
						interchange(index[j-1], index[j]);
				}
	}
	template<typename T> T** new2D(int h, int w, T init=0) {
		T** a=new T*[h];
		for(int i=0; i<h; i++) {
			a[i]=new T[w];
			for(int j=0; j<w; j++)
				a[i][j]=init;
		}
		return a;
	}
	template<typename T> void delete2D(T** a, int h) {
		for(int i=0; i<h; i++)
			delete a[i];
		delete a;
	}
 }

#endif