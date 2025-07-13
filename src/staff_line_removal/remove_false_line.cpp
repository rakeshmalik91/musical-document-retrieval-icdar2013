#include "common.h"

IplImage * remove_false_line(const IplImage * lined_image,int avg_staff_width,int avg_staff_space,int min_area,int radius,char file[100],int x_priority)
	{
	IplImage *copy_line_image=0;
	int i,j,k,l,k1,l1,i_t,j_t,a,b,count;
	int height,width;//,avg_staff_space;
	int dir_mat[9][2]={{0, 0},
		{-1, +1},
		{0, +1},
		{+1, +1},
		{+1, 0},
		{+1, -1},
		{0, -1},
		{-1, -1},
		{-1, 0}
		};
	char str[100];

	copy_line_image=cvCloneImage(lined_image);

	height=lined_image->height;
	width=lined_image->width;


	strcpy(str,"output\\remove_false_line\\out_");
	strcat(str,file);


	for(i=2;i<height-2;i++){
		for(j=0;j<width;j++){
			if(CV_IMAGE_ELEM(lined_image,uchar,i,j)==0){
				count=0;
				/*
				for(q=1;q<=radius;q++)
				{

				for( a=-q-x_priority;a<=q+x_priority && count<=min_area;a++)
				{
				i_t=i+a;
				j_t=j+q;
				if((i_t)<height && (j_t)<width && (j_t)>=0 && (i_t)>=0 && (CV_IMAGE_ELEM(lined_image,uchar,i_t,j_t)==0))count++;
				j_t=j-q;
				if((i_t)<height && (j_t)<width && (j_t)>=0 && (i_t)>=0 && (CV_IMAGE_ELEM(lined_image,uchar,i_t,j_t)==0))count++;
				}
				for(a=-q+1;a<q && count<=min_area;a++)
				{
				i_t=i+q;
				j_t=j+a;
				if((i_t)<height && (j_t)<width && (j_t)>=0 && (i_t)>=0 && (CV_IMAGE_ELEM(lined_image,uchar,i_t,j_t)==0))count++;
				i_t=i-q;
				if((i_t)<height && (j_t)<width && (j_t)>=0 && (i_t)>=0 && (CV_IMAGE_ELEM(lined_image,uchar,i_t,j_t)==0))count++;
				}

				}
				*/
				for(a=-x_priority-radius;a<=radius+x_priority && count<=min_area;a++)
					{

					for( b=-radius;b<=radius && count<=min_area;b++)
						{
						i_t=i+a;
						j_t=j+b;
						if((i_t)<height && (j_t)<width && (j_t)>=0 && (i_t)>=0 && (CV_IMAGE_ELEM(lined_image,uchar,i_t,j_t)==0))
							count++;
						}
					}
				if(count<=min_area)
					cvSetReal2D(copy_line_image,i,j,255); 
				}else
				{
				//add pixel
				count=0;
				int min_area2=min_area*3+4*radius;
				for(a=-x_priority-radius;a<=radius+x_priority && count<=min_area2;a++)
					{

					for( b=-radius;b<=radius && count<=min_area2;b++)
						{
						i_t=i+a;
						j_t=j+b;
						if((i_t)<height && (j_t)<width && (j_t)>=0 && (i_t)>=0 && (CV_IMAGE_ELEM(lined_image,uchar,i_t,j_t)==0))
							count++;
						}
					}
				if(count>=min_area2)
					cvSetReal2D(copy_line_image,i,j,0); 
					}	
			}
		}
#ifdef _DEBUG
	cvSaveImage(str,copy_line_image);
	printf("\nCall Remove_False_line");
#endif
	return copy_line_image;
	}