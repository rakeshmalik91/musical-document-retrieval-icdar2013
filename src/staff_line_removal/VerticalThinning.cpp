#include "common.h"
#define MAX_VERTICAL_BLACK 10000

int VerticalThinning(IplImage *inp_image,IplImage *out_image,int &staff_height,int &staff_space,char file_name[100])
{
	int i,j,k,count;
	vector <int> pos;
	int height,width,start,end,p_height[MAX_VERTICAL_BLACK],p_space[MAX_VERTICAL_BLACK],max=0,max_pos,s_max,s_max_pos;
	width=inp_image->width;
	height=inp_image->height;
	char out_dir_path[100];
	strcpy(out_dir_path,"output\\thin\\out_");
	strcat(out_dir_path,file_name);
	IplImage* image=cvCloneImage(inp_image);
	for(i=0;i<MAX_VERTICAL_BLACK;i++)
	{
		p_height[i]=0;
		p_space[i]=0;
	}									  
	cvSet(out_image,cvScalar(255));	 //blank white image
	for(j=2;j<width-2;j++){
		end=0;
		
		for(i=2;i<height-2;i++){
			  start=i;	   //start of black pixel
			 //  if( j==63) 
				//	  cout<<i; 
			  while(i<height-2 && CV_IMAGE_ELEM(image,uchar,i,j)==0)
			  {
				  end=i++;	 //end of black pixel
			  }
			  if(end>=start)
			  {
				  count=0;
				  pos.clear();
				  /*decide where to place thinned pixel depending on previous column*/
				  for(k=start-1;k<=end+1;k++)
				  {
						if(CV_IMAGE_ELEM(out_image,uchar,k,j-1)==0)	  //check previous column of thinned image
						{
							pos.push_back(k);
							count++;
						}
				  }
				
				  switch(count)
				  {
				  case 0:
					  cvSetReal2D(out_image,(start+1+end)/2,j,0);	break; //thinned position	at center
				  case 1:
					  if ((start+1+end)/2==pos[0])
						  cvSetReal2D(out_image,pos[0],j,0); //at center
					  else if ((start+1+end)/2>pos[0])
						  cvSetReal2D(out_image,pos[0]+1,j,CV_IMAGE_ELEM(image,uchar,pos[0]+1,j)); //previous + 1 to keep connected
					  else
						  cvSetReal2D(out_image,pos[0]-1,j,CV_IMAGE_ELEM(image,uchar,pos[0]-1,j));//previous - 1 to keep connected
					 // cvSetReal2D(out_image,(start+1+end)/2,j,0);
					  break; 
				  default: //for multiple previous
					  for(k=0;k<count;k++)
						 if(k!=0 && pos[k-1]!=pos[k]-1 && pos[k-1]!=pos[k]+1  )
							 cvSetReal2D(out_image,pos[k],j,0); //at all the previous points	
						 else if(k==0)cvSetReal2D(out_image,pos[k],j,0); 
					  break;
				  }
				  p_height[end-start+1]++;  //height calculation
			  }
			  while(i<height-2 && CV_IMAGE_ELEM(image,uchar,i,j)==255)
			  {
				   start=i++;	  //end of white // start of next black
			  }
			  if(start-end>0)
			  {
				  p_space[start-end-1]++;  //space calculation
				  i--;
			  }
		}
	}
	max=s_max=0;
	//#pragma omp parallel for
	for(i=0;i<MAX_VERTICAL_BLACK;i++){
		if(p_height[i]>max)
		{
			max=p_height[i];
			max_pos=i;
		}
		if(p_space[i]>s_max)
		{
			s_max=p_space[i];
			s_max_pos=i;
		}
		
	}

#ifdef _DEBUG
	cout<<"\nCall Thinning: Staff Height "<<max_pos<<" and space "<<s_max_pos;
	cvSaveImage(out_dir_path,out_image);
#endif
	staff_height=max_pos;
	staff_space=s_max_pos;
	cvReleaseImage(&image);
	return 1;
}