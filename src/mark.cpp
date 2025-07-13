#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include "image_processing.h"

using namespace std;
using namespace image_processing;

int main(int argc, char* argv[]){
	string textDirName = "DATA/TEXT";
	int nOutputDir = 5;
	string outputDirName[] = {
		"DATA/OUTPUT/ZERNIKE-KMEANS/MATCH",
		"DATA/OUTPUT/ZERNIKE-SOM/MATCH",
		"DATA/OUTPUT/400DIM-KMEANS/MATCH",
		"DATA/OUTPUT/400DIM-SOM/MATCH",
		"DATA/OUTPUT/DTW/MATCH"
	};
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
	for(int iOutputDir = 0; iOutputDir < nOutputDir; iOutputDir++) {
		cout<<"Processing files in "<<outputDirName[iOutputDir]<<endl;
		DIR* outputDir = opendir(outputDirName[iOutputDir].data());
		if(outputDir == NULL) {
			cout<<"Output Directory "<<outputDirName[iOutputDir]<<" Not Available"<<endl;
			continue;
		}
		for(dirent *outputFileDirent; (outputFileDirent = readdir(outputDir)) != NULL; ) {
			if(getExtension(outputFileDirent->d_name) != "txt")
				continue;
			string outputFileName = outputDirName[iOutputDir] + "/" + outputFileDirent->d_name;
			cout<<"Processing "<<outputFileName<<endl;
			string imageName = removeExtension(outputFileDirent->d_name) + ".png";
			string textImageName = textDirName + "/" + imageName;
			string outputImageName = outputDirName[iOutputDir] + "/" + imageName;
			IplImage* img = cvLoadImage(textImageName.data());
			if(img == NULL) {
				cout<<textImageName<<" Image read error"<<endl;
				exit(0);
			}
			negative(img);
			ifstream outputFile(outputFileName.data());
			while(!outputFile.eof()) {
				string queryName;
				int x1, y1, x2, y2;
				double editDistance;
				outputFile>>queryName>>x1>>y1>>x2>>y2>>editDistance;
				if(editDistance<0.1 && queryName=="05") {
					cvRectangle(img, cvPoint(x1, y1), cvPoint(x2, y2), cvScalar(0, 255*(editDistance*10), 255*(1-editDistance*10)), 3);
					cvPutText(img, queryName.data(), cvPoint(x1, y2), &font, cvScalar(0, 0, 255));
				}
				for(char c = outputFile.get(); !isgraph(c) && c != EOF; c = outputFile.get());
				if(!outputFile.eof())
					outputFile.unget();
			}
			outputFile.close();
			cvSaveImage(outputImageName.data(), img);
			cvReleaseImage(&img);
		}
		closedir(outputDir);
	}
}
