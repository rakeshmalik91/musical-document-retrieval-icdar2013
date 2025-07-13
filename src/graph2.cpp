#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include "precision_recall.h"

using namespace std;
using namespace image_processing;

struct MatchData {
	string queryName;
	int x1, y1, x2, y2;
	double editDistance;
	bool operator==(MatchData&);
};
bool MatchData::operator==(MatchData& data) {
	if(queryName != data.queryName)
		return false;
	int total=0, overlap=0;
	for(int x=x1; x<=x2; x++)
		for(int y=y1; y<=y2; y++) {
			total++;
			if(x>=data.x1 && x<=data.x2 && y>=data.y1 && y<=data.y2)
				overlap++;
		}
	for(int x=data.x1; x<=data.x2; x++)
		for(int y=data.y1; y<=data.y2; y++)
			total++;
	total-=overlap;
	double percentage=(double)overlap*100.0/(double)total;
	return percentage>50;
}
int numberOfDataInEditDistance(vector<MatchData> data, double th) {
	int n = 0;
	for(int i = 0; i < data.size() ; i++)
		if(data[i].editDistance < th)
			n++;
	return n;
}
int numberOfMatchingDataInEditDistance(vector<MatchData> outputData, vector<MatchData> groundTruthData, double th) {
	int n = 0;
	for(int i = 0; i < outputData.size() ; i++)
		if(outputData[i].editDistance < th)
			for(int j = 0; j < groundTruthData.size() ; j++)
				if(outputData[i] == groundTruthData[j]) {
					n++;
				}
	return n;
}

int main(int argc, char* argv[]){
	string groundTruthDirName = "DATA/GROUND_TRUTH";
	int nOutputDir = 3;
	string outputDirName[] = {
		"DATA/OUTPUT/ZERNIKE-KMEANS 10/MATCH",//BLUE
		"DATA/OUTPUT/ZERNIKE-KMEANS 15/MATCH",//GREEN
		"DATA/OUTPUT/ZERNIKE-KMEANS 20/MATCH",//RED
	};
	const char* csvFileName[] = {
		"DATA/OUTPUT/ZERNIKE-KMEANS 10.csv",
		"DATA/OUTPUT/ZERNIKE-KMEANS 15.csv",
		"DATA/OUTPUT/ZERNIKE-KMEANS 20.csv",
	};
	IplImage* graph=NULL;
	for(int iOutputDir = 0; iOutputDir < nOutputDir; iOutputDir++) {
		ofstream csvFile(csvFileName[iOutputDir]);
		csvFile<<"relevant,retrieved,retrieved & relevant,precision,recall"<<endl;
		DIR* outputDir = opendir(outputDirName[iOutputDir].data());
		if(outputDir == NULL) {
			cout<<"Output Directory "<<outputDirName[iOutputDir]<<" Not Available"<<endl;
			continue;
		}
		PrecisionRecallCurve curve(0.0, 0.2, 0.01);
		for(dirent *outputFileDirent; (outputFileDirent = readdir(outputDir)) != NULL; ) {
			if((string)outputFileDirent->d_name=="." || (string)outputFileDirent->d_name=="..")
				continue;
			string outputFileName = outputDirName[iOutputDir] + "/" + outputFileDirent->d_name;
			string groundTruthFileName = groundTruthDirName + "/" + removeExtension(outputFileDirent->d_name) + ".txt";
			ifstream outputFile(outputFileName.data()), groundTruthFile(groundTruthFileName.data());
			if(outputFile == NULL) {
				cout<<"Output File "<<outputFileName<<" Read Error"<<endl;
				continue;
			}
			if(groundTruthFile == NULL) {
				cout<<"Ground Truth File "<<groundTruthFileName<<" Not Available"<<endl;
				continue;
			}
			vector<MatchData> outputData, groundTruthData;
			while(!outputFile.eof()) {
				MatchData data;
				outputFile>>data.queryName>>data.x1>>data.y1>>data.x2>>data.y2>>data.editDistance;
				outputData.push_back(data);
				for(char c = outputFile.get(); !isgraph(c) && c != EOF; c = outputFile.get());
				if(!outputFile.eof())
					outputFile.unget();
			}
			outputFile.close();
			while(!groundTruthFile.eof()) {
				MatchData data;
				groundTruthFile>>data.queryName>>data.x1>>data.y1>>data.x2>>data.y2;
				data.editDistance = 0;
				groundTruthData.push_back(data);
				for(char c = groundTruthFile.get(); !isgraph(c) && c != EOF; c = groundTruthFile.get());
				if(!groundTruthFile.eof())
					groundTruthFile.unget();
			}
			groundTruthFile.close();
			
			curve.relevant += groundTruthData.size();
			for(int i=0; i<curve.ndata; i++) {
				curve.data[i].retrieved += numberOfDataInEditDistance(outputData, curve.data[i].index);
				curve.data[i].retrieved_relevant += numberOfMatchingDataInEditDistance(outputData, groundTruthData, curve.data[i].index);
			}
		}
		closedir(outputDir);
		CvScalar color=(iOutputDir==0)?cvScalar(255):(iOutputDir==1)?cvScalar(0, 255):cvScalar(0, 0, 255);
		graph=curve.plot(csvFile, graph, 2, color);
		csvFile.close();
	}
	cvSaveImage("DATA/OUTPUT/curve_2.jpg", graph);
	const char *windowName="PR-Curve";
	cvNamedWindow(windowName);
	cvShowImage(windowName, graph);
	cvWaitKey();
}
