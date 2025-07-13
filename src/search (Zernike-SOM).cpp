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
#include "asm.h"
#include "std.h"
#include "som_loader.h"
#include "precision_recall.h"

using namespace std;
using namespace image_processing;

int main(int argc, char* argv[]) {
	mkdir("DATA/OUTPUT/");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM/TEXT_COMP");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM/TEXT_FEATURE");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM/QUERY_COMP");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM/QUERY_FEATURE");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM/OUTPUT");
	mkdir("DATA/OUTPUT/ZERNIKE-SOM/MATCH");
		
	//Creating cluster
	cout<<"Training SOM..."<<endl;
	system("som_pak\\randinit -din DATA\\zernike_feature.txt -cout DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt -xdim 4 -ydim 4 -topol rect -neigh gaussian -rand 123");
	system("som_pak\\vsom -din DATA\\zernike_feature.txt -cin DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt -cout DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt -rlen 1000 -alpha 0.05 -radius 10");
	system("som_pak\\vsom -din DATA\\zernike_feature.txt -cin DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt -cout DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt -rlen 10000 -alpha 0.02 -radius 3");
	system("som_pak\\qerror -din DATA\\zernike_feature.txt -cin DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt");
	system("som_pak\\vcal -din DATA\\zernike_feature.txt -cin DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt -cout DATA\\OUTPUT\\ZERNIKE-SOM\\map.txt");
	SOM sample("DATA/OUTPUT/ZERNIKE-SOM/map.txt");
	sample.visualize("DATA/OUTPUT/ZERNIKE-SOM/som.jpg");
	
	vector<string> text_files, query_files;
	int textLB=0, textUB=0x7FFFFFFF;
	if(argc>=2)
		textLB=atoi(argv[1]);
	if(argc>=3)
		textUB=atoi(argv[2]);
	DIR* text_dir=opendir("DATA/TEXTLINE");
	for(dirent *text_dirent; (text_dirent=readdir(text_dir))!=NULL; ) {
		static int i=0;
		string fname=text_dirent->d_name;
		if(getExtension(fname)=="jpg") {
			if(i>=textLB && i<=textUB)
				text_files.push_back(removeExtension(fname));
			i++;
		}
	}
	closedir(text_dir);
	cout<<"text   ";
	DIR* query_dir=opendir("DATA/QUERY");
	for(dirent *query_dirent; (query_dirent=readdir(query_dir))!=NULL; ) {
		string fname=query_dirent->d_name;
		if(getExtension(fname)=="jpg") {
			query_files.push_back(removeExtension(fname));
			cout<<setw(2)<<removeExtension(fname)<<" ";
		}
	}
	cout<<endl;
	
	ListComp query_comps[query_files.size()];
	int text_num=0;
	for(vector<string>::iterator tfi=text_files.begin(); tfi!=text_files.end(); tfi++, text_num++) {
		string text_filename=*tfi;
		cout<<setw(6)<<left<<text_filename<<" ";
		
		IplImage* text_img=cvLoadImage(("DATA/TEXTLINE/"+text_filename+".jpg").data(), CV_LOAD_IMAGE_GRAYSCALE);
		if(text_img==NULL) {
			cerr<<"!"<<endl;
			continue;
		}
		
		//Extracting components of text
		ListComp text_comp_list=getComponents(text_img);
		MComp* text_comp=ListCompToArray(text_comp_list);
		mkdir(("DATA/OUTPUT/ZERNIKE-SOM/TEXT_COMP/"+text_filename).data());
		for(int i=0; i<text_comp_list.size(); i++) {
			char out_name[100];
			sprintf(out_name, "DATA/OUTPUT/ZERNIKE-SOM/TEXT_COMP/%s/%.5d.jpg", text_filename.data(), i);
			saveComp(out_name, text_img, text_comp[i]);
		}
	
		//Extracting Zernike feature of text
		system(("MATLAB -r \"zernike DATA\\OUTPUT\\ZERNIKE-SOM\\TEXT_COMP\\"+text_filename+"\\*.jpg DATA\\OUTPUT\\ZERNIKE-SOM\\TEXT_FEATURE\\"+text_filename+".txt\" -wait  -nojvm -sd .").data());
	
		//Creating Text Sequence
		ImageFeature text(("DATA/OUTPUT/ZERNIKE-SOM/TEXT_FEATURE/"+text_filename+".txt").data());
		sample.createSequence(text);
		
		int query_num=0;
		for(vector<string>::iterator qfi=query_files.begin(); qfi!=query_files.end(); qfi++, query_num++) {
			string query_filename=*qfi;
			
			IplImage* query_img=cvLoadImage(("DATA/QUERY/"+query_filename+".jpg").data(), CV_LOAD_IMAGE_GRAYSCALE);
			if(query_img==NULL) {
				cerr<<"!  "<<endl;
				continue;
			}
			
			//Extracting components of query
			if(text_num==0)
				query_comps[query_num]=getComponents(query_img);
			MComp* query_comp=ListCompToArray(query_comps[query_num]);
			if(text_num==0) {
				mkdir(("DATA/OUTPUT/ZERNIKE-SOM/QUERY_COMP/"+query_filename).data());
				for(int i=0; i<query_comps[query_num].size(); i++) {
					char out_name[100];
					sprintf(out_name, "DATA/OUTPUT/ZERNIKE-SOM/QUERY_COMP/%s/%.5d.jpg", query_filename.data(), i);
					saveComp(out_name, query_img, query_comp[i]);
				}
			}
			
			//Extracting Zernike feature of query
			if(text_num==0)
				system(("MATLAB -r \"zernike DATA\\OUTPUT\\ZERNIKE-SOM\\QUERY_COMP\\"+query_filename+"\\*.jpg DATA\\OUTPUT\\ZERNIKE-SOM\\QUERY_FEATURE\\"+query_filename+".txt\" -wait -nojvm -sd .").data());
			
			//Creating Query Sequence
			ImageFeature query(("DATA/OUTPUT/ZERNIKE-SOM/QUERY_FEATURE/"+query_filename+".txt").data());
			sample.createSequence(query);
	
			//Searching Query in Text
			double match_th=query.size()*1.0;
			MatchList asm_match=sample.search(text, query, match_th);
			int nmatch=0;
			for(MatchList::iterator i=asm_match.begin(); i!=asm_match.end(); i++)
				nmatch++;
			cout<<setw(2)<<asm_match.size()<<" ";
	
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
				
				ifstream text_info_file(("DATA/TEXTLINE/"+text_filename+".txt").data());
				if(text_info_file==NULL)
					cout<<"Text info file not found"<<endl;
				else {
					int xgap, ygap;
					string original_text_filename;
					text_info_file>>original_text_filename>>xgap>>ygap;
					ofstream output(("DATA/OUTPUT/ZERNIKE-SOM/MATCH/"+removeExtension(original_text_filename)+".txt").data(), ios::app);
					output<<query_filename<<" "<<xgap+x1<<" "<<ygap+y1<<" "<<xgap+x2<<" "<<ygap+y2<<" "<<i->edit_distance/query.size()<<endl;
					output.close();
					text_info_file.close();
				}
			}
			if(text_num==0)
				mkdir(("DATA/OUTPUT/ZERNIKE-SOM/OUTPUT/"+query_filename).data());
			cvSaveImage(("DATA/OUTPUT/ZERNIKE-SOM/OUTPUT/"+query_filename+"/"+text_filename+".jpg").data(), output_img);
			cvReleaseImage(&output_img);
			
			delete query_comp;
			cvReleaseImage(&query_img);
		}
		delete text_comp;
		cvReleaseImage(&text_img);
		cout<<endl;
	}
}
