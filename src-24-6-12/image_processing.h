#ifndef _IMAGE_PROCESSING_H
#define _IMAGE_PROCESSING_H
	
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cctype>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>
#include <dirent.h>
#include "std.h"
	
using namespace std;
	
namespace image_processing {
	typedef float* FeatureVector;
	
	double euclideanDistance(FeatureVector v1, FeatureVector v2, int n) {
		double sqdist=0;
		for(int i=0; i<n; i++)
			sqdist+=(v1[i]-v2[i])*(v1[i]-v2[i]);
		return sqrt(sqdist);
	}
	double dTWDistance(FeatureVector v1, FeatureVector v2, int n1, int n2) {
		float DTW[n1+1][n2+1];
		for(int i=0; i<=n1; i++)
			DTW[i][0]=INFINITY;
		for(int i=0; i<=n2; i++)
			DTW[0][i]=INFINITY;
		DTW[0][0]=0;
		for(int i=1; i<=n1; i++)
			for(int j=1; j<=n2; j++)
				DTW[i][j]=fabs(v1[i-1]-v2[j-1])+min(DTW[i-1][j], DTW[i][j-1], DTW[i-1][j-1]);
		return DTW[n1][n2];
	}
	
	typedef vector<int> Sequence;
	struct Match {
		int start, end;
		double edit_distance;
		Match(int, int, double);
		bool overlaps(Match);
	};
	Match::Match(int start, int end, double edit_distance) {
		this->start=start;
		this->end=end;
		this->edit_distance=edit_distance;
	}
	bool Match::overlaps(Match m) {
		return (m.start>=start && m.start<=end || m.end>=start && m.end<=end);
	}
	typedef list<Match> MatchList;
	MatchList approximateStringMatch(Sequence text, Sequence pattern, double n=0, double** cost=NULL, bool overlapping=true) {
		MatchList match;
		double d[pattern.size()+1][text.size()+1];
		for(int t=0; t<=text.size(); t++)
			d[0][t]=0;
		for(int p=1; p<=pattern.size(); p++)
			d[p][0]=p;
		for(int p=1; p<=pattern.size(); p++) {
			for(int t=1; t<=text.size(); t++) {
				double c=cost ? cost[p-1][t-1] : (text[t-1]==pattern[p-1] ? 0 : 1);
				d[p][t]=min(d[p-1][t-1]+c, d[p][t-1]+1.0, d[p-1][t]+1.0);
			}
		}
		for(int t=1; t<=text.size(); t++)
			if(d[pattern.size()][t]<=n)
				match.push_back(Match(t-1, t-1, d[pattern.size()][t]));
		for(MatchList::iterator i=match.begin(); i!=match.end(); i++) {
			for(int t=i->end+1; t>=0; t--)
				d[pattern.size()][t]=0;
			for(int p=pattern.size()-1; p>=0; p--)
				d[p][i->end+1]=pattern.size()-p;
			for(int p=pattern.size()-1; p>=0; p--) {
				for(int t=i->end; t>=0; t--) {
					double c=cost ? cost[p][t] : (text[t]==pattern[p] ? 0 : 1);
					d[p][t]=min(d[p+1][t+1]+c, d[p][t+1]+1.0, d[p+1][t]+1.0);
				}
			}
			for(int t=i->end+1; t>=0; t--)
				if(round(d[0][t]*1e6)==round(i->edit_distance*1e6)) {
					i->start=t;
					break;
				}
		}
		if(!overlapping) {
			MatchList::iterator i=match.begin();
			Match last=*i;
			for(i++; i!=match.end(); ) {
				if(last.overlaps(*i)) {
					if(last.edit_distance<i->edit_distance)
						i=match.erase(i);
					else {
						i--;
						i=match.erase(i);
						last=*i;
						i++;
					}
				} else {
					last=*i;
					i++;
				}
			}
		}
		return match;
	}
	
