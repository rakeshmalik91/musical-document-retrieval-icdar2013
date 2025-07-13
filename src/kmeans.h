#ifndef _KMEANS_H
#define _KMEANS_H
	
#include "image_processing.h"
#include "asm.h"

using namespace std;
using namespace image_processing;
	
namespace image_processing {
	class KMeansTrainingSample : public ImageFeature {
	protected:
		CvMat* cluster;
		vector<FeatureVector> clusterCentre;
		int clusterCount;
		double maxCentreDistance;
	private:
		void calculateClusterCentre();
		void calculateMaxCentreDistance();
		double** calculateCostMatrix(ImageFeature&, ImageFeature&);
	public:
		void createCluster(int);
		KMeansTrainingSample(string filename, int);
		int getNearestCluster(FeatureVector) const ;
		FeatureVector getNearestMeanClusterCentre(FeatureVector, int) const;
		Sequence createSequence(ImageFeature&) const;
		MatchList search(ImageFeature&, ImageFeature&, double);
		~KMeansTrainingSample();
	};
	KMeansTrainingSample::KMeansTrainingSample(string filename, int clusterCount) : ImageFeature(filename) {
		createCluster(clusterCount);
	}
	void KMeansTrainingSample::createCluster(int clusterCount) {
		this->clusterCount=clusterCount;
		CvMat* samplemat=cvCreateMat(sample.size(), dim, CV_32FC1);
		for(int i=0; i<sample.size(); i++)
			for(int j=0; j<dim; j++)
				CV_MAT_ELEM(*samplemat, float, i, j)=sample[i][j];
		cluster=cvCreateMat(sample.size(), 1, CV_32SC1);
		cvKMeans2(samplemat, clusterCount, cluster, cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0));
		cvReleaseMat(&samplemat);
		
		calculateClusterCentre();
		calculateMaxCentreDistance();
	}
	void KMeansTrainingSample::calculateClusterCentre() {
		clusterCentre.resize(clusterCount);
		int cluster_size[clusterCount];
		for(int i=0; i<clusterCount; i++) {
			clusterCentre[i]=new float[dim];
			for(int j=0; j<dim; j++)
				clusterCentre[i][j]=0;
			cluster_size[i]=0;
		}
		for(int i=0; i<sample.size(); i++) {
			for(int j=0; j<dim; j++)
				clusterCentre[CV_MAT_ELEM(*cluster, int, i, 0)][j]+=sample[i][j];
			cluster_size[CV_MAT_ELEM(*cluster, int, i, 0)]++;
		}
		for(int i=0; i<clusterCount; i++)
			if(cluster_size[i]>0)
				for(int j=0; j<dim; j++)
					clusterCentre[i][j]/=cluster_size[i];
	}
	void KMeansTrainingSample::calculateMaxCentreDistance() {
		maxCentreDistance=-INFINITY;
		for(int i=0; i<clusterCount; i++)
			for(int j=0; j<clusterCount; j++) {
				double dist=euclideanDistance(clusterCentre[i], clusterCentre[j], dim);
				if(dist>maxCentreDistance)
					maxCentreDistance=dist;
			}
	}
	int KMeansTrainingSample::getNearestCluster(FeatureVector v) const {
		double mindist=INFINITY;
		int c;
		for(int j=0; j<clusterCount; j++) {
			double dist=euclideanDistance(clusterCentre[j], v, dim);
			if(dist<mindist) {
				mindist=dist;
				c=j;
			}
		}
		return c;
	}
	FeatureVector KMeansTrainingSample::getNearestMeanClusterCentre(FeatureVector v, int n) const {
		double dist[clusterCount];
		int num[clusterCount];
		for(int i=0; i<clusterCount; i++) {
			dist[i]=euclideanDistance(clusterCentre[i], v, dim);
			num[i]=i;
		}
		for(int i=0; i<n; i++) {
			for(int j=0; j<clusterCount-i-1; j++) {
				if(dist[j]<dist[j+1]) {
					interchange(dist[j], dist[j+1]);
					interchange(num[j], num[j+1]);
				}
			}
		}
		FeatureVector mean=new float[dim];
		int total_weight=0;
		for(int i=0; i<dim; i++)
			mean[i]=0;
		for(int i=clusterCount-n, weight=1; i<clusterCount; i++, weight+=2) {
			total_weight+=weight;
			for(int j=0; j<dim; j++)
				mean[j]+=clusterCentre[num[i]][j]*weight;
		}
		for(int i=0; i<dim; i++)
			mean[i]/=total_weight;
		return mean;
	}
	Sequence KMeansTrainingSample::createSequence(ImageFeature &f) const {
		Sequence seq(f.size());
		for(int i=0; i<f.size(); i++)
			seq[i]=getNearestCluster(f[i]);
		f.seq=seq;
		return seq;
	}
	double** KMeansTrainingSample::calculateCostMatrix(ImageFeature &text, ImageFeature &query) {
		FeatureVector tc[text.size()], qc[query.size()];
		for(int i=0; i<text.size(); i++)
			tc[i]=getNearestMeanClusterCentre(text[i], 3);
		for(int i=0; i<query.size(); i++)
			qc[i]=getNearestMeanClusterCentre(query[i], 3);
		double **cost=new double*[query.size()];
		for(int i=0; i<query.size(); i++) {
			cost[i]=new double[text.size()];
			for(int j=0; j<text.size(); j++)
				cost[i][j]=euclideanDistance(tc[j], qc[i], dim)/maxCentreDistance;
		}
		for(int i=0; i<text.size(); i++)
			delete tc[i];
		for(int i=0; i<query.size(); i++)
			delete qc[i];
		return cost;
	}
	MatchList KMeansTrainingSample::search(ImageFeature &text, ImageFeature &query, double match_th=0) {
		double **cost=calculateCostMatrix(text, query);
		MatchList match=approximateStringMatch(cost, text.size(), query.size(), match_th, false);
		delete2D(cost, query.size());
		return match;
	}
	KMeansTrainingSample::~KMeansTrainingSample() {
		for(int i=0; i<clusterCentre.size(); i++)
			delete clusterCentre[i];
		if(cluster!=NULL)
			cvReleaseMat(&cluster);
	}
}

#endif