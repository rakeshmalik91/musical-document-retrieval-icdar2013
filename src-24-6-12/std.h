#ifndef _STD_H
#define _STD_H
	
#include <iostream>
#include <dirent.h>
#include <opencv/cv.h>
	
using namespace std;
	
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
	void sort(CvMat &mat, size_t size, int index[]) {
		for(int i=0; i<size; i++)
			index[i]=i;
		for(int i=0; i<size-1; i++)
			for(int j=0; j<size-1-i; j++)
				if(CV_MAT_ELEM(mat, float, 0, j)<CV_MAT_ELEM(mat, float, 0, j+1)) {
					interchange(CV_MAT_ELEM(mat, float, 0, j), CV_MAT_ELEM(mat, float, 0, j+1));
					interchange(index[j], index[j+1]);
				}
	}
}

#endif