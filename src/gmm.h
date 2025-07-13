#ifndef _GMM_H
#define _GMM_H
	
#include "image_processing.h"
#include "asm.h"

using namespace std;
using namespace cv;
using namespace image_processing;
	
namespace image_processing {
	class GMMTrainingSample : public ImageFeature {
	protected:
		int clusterCount;
		CvEM em_model;
		CvEMParams em_params;
		long* label;
	private:
		double** calculateCostMatrix(ImageFeature&, ImageFeature&);
	public:
		GMMTrainingSample(string, int);
		int getNearestCluster(FeatureVector v) const;
		Sequence createSequence(ImageFeature&) const;
		MatchList search(ImageFeature&, ImageFeature&, double);
		int error();
		~GMMTrainingSample();
	};
	GMMTrainingSample::GMMTrainingSample(string filename, int n) : ImageFeature(filename)/*, clusterCount(n), label(new long[size()])*/ {
		cout<<dim<<endl;
		em_params.covs=NULL;
		em_params.means=NULL;
		em_params.weights=NULL;
		em_params.probs=NULL;
		em_params.nclusters=clusterCount;
		em_params.cov_mat_type=CvEM::COV_MAT_GENERIC;
		em_params.start_step=CvEM::START_AUTO_STEP;
		em_params.term_crit.max_iter=10;
		em_params.term_crit.epsilon=0.1;
		em_params.term_crit.type=CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;
		
		/*Mat samplemat(size(), dim, CV_32FC1), labelmat;
		for(int i=0; i<size(); i++)
			for(int j=0; j<dim; j++)
				samplemat.at<float>(i, j)=sample[i][j];
		em_model.train(samplemat, Mat(), em_params, &labelmat);
		for(int i=0; i<size(); i++)
			label[i]=labelmat.at<long>(i, 0);
		*/
		//error();
	}
	int GMMTrainingSample::error() {
		int e=0;
		for(int i=0; i<sample.size(); i++) {
			Mat test(1, dim, CV_32FC1, sample[i]), probs;
			if(cvRound(em_model.predict(test, &probs))!=label[i])
				e++;
			int index[clusterCount];
			sortBubbleDesc(probs, index);
			cout<<label[i]<<" ";
			double sum=0;
			for(int j=0; j<clusterCount; j++) {
				sum+=probs.at<float>(j, 0);
				if(probs.at<float>(j, 0)>0)
					cout<<index[j]<<":"<<probs.at<float>(j, 0)<<" ";
			}
			cout<<"("<<sum<<")";
			getchar();
		}
		return e;
	}
	int GMMTrainingSample::getNearestCluster(FeatureVector v) const {
		Mat samplemat(1, dim, CV_32FC1, v);
		return cvRound(em_model.predict(samplemat, NULL));
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
				Mat textmat(1, dim, CV_32FC1, text[t]), textprobs;
				int textcluster=cvRound(em_model.predict(textmat, &textprobs));
				int textprobindex[clusterCount];
				sortBubbleDesc(textprobs, textprobindex, 3);
				
				Mat querymat(1, dim, CV_32FC1, query[q]), queryprobs;
				int querycluster=cvRound(em_model.predict(querymat, &queryprobs));
				int queryprobindex[clusterCount];
				sortBubbleDesc(queryprobs, queryprobindex, 3);
				
				cost[q][t]=(querycluster!=textcluster);
				for(int i=0; i<3 && cost[q][t]; i++)
					if(textprobs.at<float>(i, 0)>0)
						for(int j=0; j<3 && cost[q][t]; j++)
							if(queryprobs.at<float>(j, 0)>0)
								cost[q][t]=(queryprobindex[j]!=textprobindex[i]);
			}
		}
		return cost;
	}
	MatchList GMMTrainingSample::search(ImageFeature &text, ImageFeature &query, double match_th=0) {
		double **cost=calculateCostMatrix(text, query);
		MatchList match=approximateStringMatch(text.seq, query.seq, match_th, cost, false);
		delete2D(cost, query.size());
		return match;
	}
	GMMTrainingSample::~GMMTrainingSample() {
		delete label;
	}
}

#endif