	class ImageFeature {
	protected:
		int k;
		vector<FeatureVector> sample;
		vector<string> lbl;
	public:
		Sequence seq;
		ImageFeature(int);
		ImageFeature(string);
		void add(FeatureVector, string);
		int size() const;
		int dimension() const;
		FeatureVector& operator[](int);
		string& label(int);
		~ImageFeature();
	};
	ImageFeature::ImageFeature(int k) {
		this->k=k;
	}
	void ImageFeature::add(FeatureVector v, string l="") {
		sample.push_back(v);
		lbl.push_back(l);
	}
	ImageFeature::ImageFeature(string filename) {
		ifstream in(filename.data());
		if(in==NULL) {
			cerr<<"File "<<filename<<" read error"<<endl;
			exit(0);
		}
		in>>k;
		while(!in.eof()) {
			FeatureVector v=new float[k];
			for(int i=0; i<k; i++)
				in>>v[i];
			sample.push_back(v);
			string l;
			in>>l;
			lbl.push_back(l);
			for(char c=in.get(); !isgraph(c) && c!=EOF; c=in.get());
			if(!in.eof())
				in.unget();
		}
		in.close();
	}
	int ImageFeature::size() const {
		return sample.size();
	}
	int ImageFeature::dimension() const {
		return k;
	}
	FeatureVector& ImageFeature::operator[](int i) {
		return sample[i];
	}
	string& ImageFeature::label(int i) {
		return lbl[i];
	}
	ImageFeature::~ImageFeature() {
		for(int i=0; i<sample.size(); i++)
			delete sample[i];
	}
	
