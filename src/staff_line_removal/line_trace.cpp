#include "common.h"
#define CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(lined_image,uchar,i,j) != 0 
typedef struct sLine
{
	int id;
	int valid;
	int height;
	char type;
	int length;
	vector <CvPoint> points;
}StaffLine;

IplImage * line_trace(IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100])
{
	IplImage *lined_image=0;
	int i,j,k,count,i_main,j_main;
	int height,width,find_radius;
	char out_dir_path[100];
	strcpy(out_dir_path,"output\\TRACE_line\\out_");
	strcat(out_dir_path,file_name);
	lined_image=cvCreateImage(cvGetSize(image),8,1);

	height=image->height;
	width=image->width;
	int min_length_staff=width/10;
	find_radius=width/80;
	if(find_radius<5)find_radius=5;
	find_radius=2;
	vector <StaffLine> StaffLines;
	int sl_no=0;
	CvPoint lastpoint;
	int flag_continue_find;
	cvSet(lined_image,cvScalar(255));
	//IplImage *disp_image=cvCreateImage(cvGetSize(image),8,3);
	//*cvMerge(lined_image,lined_image,lined_image,NULL,disp_image);
	//cvNamedWindow("lined_image",WIND_TYPE);
	for(j_main=0;j_main<width;j_main++)
	{
		for(i_main=0;i_main<height;i_main++)
		{

			flag_continue_find=1;
			i=i_main;
			j=j_main;
			if(CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) )
			{
				StaffLine *SL_current=new StaffLine;
				SL_current->id=sl_no;
				SL_current->length=0;
				sl_no++;

				while (flag_continue_find)
				{
					flag_continue_find=0;
					while(j<(width-2) && CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j))
					{
						lastpoint=cvPoint(i,j);
						SL_current->points.push_back(lastpoint);
						SL_current->length++;
						j++;
					}
					for(int dist=0;flag_continue_find==0 && dist<find_radius;dist++)
					{
						for(int v_diff=0;flag_continue_find==0 && v_diff<=find_radius;v_diff++)
						{
							if(j+v_diff<(width-2)  && (i+dist<height) && CHECK_BLACK_NOT_COVERED(image,lined_image,i+dist,j+v_diff) )
							{
								lastpoint=cvPoint(i+dist,j+v_diff);
								SL_current->points.push_back(lastpoint);
								SL_current->length++;
								flag_continue_find=1;
							}
							else if(dist!=0 && j+v_diff<(width-2) && (i-dist>0) && CHECK_BLACK_NOT_COVERED(image,lined_image,i-dist,j+v_diff))
							{
								lastpoint=cvPoint(i-dist,j+v_diff);
								SL_current->points.push_back(lastpoint);
								SL_current->length++;
								flag_continue_find=1;
							}
						}
					}	
					if(flag_continue_find==1)
					{
						j=lastpoint.y;
						i=lastpoint.x;
					}
				}

				if(SL_current->length>min_length_staff)
				{
					//cout<<"\nSL_current->length "<< SL_current->length;
					for(k=0;k<SL_current->length;k++)
					{
						i=SL_current->points[k].x;
						j=SL_current->points[k].y;
						cvSetReal2D(lined_image,i,j,CV_IMAGE_ELEM(image,uchar,i,j));

						for(int up=1;up<ceil((float)avg_line_width/2);up++)
						{
							if(CV_IMAGE_ELEM(image,uchar,i+up,j) == 0)
								cvSetReal2D(lined_image,i+up,j,0);
							if(CV_IMAGE_ELEM(image,uchar,i-up,j) == 0)
								cvSetReal2D(lined_image,i-up,j,0);
						}
					}

				}
				delete SL_current;

			}

			count=0;
		}
		if(j_main%10==0)
		{
			//cout<<"\ni_main "<<i_main;
			//cvShowImage("lined_image",lined_image);
			//cvSaveImage("lined_image.png",lined_image);
			//cvWaitKey(10);

		}
	}

#ifdef _DEBUG
	printf("\nCall Line Trace.");
	cvSaveImage(out_dir_path,lined_image);
#endif
	//cvReleaseImage(&disp_image);
	return lined_image;
}