#ifndef _COMPONENT_H
#define _COMPONENT_H
	
#include <list>
#include <climits>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cmath>
#include "image_processing.h"
#include "std.h"
	
using namespace std;
using namespace image_processing;

#define BLACK 0
#define WHITE 255

namespace image_processing {
	typedef struct {
		int x[5];
	}MInt;
	typedef list<MInt> ListInt;
	typedef struct MPix{
		CvPoint  V;
		int      VI;
		double	VD;
	}MPix;
	typedef list<MPix> ListPix;
	typedef struct MBoundingBox{
		CvPoint TopLftV;
		CvPoint BotRgtV;
		CvPoint MinRowV;
		CvPoint MinColV;
		CvPoint MaxRowV;
		CvPoint MaxColV;
		CvSize HW;
	}MBoundingBox;
	typedef struct MComp{
		int area;
		MBoundingBox B_Box;
		ListPix Pt;
		int flag;
		bool isSmallPartOf(MComp, int);
		bool boundingBoxOverlaps(MComp);
	}MComp;
	MComp ComponentProperty(ListPix LP);
	typedef list<MComp> ListComp;
	ListComp BinImg2Comp( IplImage *src){	//Extract component of white pixels
		bool flagL,flagLU,flagU,flagRU;
		int i;
		MInt numdata;	ListInt numlist;	numlist.clear();
		int **labelMAT =  (int**)malloc(src->height*sizeof(int*));
		for(i = 0; i < src->height; i++) labelMAT[i] = new int[src->width];
		int newlabel=1;
		unsigned char *data = reinterpret_cast<unsigned char *>(src->imageData);
		unsigned char *pdata;
		
		for( i=0; i < src->height;i++){
			for( int j=0;j< src->width ;j++){			
				flagL=flagLU=flagU=flagRU=1;
				if(i==0 ){	flagLU=0;	flagU=0;	flagRU=0;	}
				if(j==0){	flagL=0;	flagLU=0;	}
				if(j==src->width-1)		flagRU=0;
	
				if( data[j] == WHITE){
					if( flagL==1 && data[j-1]==WHITE)           	labelMAT[i][j]=labelMAT[i][j-1];			
					else if( flagLU==1 && pdata[j-1] ==WHITE)		labelMAT[i][j]=labelMAT[i-1][j-1];
					else if( flagU==1 && pdata[j]==WHITE)		    labelMAT[i][j]=labelMAT[i-1][j];
					else if( flagRU==1 && pdata[j+1]==WHITE)        labelMAT[i][j]=labelMAT[i-1][j+1];
					else{				
						labelMAT[i][j]=newlabel; 				newlabel=newlabel+1;						
					}
					
					if(flagL==1 && flagU==1 && data[j-1]== WHITE && pdata[j] == WHITE && labelMAT[i][j-1]!=labelMAT[i-1][j]  ){
						numdata.x[0] = labelMAT[i-1][j];	numdata.x[1] = labelMAT[i-1][j];
						numlist.insert( numlist.end(), numdata);
					}	
					else if(flagL==1 && flagRU==1 && data[j-1]==WHITE && pdata[j+1] == WHITE && labelMAT[i][j-1]!=labelMAT[i-1][j+1]){
						numdata.x[0] = labelMAT[i-1][j+1];		numdata.x[1] = labelMAT[i][j-1];
						numlist.insert( numlist.end(), numdata);
					}
					else if(flagLU==1 && flagRU==1 && pdata[j-1]==WHITE && pdata[j+1] == WHITE && labelMAT[i-1][j-1]!=labelMAT[i-1][j+1]){
						numdata.x[0] = labelMAT[i-1][j-1];		numdata.x[1] = labelMAT[i-1][j+1];
						numlist.insert( numlist.end(), numdata);			
					}
				}
				else labelMAT[i][j] = 0;
			}
			pdata = data;
			data += src->widthStep;
		}
	
		int *maptable= new int[newlabel];
		for( i=1;i<newlabel;i++)	maptable[i]=i;
	
		for( ListInt:: iterator LN = numlist.begin(); LN != (numlist).end(); ++LN){
			int val1= (*LN).x[0];	int val2=(*LN).x[1];
			while(val1!=maptable[val1])	val1=maptable[val1];
			while(val2!=maptable[val2])	val2=maptable[val2];
			if(val1>val2)	maptable[val1]=val2;
			else if(val1<val2)	maptable[val2]=val1;
		}
		numlist.clear();
	
		int *checklabel=new int[newlabel];
	
		int totalcomp = 0;
		for( i=1;i<newlabel;i++){
			if(maptable[i]==i){
				checklabel[i]= totalcomp;
				totalcomp++;
			}
			else	checklabel[i]=-1;
		}
	
		MComp *comp = new MComp[totalcomp];
	
		for( i=0;i<totalcomp;i++)	comp[i].area = 0;
	
		MPix Pix;
		for( i=0;i < src->height;i++){		
			for( int j=0;j < src->width;j++){
				int val = labelMAT[i][j];
				if( val != 0){
					while(val!=maptable[val])	val=maptable[val];
					int lab=checklabel[val];
					labelMAT[i][j]=lab;
					comp[lab].area++;
	
					Pix.V.x = j;	Pix.V.y = i;
					if(comp[lab].area==1)	comp[lab].Pt.insert( comp[lab].Pt.end(), Pix);
					else	comp[lab].Pt.insert( comp[lab].Pt.end(), Pix);
				}
			}
		}
	
		ListComp Clist;
		for(i=0;i<totalcomp;i++){
			MComp MP = ComponentProperty(comp[i].Pt);
			Clist.push_back(MP);
		}
		
		
		free( maptable);
		free( checklabel);
		free( labelMAT );
		return Clist;
	}
	MComp ComponentProperty(ListPix LP){
		MComp MP;
		MP.Pt = LP;
		MP.area = (int)LP.size();
		ListPix::iterator Li = LP.begin();
		MP.B_Box.TopLftV.y = MP.B_Box.BotRgtV.y=(*Li).V.y;
		MP.B_Box.TopLftV.x = MP.B_Box.BotRgtV.x=(*Li).V.x;
		MP.B_Box.MinRowV = MP.B_Box.MaxRowV = MP.B_Box.MinColV = MP.B_Box.MaxColV = (*Li).V;
		for(; Li != LP.end(); ++Li){					
			if(MP.B_Box.TopLftV.y > (*Li).V.y){	MP.B_Box.TopLftV.y = (*Li).V.y;	MP.B_Box.MinRowV = (*Li).V;}
			if(MP.B_Box.BotRgtV.y < (*Li).V.y){	MP.B_Box.BotRgtV.y = (*Li).V.y;	MP.B_Box.MaxRowV = (*Li).V;}
			if(MP.B_Box.TopLftV.x > (*Li).V.x){	MP.B_Box.TopLftV.x = (*Li).V.x;	MP.B_Box.MinColV = (*Li).V;}
			if(MP.B_Box.BotRgtV.x < (*Li).V.x){	MP.B_Box.BotRgtV.x = (*Li).V.x;	MP.B_Box.MaxColV = (*Li).V;}
		}
	
		MP.B_Box.HW.height = MP.B_Box.BotRgtV.y-MP.B_Box.TopLftV.y+1;
		MP.B_Box.HW.width = MP.B_Box.BotRgtV.x-MP.B_Box.TopLftV.x+1;
		return(MP);
	}
	typedef struct MPix2{
		CvPoint V[2];
		int		VI;
		double  VD;
	}MPix2;
	void Swap(int &a, int &b) {
		int c=a;
		a=b;
		b=c;
	}
	bool Overlap2Line( MPix2 A, MPix2 B, double *part){ // The lines are horizontally inclined
		int a1 = A.V[0].x; int a2 = A.V[1].x;
		if( a1 > a2) Swap( a1, a2);
		int b1 = B.V[0].x; int b2 = B.V[1].x;
		if( b1 > b2) Swap( b1, b2);
		int p1, p2;
		if( a2 < b1 || b2 < a1) return false;
		if( a1 <= b1 && a2 >= b2) *part = 100; //B is in A
		else if( b1 <= a1 && b2 >= a2) *part = 100; //A is in B
		else{
			if( a1 < b1) p1 = b1; else p1 = a1;
			if( a2 > b2) p2 = b2; else p2 = a2;
			int d = p2-p1;
			int ad = a2-a1; int bd = b2-b1;
			*part = (double)d*100/MIN(ad, bd);
			//*part = d;
		}
		return true;
	}
	MBoundingBox BoundingBox(ListPix LP) {
		MBoundingBox Box;
		ListPix::iterator Pi = LP.begin();
		Box.BotRgtV.x = Box.TopLftV.x = (*Pi).V.x;	Box.BotRgtV.y = Box.TopLftV.y = (*Pi).V.y;
		Box.MinRowV = Box.MaxRowV = Box.MinColV = Box.MaxColV = (*Pi).V;
	
		for( ; Pi != LP.end(); ++Pi){
			if( (*Pi).V.x < Box.TopLftV.x){	Box.TopLftV.x = (*Pi).V.x; Box.MinColV = (*Pi).V;}
			if( (*Pi).V.x > Box.BotRgtV.x){	Box.BotRgtV.x = (*Pi).V.x;	Box.MaxColV = (*Pi).V;}
			if( (*Pi).V.y < Box.TopLftV.y){	Box.TopLftV.y = (*Pi).V.y;	Box.MinRowV = (*Pi).V;}
			if( (*Pi).V.y > Box.BotRgtV.y){	Box.BotRgtV.y = (*Pi).V.y;	Box.MaxRowV = (*Pi).V;}
		}
		Box.HW.height = Box.BotRgtV.y - Box.TopLftV.y + 1;
		Box.HW.width = Box.BotRgtV.x - Box.TopLftV.x + 1;
		return( Box);
	}
	void CompOverlapRemove(ListComp *comp, double Th){
		int N = (int)comp->size();
		MComp *tmp = new MComp[N];
		int i = 0;
		MPix2 A, B;
		double part;
		for(ListComp::iterator Ci = comp->begin(); Ci != comp->end(); Ci++, i++){
			tmp[i] = *Ci;	tmp[i].flag = 0;
		}
		lab:
		for( i = 0; i < N; i++){
			if(tmp[i].flag == 0){
				A.V[0] = tmp[i].B_Box.MinColV;	A.V[1] = tmp[i].B_Box.MaxColV;	
				for(int j = i+1; j < N; j++){
					int samewidth_th=min(tmp[i].B_Box.HW.width, tmp[j].B_Box.HW.width)/2;
					if(tmp[j].flag==0 && abs(tmp[i].B_Box.HW.width-tmp[j].B_Box.HW.width)<samewidth_th){
						B.V[0] = tmp[j].B_Box.MinColV;	B.V[1] = tmp[j].B_Box.MaxColV;
						bool flag = Overlap2Line(A, B, &part);
						if(flag==TRUE && part>Th){
							tmp[j].flag = 1;
							for(ListPix::iterator Li = tmp[j].Pt.begin(); Li != tmp[j].Pt.end(); ++Li)
								tmp[i].Pt.insert(tmp[i].Pt.end(), *Li);
							tmp[i].B_Box=BoundingBox(tmp[i].Pt);
							goto lab;
						}
					}
				}
			}
		}
		comp->clear();
		for(i = 0; i < N; i++){
			if(tmp[i].flag == 0)
				comp->insert(comp->end(), tmp[i]);
		}
		delete tmp;
	}
	bool MComp::isSmallPartOf(MComp comp, int th) {
		return this->B_Box.BotRgtV.x>=comp.B_Box.TopLftV.x-th 
			&& this->B_Box.TopLftV.x<=comp.B_Box.BotRgtV.x+th
			&& this->B_Box.HW.height<comp.B_Box.HW.height/2
			&& this->B_Box.HW.width<comp.B_Box.HW.height/2;
	}
	bool MComp::boundingBoxOverlaps(MComp comp) {
		if(this->B_Box.TopLftV.x>comp.B_Box.BotRgtV.x 
		|| this->B_Box.BotRgtV.x<comp.B_Box.TopLftV.x
		|| this->B_Box.TopLftV.y>comp.B_Box.BotRgtV.y 
		|| this->B_Box.BotRgtV.y<comp.B_Box.TopLftV.y)
			return false;
		else
			return true;
	}
	void compJoinSmallParts(ListComp &l) {
		int th=0;
		for(ListComp::iterator i=l.begin(); i!=l.end(); i++)
			th+=i->B_Box.HW.width;
		th/=l.size()*4;
		for(bool changed=true; changed; ) {
			changed=false;
			for(ListComp::iterator i=l.begin(); i!=l.end(); i++) {
				for(ListComp::iterator j=(++i)--; j!=l.end(); ) {
					if(i->isSmallPartOf(*j, th) || j->isSmallPartOf(*i, th)) {
						changed=true;
						for(ListPix::iterator Li=j->Pt.begin(); Li!=j->Pt.end(); ++Li)
							i->Pt.insert(i->Pt.end(), *Li);
						i->B_Box=BoundingBox(i->Pt);
						j=l.erase(j);
					} else
						j++;
				}
			}
		}
	}
	void compJoinBoundingBoxOverlap(ListComp &l) {
		for(bool changed=true; changed; ) {
			changed=false;
			for(ListComp::iterator i=l.begin(); i!=l.end(); i++) {
				for(ListComp::iterator j=(++i)--; j!=l.end(); ) {
					if(i->boundingBoxOverlaps(*j)) {
						changed=true;
						for(ListPix::iterator Li=j->Pt.begin(); Li!=j->Pt.end(); ++Li)
							i->Pt.insert(i->Pt.end(), *Li);
						i->B_Box=BoundingBox(i->Pt);
						j=l.erase(j);
					} else
						j++;
				}
			}
		}
	}
	MComp* ListCompToArray(ListComp l) {
		MComp* arr=new MComp[l.size()];
		int i=0;
		for(ListComp::iterator Ci = l.begin(); Ci != l.end(); Ci++, i++) {
			arr[i] = *Ci; arr[i].flag = 0;
		}
		return arr;
	}
	void sortListComp(ListComp &comp_list) {
		MComp* comp_arr=ListCompToArray(comp_list);
		for(int i=0; i<comp_list.size()-1; i++) {
			for(int j=0; j<comp_list.size()-i-1; j++) {
				int xj=comp_arr[j].B_Box.TopLftV.x;
				int xj1=comp_arr[j+1].B_Box.TopLftV.x;
				if(xj>xj1) {
					MComp temp=comp_arr[j];
					comp_arr[j]=comp_arr[j+1];
					comp_arr[j+1]=temp;
				}
			}
		}
		int i=0;
		for(ListComp::iterator Ci=comp_list.begin(); Ci!=comp_list.end(); Ci++, i++)
			*Ci=comp_arr[i];
		delete comp_arr;
	}
	void compFilter(ListComp &comp_list, int MIN_WIDTH, int MIN_HEIGHT=0, double MIN_HEIGHT_WIDTH_RATIO=INFINITY) {
		for(ListComp::iterator i=comp_list.begin(); i!=comp_list.end();) {
			int x=i->B_Box.TopLftV.x;
			int y=i->B_Box.TopLftV.y;
			int width=i->B_Box.HW.width;
			int height=i->B_Box.HW.height;
			if(width>=MIN_WIDTH && height>=MIN_HEIGHT && height/(double)width<=MIN_HEIGHT_WIDTH_RATIO)
				i++;
			else
				i=comp_list.erase(i);
		}
	}
	ListComp getComponents(IplImage* img) {
		IplImage* img_bin=cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);	
		cvThreshold(img, img_bin, 128, 255, CV_THRESH_BINARY);
		cvDilate(img_bin, img_bin, NULL, 3);
		ListComp comp_list=BinImg2Comp(img_bin);
		compFilter(comp_list, 10, 20, 7);
		CompOverlapRemove(&comp_list, 75);
		compJoinSmallParts(comp_list);
		compJoinBoundingBoxOverlap(comp_list);
		compFilter(comp_list, 10, 20, 5);
		sortListComp(comp_list);
		cvReleaseImage(&img_bin);
		return comp_list;
	}
	void saveComp(const char *filename, IplImage* img, MComp comp) {
		IplImage* comp_img=cvCreateImage(comp.B_Box.HW, IPL_DEPTH_8U, 1);
		cvSetZero(comp_img);
		for(ListPix::iterator i=comp.Pt.begin(); i!=comp.Pt.end(); i++)
			((unsigned char*)comp_img->imageData)[comp_img->widthStep*(i->V.y-comp.B_Box.TopLftV.y)+(i->V.x-comp.B_Box.TopLftV.x)]=((unsigned char*)img->imageData)[img->widthStep*(i->V.y)+(i->V.x)*img->nChannels];
		cvSaveImage(filename, comp_img);
		cvReleaseImage(&comp_img);
	}
	IplImage* getCompImage(IplImage* img, MComp comp) {
		IplImage* comp_img=cvCreateImage(comp.B_Box.HW, IPL_DEPTH_8U, 1);
		cvSetZero(comp_img);
		for(ListPix::iterator i=comp.Pt.begin(); i!=comp.Pt.end(); i++)
			((unsigned char*)comp_img->imageData)[comp_img->widthStep*(i->V.y-comp.B_Box.TopLftV.y)+(i->V.x-comp.B_Box.TopLftV.x)]=((unsigned char*)img->imageData)[img->widthStep*(i->V.y)+(i->V.x)*img->nChannels];
		return comp_img;
	}
}

#endif