	class KMeansTrainingSample : public ImageFeature{
	protected:
		CvMat* cluster;
		vector<FeatureVector> clusterCentre;
		int clusterCount;
		double maxCentreDistance;
	private:
		void calculateClusterCentre();
		void calculateMaxCentreDistance();
		double** calculateCostMatrix(ImageFeature&, ImageFeature&);
		void deleteCostMatrix(double**&, ImageFeature&);
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
		CvMat* samplemat=cvCreateMat(sample.size(), 1, CV_32FC(k));
		for(int i=0; i<sample.size(); i++) {
			float* elem=(float*)CV_MAT_ELEM_PTR(*samplemat, i, 0);
			memcpy(elem, sample[i], sizeof(float)*k);
		}
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
			clusterCentre[i]=new float[k];
			for(int j=0; j<k; j++)
				clusterCentre[i][j]=0;
			cluster_size[i]=0;
		}
		for(int i=0; i<sample.size(); i++) {
			for(int j=0; j<k; j++)
				clusterCentre[CV_MAT_ELEM(*cluster, int, i, 0)][j]+=sample[i][j];
			cluster_size[CV_MAT_ELEM(*cluster, int, i, 0)]++;
		}
		for(int i=0; i<clusterCount; i++)
			if(cluster_size[i]>0)
				for(int j=0; j<k; j++)
					clusterCentre[i][j]/=cluster_size[i];
	}
	void KMeansTrainingSample::calculateMaxCentreDistance() {
		maxCentreDistance=-INFINITY;
		for(int i=0; i<clusterCount; i++)
			for(int j=0; j<clusterCount; j++) {
				double dist=euclideanDistance(clusterCentre[i], clusterCentre[j], k);
				if(dist>maxCentreDistance)
					maxCentreDistance=dist;
			}
	}
	int KMeansTrainingSample::getNearestCluster(FeatureVector v) const {
		double mindist=INFINITY;
		int c;
		for(int j=0; j<clusterCount; j++) {
			double dist=euclideanDistance(clusterCentre[j], v, k);
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
			dist[i]=euclideanDistance(clusterCentre[i], v, k);
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
		FeatureVector mean=new float[k];
		int total_weight=0;
		for(int i=0; i<k; i++)
			mean[i]=0;
		for(int i=clusterCount-n, weight=1; i<clusterCount; i++, weight+=2) {
			total_weight+=weight;
			for(int j=0; j<k; j++)
				mean[j]+=clusterCentre[num[i]][j]*weight;
		}
		for(int i=0; i<k; i++)
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
				cost[i][j]=euclideanDistance(tc[j], qc[i], k)/maxCentreDistance;
		}
		for(int i=0; i<text.size(); i++)
			delete tc[i];
		for(int i=0; i<query.size(); i++)
			delete qc[i];
		return cost;
	}
	void KMeansTrainingSample::deleteCostMatrix(double** &cost, ImageFeature &query) {
		for(int i=0; i<query.size(); i++)
			delete cost[i];
		delete cost;
	}
	MatchList KMeansTrainingSample::search(ImageFeature &text, ImageFeature &query, double match_th=0) {
		double **cost=calculateCostMatrix(text, query);
		MatchList match=approximateStringMatch(text.seq, query.seq, match_th, cost, false);
		deleteCostMatrix(cost, query);
		return match;
	}
	KMeansTrainingSample::~KMeansTrainingSample() {
		for(int i=0; i<clusterCentre.size(); i++)
			delete clusterCentre[i];
		if(cluster!=NULL)
			cvReleaseMat(&cluster);
	}

	class GMMTrainingSample : public ImageFeature {
		int clusterCount;
		CvEM em_model;
		CvEMParams em_params;
		double** calculateCostMatrix(ImageFeature&, ImageFeature&);
		void deleteCostMatrix(double**&, ImageFeature&);
	public:
		GMMTrainingSample(string, int);
		int getNearestCluster(FeatureVector v) const;
		Sequence createSequence(ImageFeature&) const;
		MatchList search(ImageFeature&, ImageFeature&, double);
	};
	GMMTrainingSample::GMMTrainingSample(string filename, int clusterCount) : ImageFeature(filename) {
		this->clusterCount=clusterCount;
		em_params.covs=NULL;
		em_params.means=NULL;
		em_params.weights=NULL;
		em_params.probs=NULL;
		em_params.nclusters=clusterCount;
		em_params.cov_mat_type=CvEM::COV_MAT_SPHERICAL;
		em_params.start_step=CvEM::START_AUTO_STEP;
		em_params.term_crit.max_iter=10;
		em_params.term_crit.epsilon=0.1;
		em_params.term_crit.type=CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;
		
		CvMat* samplemat=cvCreateMat(sample.size(), k, CV_32FC1 );
		CvMat* labelmat=cvCreateMat(sample.size(), 1, CV_32SC1 );
		for(int i=0; i<sample.size(); i++)
			for(int j=0; j<k; j++)
				CV_MAT_ELEM(*samplemat, float, i, j)=sample[i][j];
		em_model.train(samplemat, 0, em_params, labelmat);
		cvReleaseMat(&samplemat);
		cvReleaseMat(&labelmat);
	}
	int GMMTrainingSample::getNearestCluster(FeatureVector v) const {
		CvMat samplemat=cvMat(1, k, CV_32FC1, v);
		return cvRound(em_model.predict(&samplemat, NULL));
	}
	Sequence GMMTrainingSample::createSequence(ImageFeature &f) const {
		Sequence seq(f.size());
		for(int i=0; i<f.size(); i++)
			seq[i]=getNearestCluster(f[i]);
		f.seq=seq;
		return seq;
	}
	double** GMMTrainingSample::calculateCostMatrix(ImageFeature &text, ImageFeature &query) {
		double **cost=new double*[query.size()];
		for(int q=0; q<query.size(); q++) {
			cost[q]=new double[text.size()];
			for(int t=0; t<text.size(); t++) {
				CvMat textmat=cvMat(1, k, CV_32FC1, text[t]);
				CvMat* textprobs=cvCreateMat(1, clusterCount, CV_32FC1);
				em_model.predict(&textmat, textprobs);
				int textprobindex[clusterCount];
				sort(*textprobs, clusterCount, textprobindex);
				
				CvMat querymat=cvMat(1, k, CV_32FC1, query[q]);
				CvMat* queryprobs=cvCreateMat(1, clusterCount, CV_32FC1);
				em_model.predict(&querymat, queryprobs);
				int queryprobindex[clusterCount];
				sort(*queryprobs, clusterCount, queryprobindex);
				
				cvReleaseMat(&textprobs);
				cvReleaseMat(&queryprobs);
			}
		}
		return cost;
	}
	void GMMTrainingSample::deleteCostMatrix(double** &cost, ImageFeature &query) {
		for(int i=0; i<query.size(); i++)
			delete cost[i];
		delete cost;
	}
	MatchList GMMTrainingSample::search(ImageFeature &text, ImageFeature &query, double match_th=0) {
		double **cost=calculateCostMatrix(text, query);
		MatchList match=approximateStringMatch(text.seq, query.seq, match_th, cost, true);
		deleteCostMatrix(cost, query);
		return match;
	}
}

#endif