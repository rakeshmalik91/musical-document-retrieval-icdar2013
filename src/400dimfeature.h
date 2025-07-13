#ifndef _400DIMFEATURE_H
#define _400DIMFEATURE_H

#include "image_processing.h"

using namespace image_processing;

namespace image_processing {
	void division(int k,int a[10],float dim) {
		float p,t,j=0;
		int q,i,prev=0;
	       
		p=k/dim;
		//printf("%f===  ",p);
	
		for(i=0;i<dim;i++){
			j=j+p;
			q=j;
			t=q+0.5;
			if (j>t)  a[i]= q+1-prev;
			else a[i]=q-prev;
			prev=prev+a[i];
		}
	
	}
	typedef struct bbox{
		int sr, er, sc, ec;
	
	}bbox;
	bbox FindBoundingBox( short **bin, bbox bb){
		int bsr,ber,bsc,bec,fla;
		int i,j;
		bbox rbb;
		fla=0;
		//----------------------------------------End col
		bec=bb.ec;
		for(j=bb.ec;j>bb.sc && fla==0;j--){
			for(i=bb.sr;i<bb.er && fla==0;i++){
				if(bin[i][j]==1){
					bec=j;
					fla=1;
				}
			}
		}
		if(bec<bb.ec) bec=bec+1;
		if(fla==1){
			//------------------------------------------START col
			fla=0;bsc=bb.sc;
			for(j=bb.sc;j<bec && fla==0 ;j++){  ///********endcolumn
				for(i=bb.sr;i<bb.er && fla==0;i++){
					if(bin[i][j]==1){
						bsc=j;
						fla=1;
					}
				}
			}
				
			//if(bec>startcolumn) bsc=bsc-1;
			//-------------------------------------------START row
			fla=0;bsr=bb.sr;
			for(i=bb.sr;i<bb.er && fla==0;i++){
				for(j=bsc;j<bec && fla==0;j++){    //startcolumn***endcolumn
					if(bin[i][j]==1){
						bsr=i;
						fla=1;
					}
				}
			}
			//---------------------------------------------END row
			fla=0;ber=bb.er;
			for(i=bb.er;i>=bsr && fla==0;i--){//start_row
				for(j=bsc;j<bec && fla==0;j++){
					if(bin[i][j]==1){
						ber=i;
						fla=1;
					}
				}
			}
		
			rbb.ec=bec;		rbb.er=ber;		rbb.sc=bsc;		rbb.sr=bsr;
		}
		else{
			rbb.ec=0;		rbb.er=0;		rbb.sc=0;		rbb.sr=0;
		
		}
		
		return rbb;
	
	}
	FeatureVector dimension400feature(short **image,int row,int col) {
		int i,j,k,count=0,m,n,rxy,r11[20],s1,t,y,z,ro,co,i1,j1,c11[20],u,y1,h11;
		float d[9][9][16]={0},nc1[11][11][16]={0},counter=0,nc2[5][5][16]={0},delv,delu,fxy,angxy,max,total=0,mean;
		float **img,**img1,*appe,s;
		appe=new float[400];
	
		img =(float **) calloc(row , sizeof(float *));
	
		for (i = 0; i < row; i++) {
			img[i] = (float *)calloc(col , sizeof(float));
		}
		
		for(i=0;i<row;i++){
			for(j=0;j<col;j++){
				img[i][j]=(float)image[i][j];
			 }
		}
		u=col-1;
		y1=row-1;
		/*** finding bounding box of input image***/
		bbox b1,b2;
		b1.sr=0;	b1.er=row-1;
		b1.sc=0;	b1.ec=col-1;
		b2=FindBoundingBox(image, b1);
		
		ro=b2.er-b2.sr+1;
		co=b2.ec-b2.sc+1;
	
		for(i=b2.sr;i<=b2.er;i++){
			for (j=b2.sc;j<=b2.ec;j++){
				if(img[i][j]==1)
					counter++;
				}
			}
	
		for(k=1;k<=5;k++){
			for(i=b2.sr;i<=b2.er;i++){
				for (j=b2.sc;j<=b2.ec;j++){
					s=0;
					s=s+img[i][j];
					if (i>=b2.sr && i<=b2.er && j+1>=b2.sc && j+1<=b2.ec)
						s=s+img[i][j+1];
					if (i+1>=b2.sr && i+1<=b2.er && j>=b2.sc && j<=b2.ec)
						s=s+img[i+1][j];
					if (i+1>=b2.sr && i+1<=b2.er && j+1>=b2.sc && j+1<=b2.ec)
						s=s+ img[i+1][j+1];
					img[i][j]=s;
				}
			}
		}
	
	
		for(i=b2.sr;i<=b2.er;i++){
			for(j=b2.sc;j<=b2.ec;j++){
				total=total+img[i][j];
			}
		}
		count=ro*co;
		mean=total/count;
		//	printf("===%f===",mean);
	
		for(i=b2.sr;i<=b2.er;i++){
			for(j=b2.sc;j<=b2.ec;j++){
				img[i][j]=img[i][j]-mean;
			}
		}
			
	
		max=img[b2.sr][b2.sc];
		for(i=b2.sr;i<=b2.er;i++){
			for(j=b2.sc;j<=b2.ec;j++){
				if(max<img[i][j])
					max=img[i][j];
				}
			}
		//printf("```%f```",max);
	
		for(i=b2.sr;i<=b2.er;i++){
			for(j=b2.sc;j<=b2.ec;j++){
				img[i][j]=img[i][j]/max;
			}
		}
			
		division(ro,r11,9);
		division(co,c11,9);
		
		img1 =(float **) calloc((ro+1) , sizeof(float *));
	
		for (i = 0; i < (ro+1); i++) {
			img1[i] = (float *)calloc((co+1) , sizeof(float));
		}
	 
	
		for(i=b2.sr,i1=0;i<=b2.er,i1<ro;i++,i1++){
			for(j=b2.sc,j1=0;j<=b2.ec,j1<co;j++,j1++){
				img1[i1][j1]=img[i][j];
			}
		}	
	
		for(i=0;i<ro;i++)
			img1[i][co]=(img1[i][co-1]+ img1[i][co-2])/2;
	
		for(j=0;j<co;j++){
			
			img1[ro][j]=(img1[ro-1][j]+ img1[ro-2][j])/2;
		}
	
		img1[ro][co]=(img1[ro-1][co-1]+img1[ro-2][co-2])/2;
	
	
		for(m=0;m<9;m++)
			for(n=0;n<9;n++){
				s1=0;
				t=0;
				y=0;z=0;
				for(i=0;i<m;i++)//for row
					y=y+r11[i];
	
				z=y+r11[m];
				for(j=0;j<n;j++)//for colm
					s1=s1+c11[j];
	
				t=s1+c11[n];
	
				for(i=y;i<z;i++){
					for(j=s1;j<t;j++){
						fxy=0;
						angxy=0;
						delu=0;
						delv=0;
						delu=img1[i+1][j+1]-img1[i][j];// this should be [i+1][j]-[i][j]
				 
						delv=img1[i+1][j]-img1[i][j+1];//////  this should be [i][j+1]-[i][j];
						if(delu==0)
							continue; 
						else{
							fxy=sqrt((delu*delu)+(delv*delv));
				 
							angxy=(atan2(delv,delu)*180)/3.142857;
							if(angxy<0.0)
							angxy=angxy+360;
				 
							rxy=angxy/22.5;
				
							d[m][n][rxy]=d[m][n][rxy]+fxy;
						}
					}
				}
			}
	
	
		for(i=0;i<9;){
			for(j=0;j<9;){
				for(k=0;k<16;k++){
					d[i][j][k]=d[i][j][k]*2;
				}
				j=j+2;
			}
			i=i+2;
		}
	
		for(i=1;i<10;i++){
			for(j=1;j<10;j++){
				for(k=0;k<16;k++)
					nc1[i][j][k]=d[i-1][j-1][k];
			}
		}
		for (i=1;i<10;i+=2){
			for (j=1;j<10;j+=2){
				for(k=0;k<16;k++)
				nc2[i/2][j/2][k]=nc1[i][j][k]+nc1[i-1][j-1][k]+nc1[i-1][j][k]+nc1[i-1][j+1][k]+
				nc1[i][j-1][k]+nc1[i][j+1][k]+nc1[i+1][j-1][k]+nc1[i+1][j][k]+nc1[i+1][j+1][k];
			}
		}
	
		h11=0;
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
				for(k=0;k<16;k++)
				{
					appe[h11]=nc2[i][j][k]/counter;
					h11++;
				}
	
		for (i = 0; i < row; i++)
			free(img[i]);
		free(img);
		for (i = 0; i < (ro+1); i++) 
			free(img1[i]);
		free(img1);
		
		//writefile (appe,400);
	
		return (appe);
	}
	short** image2Array(IplImage *img) {
		short **arr=new short*[img->height];
		for(int i=0; i<img->height; i++) {
			arr[i]=new short[img->width];
			for(int j=0; j<img->width; j++)
				arr[i][j]=img->imageData[i*img->widthStep+j];
		}
		return arr;
	}
	FeatureVector dimension400feature(IplImage *img) {
		short** imgarr=image2Array(img);
		FeatureVector f=dimension400feature(imgarr, img->height, img->width);
		for(int i=0; i<img->height; i++)
			delete imgarr[i];
		delete imgarr;
		return f;
	}
}
	
#endif