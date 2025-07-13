#ifndef _PRECISION_RECALL_H
#define _PRECISION_RECALL_H
	
#include "image_processing.h"

using namespace std;
using namespace image_processing;

namespace image_processing {
	class PrecisionRecallData {
	public:
		double index, precision, recall;
		int retrieved, retrieved_relevant;
		PrecisionRecallData(double, int, int);
	};
	PrecisionRecallData::PrecisionRecallData(double index=0, int retrieved=0, int retrieved_relevant=0) {
		this->index=index;
		this->retrieved=retrieved;
		this->retrieved_relevant=retrieved_relevant;
	}
	class PrecisionRecallCurve {
	public:
		PrecisionRecallData *data;
		int ndata;
		int relevant;
		PrecisionRecallCurve(double, double, double);
		~PrecisionRecallCurve();
		IplImage* plot(ofstream&, IplImage*, int, CvScalar);
	};
	PrecisionRecallCurve::PrecisionRecallCurve(double min, double max, double step) {
		ndata=(max-min)/step;
		data=new PrecisionRecallData[ndata];
		for(int i=0; i<ndata; i++)
			data[i].index=min+i*step;
		relevant=0;
	}
	PrecisionRecallCurve::~PrecisionRecallCurve() {
		delete data;
	}
	IplImage* PrecisionRecallCurve::plot(ofstream &csvFile, IplImage* graph=NULL, int thickness=1, CvScalar color=cvScalar(0)) {
		int h=500, w=500, border=50;
		if(graph==NULL) {
			graph=cvLoadImage("DATA/graph_frame.jpg");
			if(graph==NULL) {
				cout<<"Graph frame not Found"<<endl;
				return NULL;
			}
		}
		for(int i=0; i<ndata; i++) {
			data[i].precision=(double)data[i].retrieved_relevant/(double)data[i].retrieved;
			data[i].recall=(double)data[i].retrieved_relevant/(double)relevant;
			if(relevant==0)
				data[i].recall=1;
			if(data[i].retrieved==0)
				data[i].precision=1;
			if(csvFile!=NULL)
				csvFile<<relevant<<","<<data[i].retrieved<<","<<data[i].retrieved_relevant<<","<<data[i].precision<<","<<data[i].recall<<endl;
		}
		for(int i=0; i<ndata-1; i++)
			cvLine(graph, cvPoint(border+(data[i].precision)*(w-2*border), border+(1-data[i].recall)*(h-2*border)), cvPoint(border+(data[i+1].precision)*(w-2*border), border+(1-data[i+1].recall)*(h-2*border)),  color, thickness);
		return graph;
	}
}

#endif