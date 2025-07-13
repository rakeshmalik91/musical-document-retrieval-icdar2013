#include "common.h"
#define CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(lined_image,uchar,i,j) != 0 


IplImage * scanfor5(IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100])
{
	IplImage *lined_image=0;
	int i,j,i_main,j_main,notfound=1;
	int height,width,find_radius;
	char out_dir_path[100];
	strcpy(out_dir_path,"output\\scanfor5\\out_");
	strcat(out_dir_path,file_name);
	lined_image=cvCreateImage(cvGetSize(image),8,1);

	height=image->height;
	width=image->width;
	int min_length_staff=width/3;
	//int find_radius=width/80;
	//if(find_radius<5)find_radius=5;
	find_radius=avg_staff_space*0.9;
	int gap_min=avg_staff_space*0.8;//-avg_line_width*0.25;
	int gap_max=avg_staff_space*1.2+avg_line_width;//*1.25;


	vector <int> vpoints;
	int c_width,c_down;
	cvSet(lined_image,cvScalar(255));

	for(j_main=0;j_main<width;j_main++)
	{
		for(i_main=0;i_main<height;i_main++)
		{
			i=i_main;
			j=j_main;
			c_width=0;
			if( CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0  )
			{
				notfound=1;
				c_down=0;
				for(int p=i_main+gap_min;notfound && p<i_main+gap_max;p++)
					if	( CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
					{
						for(int q=i_main+gap_min+(avg_staff_space+avg_line_width);notfound && q<i_main+gap_max+(avg_staff_space+avg_line_width);q++)
							if	( CV_IMAGE_ELEM(thinned_image,uchar,q,j) == 0  )
							{
								for(int r=i_main+gap_min+2*(avg_staff_space+avg_line_width);notfound && r<i_main+gap_max+2*(avg_staff_space+avg_line_width);r++)
									if	( CV_IMAGE_ELEM(thinned_image,uchar,r,j) == 0  )
									{
										for(int s=i_main+gap_min+3*(avg_staff_space+avg_line_width);notfound && s<i_main+gap_max+3*(avg_staff_space+avg_line_width);s++)
											if	( CV_IMAGE_ELEM(thinned_image,uchar,s,j) == 0  )
											{
												notfound=0;
												vpoints.push_back(i_main);
												vpoints.push_back(p);
												vpoints.push_back(q);
												vpoints.push_back(r);
												vpoints.push_back(s);

											}
									}
							}
					}

			}

		}


		for(i_main=0;i_main<vpoints.size();i_main++)
		{
			cvSetReal2D(lined_image,vpoints[i_main],j,0);
		}
		vpoints.clear();
	}

#ifdef _DEBUG
	printf("\nCall scanfor5.");
	cvSaveImage(out_dir_path,lined_image);
#endif
	return lined_image;
}