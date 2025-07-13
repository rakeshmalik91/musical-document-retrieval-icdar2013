#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cmath>
#include <vector>
#include <dirent.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>
#include "component.h"
#include "image_processing.h"
#include "std.h"

using namespace std;
using namespace image_processing;

int main(int argc, char* argv[]) {
	vector<string> text_files, query_files;
	if(argc<=1 || (string)argv[1]=="-") {
		DIR* text_dir=opendir("DATA/TEXT");
		for(dirent *text_dirent; (text_dirent=readdir(text_dir))!=NULL; ) {
			string fname=text_dirent->d_name;
			if(fname=="." || fname=="..")
				continue;
			text_files.push_back(removeExtension(fname));
		}
		closedir(text_dir);
	} else {
		text_files.push_back(argv[1]);
	}
	if(argc<=2 || (string)argv[2]=="-") {
		DIR* query_dir=opendir("DATA/QUERY");
		for(dirent *query_dirent; (query_dirent=readdir(query_dir))!=NULL; ) {
			string fname=query_dirent->d_name;
			if(fname=="." || fname=="..")
				continue;
			query_files.push_back(removeExtension(fname));
		}
		closedir(query_dir);
	} else {
		query_files.push_back(argv[2]);
	}
	
	mkdir("DATA/OUTPUT/400dim-KMeans");
	mkdir("DATA/TEXT_COMPONENT/");
	mkdir("DATA/QUERY_COMPONENT/");
	ofstream log("DATA/OUTPUT/400dim-KMeans/output.log");
		
	//Creating cluster
	cout<<"Creating kmeans cluster..."<<endl;
	KMeansTrainingSample sample("DATA/400dim_feature.txt", 20);
	
	int relevent_documents=0, retrieved_documents=0, retrieved_relevant_documents=0;
	ListComp query_comps[query_files.size()];
	int text_num=0;
	for(vector<string>::iterator tfi=text_files.begin(); tfi!=text_files.end(); tfi++, text_num++) {
		string text_filename=*tfi;
		cout<<setw(3)<<left<<text_num+1<<" Processing "<<text_filename<<" ..."<<endl;
		log<<setw(3)<<left<<text_num+1<<" Processing "<<text_filename<<" ..."<<endl;
		
		IplImage* text_img=cvLoadImage(("DATA/TEXT/"+text_filename+".jpg").data(), CV_LOAD_IMAGE_GRAYSCALE);
		if(text_img==NULL) {
			cerr<<"    File read error or not an image"<<endl;
			continue;
		}
		
		//Extracting components of text
		ListComp text_comp_list=getComponents(text_img);
		MComp* text_comp=ListCompToArray(text_comp_list);
		mkdir(("DATA/TEXT_COMPONENT/"+text_filename).data());
		for(int i=0; i<text_comp_list.size(); i++) {
			char out_name[100];
			sprintf(out_name, "DATA/TEXT_COMPONENT/%s/%.5d.jpg", text_filename.data(), i);
			saveComp(out_name, text_img, text_comp[i]);
		}
	
		//Extracting feature of text
		system(("feature_400dim DATA\\TEXT_COMPONENT\\"+text_filename+" DATA\\TEXT_COMPONENT\\"+text_filename+".txt").data());
	
		//Creating Text Sequence
		ImageFeature text(("DATA/TEXT_COMPONENT/"+text_filename+".txt").data());
		sample.createSequence(text);
		cout<<"   Cluster sequence : ";
		for(int i=0; i<text.seq.size(); i++)
			cout<<" "<<text.seq[i];
		cout<<endl;
		log<<"   ";
		for(int i=0; i<text.seq.size(); i++)
			log<<" "<<text.seq[i];
		log<<endl;
		
		int query_num=0;
		for(vector<string>::iterator qfi=query_files.begin(); qfi!=query_files.end(); qfi++, query_num++) {
			string query_filename=*qfi;
			cout<<"    "<<setw(3)<<left<<query_num+1<<" Searching "<<query_filename<<" ..."<<endl;
			log<<"    "<<setw(3)<<left<<query_num+1<<" Searching "<<query_filename<<" ..."<<endl;
			
			IplImage* query_img=cvLoadImage(("DATA/QUERY/"+query_filename+".jpg").data(), CV_LOAD_IMAGE_GRAYSCALE);
			if(query_img==NULL) {
				cerr<<"        File read error or not an image"<<endl;
				continue;
			}
			
			//Extracting components of query
			if(text_num==0)
				query_comps[query_num]=getComponents(query_img);
			MComp* query_comp=ListCompToArray(query_comps[query_num]);
			if(text_num==0) {
				mkdir(("DATA/QUERY_COMPONENT/"+query_filename).data());
				for(int i=0; i<query_comps[query_num].size(); i++) {
					char out_name[100];
					sprintf(out_name, "DATA/QUERY_COMPONENT/%s/%.5d.jpg", query_filename.data(), i);
					saveComp(out_name, query_img, query_comp[i]);
				}
			}
			
			//Extracting feature of query
			if(text_num==0)
				system(("feature_400dim DATA\\QUERY_COMPONENT\\"+query_filename+" DATA\\QUERY_COMPONENT\\"+query_filename+".txt").data());
			
			//Creating Query Sequence
			ImageFeature query(("DATA/QUERY_COMPONENT/"+query_filename+".txt").data());
			sample.createSequence(query);
			cout<<"        Cluster sequence : ";
			for(int i=0; i<query.seq.size(); i++)
				cout<<query.seq[i]<<" ";
			cout<<endl;
			log<<"        ";
			for(int i=0; i<query.seq.size(); i++)
				log<<query.seq[i]<<" ";
			log<<endl;
	
			//Searching Query in Text
			double match_th=query.size()*0.11;
			MatchList asm_match=sample.search(text, query, match_th);
			for(MatchList::iterator i=asm_match.begin(); i!=asm_match.end(); i++) {
				cout<<"        match from "<<i->start<<" to "<<i->end<<" at edit distance "<<i->edit_distance<<endl;
				log<<"        match from "<<i->start<<" to "<<i->end<<" at edit distance "<<i->edit_distance<<endl;
			}	
	
			//Storing Text output to File
			IplImage* output_img=cvCreateImage(cvGetSize(text_img), IPL_DEPTH_8U, 3);
			cvCvtColor(text_img, output_img, CV_GRAY2BGR);
			for(MatchList::iterator i=asm_match.begin(); i!=asm_match.end(); i++) {
				int x1=text_comp[i->start].B_Box.TopLftV.x;
				int y1=(text_comp[i->start].B_Box.TopLftV.y < text_comp[i->end].B_Box.TopLftV.y) ? text_comp[i->start].B_Box.TopLftV.y : text_comp[i->end].B_Box.TopLftV.y;
				int x2=text_comp[i->end].B_Box.BotRgtV.x;
				int y2=(text_comp[i->start].B_Box.BotRgtV.y > text_comp[i->end].B_Box.BotRgtV.y) ? text_comp[i->start].B_Box.BotRgtV.y : text_comp[i->end].B_Box.BotRgtV.y;
				CvScalar color=(i->edit_distance<match_th/3)?cvScalar(0, 255, 0):(i->edit_distance<2*match_th/3)?cvScalar(0, 255, 255):cvScalar(0, 0, 255);
				cvRectangle(output_img, cvPoint(x1, y1), cvPoint(x2, y2), color, 3);
			}
			CvFont font;
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA);
			for(int i=0; i<text.seq.size(); i++) {
				int x=text_comp[i].B_Box.TopLftV.x;
				int y=text_comp[i].B_Box.BotRgtV.y;
				char str[5];
				sprintf(str, "%d", text.seq[i]);
				cvPutText(output_img, str, cvPoint(x, y), &font, cvScalar(127, 127, 127));
			}
			if(text_num==0)
				mkdir(("DATA/OUTPUT/400dim-KMeans/"+query_filename).data());
			cvSaveImage(("DATA/OUTPUT/400dim-KMeans/"+query_filename+"/"+text_filename+".jpg").data(), output_img);
			cvReleaseImage(&output_img);
			
			//Storing Query Output to File
			if(text_num==0) {
				for(int i=0; i<query.seq.size(); i++) {
					int x=query_comp[i].B_Box.TopLftV.x;
					int y=query_comp[i].B_Box.BotRgtV.y;
					char str[5];
					sprintf(str, "%d", query.seq[i]);
					cvPutText(query_img, str, cvPoint(x, y), &font, cvScalar(127));
				}
				cvSaveImage(("DATA/OUTPUT/400dim-KMeans/"+query_filename+"/"+query_filename+".jpg").data(), query_img);
			}
			
			// Calculating relevent_documents, retrieved_documents, retrieved_relevant_documents
			int occur=numberOfOccuranceFromFirst(text_filename, query_filename[0], '_');
			if(occur>0) {
				relevent_documents+=occur;
				retrieved_relevant_documents+=(asm_match.size()<occur?asm_match.size():occur);
			}
			retrieved_documents+=asm_match.size();
			
			delete query_comp;
			cvReleaseImage(&query_img);
		}
		delete text_comp;
		cvReleaseImage(&text_img);
		
		// Calculating precision and recall
		double precision=(double)retrieved_relevant_documents/(double)retrieved_documents;
		double recall=(double)retrieved_relevant_documents/(double)relevent_documents;
		cout<<"    precision="<<precision<<", recall="<<recall<<endl;
		log<<"    precision="<<precision<<", recall="<<recall<<endl;
	}
	log.close();
	//cout<<endl<<"Press any key..."<<endl;
	//getchar();
}
