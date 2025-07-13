#include "common.h"
#define MAX_VERTICAL 10000

char type_detect(IplImage* image,int avg_line_width,int avg_staff_space,int positions[200],int &Staff_count,float &Variation,char file_name[100])
{
	char ans='u';	 //Not Straight (Curved/Rotated)
	int i,j,k,count,height_count[MAX_VERTICAL];
	int height,width,max=0,Total=0,var_sum=0,var_c=0;
	Staff_count=0;
	double mul=1,var_avg=0;
	width=image->width;
	height=image->height;
	int verti_l=MAX_VERTICAL;
	if(height>verti_l)mul=height/(double)verti_l;
	else verti_l=height;
	char out_dir_path[100];
	strcpy(out_dir_path,"output\\type\\out_");
	strcat(out_dir_path,file_name);
	IplImage *out_image=cvCreateImage(cvSize(width,verti_l+2),8,1);
	cvSet(out_image,cvScalar(255));	
//#pragma omp parallel for
#pragma loop count min(512)
	for(i=0;i<verti_l;i++)
	{
		height_count[i]=0;
	}									  

	for(j=2;j<width-2;j++){
		for(i=2;i<height-2;i++){
			if(CV_IMAGE_ELEM(image,uchar,i,j)==0)
			{
				height_count[(int)(i/mul)]++;
				Total++;
			}
		}
	}


	for(i=1;i<verti_l-1;i++){
		if(height_count[i]>max)max=height_count[i];
#ifdef _DEBUG
		cvLine(out_image,cvPoint(height_count[i-1],i-1),cvPoint(height_count[i],i),cvScalar(0));
#endif
	}
	try {
		if(max>5*Total/verti_l+width/9)  
		{
			ans='s'  ;	// Straight
			int hist_line_thresh=(max+Total/verti_l)/8+Total/verti_l;
			count=0;
			int Consecutive_count=0;
			for(i=1;i<verti_l-1;i++)
			{
				if(height_count[i]>hist_line_thresh)
				{
					if(count!=0 && positions[count-1]==(i-1)*mul)
					{
						Consecutive_count++;
					}
					positions[count]=(int)i*mul;
					count++;
				   if(count>100)
				   {
					   count=0;
					  
					   if(hist_line_thresh!=(max+Total/verti_l)/4+Total/verti_l)
						{
							hist_line_thresh=(max+Total/verti_l)/4+Total/verti_l;
							 i=1;
							cout<<"!! higher hist_line_thresh !!";
					   }else
					   {
						   ans='u';
						    i=verti_l;
							cout<<"!! higher hist_line_thresh !! did not work";
					   }

				   }
#ifdef _DEBUG
					cvLine(out_image,cvPoint(width-25,i),cvPoint(width-1,i),cvScalar(0));
#endif
				}else if(Consecutive_count>0)
				{
					count-=Consecutive_count;//removing extra counts
					positions[count-1]=positions[count-1+(int)floor((float)Consecutive_count/2)]; //saving average position in last position
					var_sum+=Consecutive_count;
					var_c++;
					Consecutive_count=0;
				}

			}

			if(count<5) //if count of lines is too less then reseting status to non-straight
			{
				Staff_count=0;
				ans='u';
			}else
				Staff_count=count;

#ifdef _DEBUG
			cvLine(out_image,cvPoint(hist_line_thresh,0),cvPoint(hist_line_thresh,verti_l),cvScalar(0));
#endif
		}
		if(var_c>0){
			var_avg=(float)var_sum/var_c;
			if(var_avg>2.6)ans='y';//Y variation
			else if((float)max/width<=0.45 && var_avg<2)ans='n';//interrupted
			Variation=var_avg;
		}
	}
	
	catch (...) {                       // catch an error
		cout << "Caught an exception --TYPE SR";
	}
	  
#ifdef _DEBUG
	cout<<"\nCall TypeDetect";
	cout<<"var_avg "<<var_avg<<"var_avg %"<< (var_avg)*100/avg_line_width<<" max"<<max<<"max %"<<max*100/width<<endl;
	if(ans=='s')
		cout<<" : Straight";
	else if(ans=='n')
		cout<<" : Straight interrupted";
	else if(ans=='y')
		cout<<" : Straight Y variation";
	else 
		cout<<" : Not Straight (Curved/Rotated)";
	cvSaveImage(out_dir_path,out_image);
#endif
	cvReleaseImage(&out_image);
	return ans;
}