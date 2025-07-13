#include "common.h"
#define CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(lined_image,uchar,i,j) != 0 


IplImage * scanfor5curve(IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100])
{
	IplImage *lined_image=0;
	int i,j,i_main,j_main,notfound=1;
	int height,width,find_radius;
	char out_dir_path[100];
	strcpy(out_dir_path,"output\\scanfor5\\out_");
	strcat(out_dir_path,file_name);
	lined_image=cvCreateImage(cvGetSize(image),8,1);
	//cvEqualizeHist(gray_orig,gray_orig);
	//cvSmooth( thinned_image, thinned_image, CV_GAUSSIAN, 3, 3 );
	cvErode(thinned_image, thinned_image, NULL ,1);
	cvDilate(thinned_image, thinned_image,NULL,1);
	height=image->height;
	width=image->width;
	int min_length_staff=width/3;
	//int find_radius=width/80;
	//if(find_radius<5)find_radius=5;
	//find_radius=avg_staff_space*0.9;
	int gap_min=avg_staff_space*0.85;
	int gap_max=avg_staff_space*1.15;
	int gap_min2=avg_staff_space*0.75;
	int gap_max2=avg_staff_space*1.25;

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
				int count=1,p,q,r,s;
				int pos[5]={1,0,0,0,0};
				for(p=i_main+gap_min;notfound && p<i_main+gap_max+avg_line_width*1.25;p++)
				{	
					if	(p>0 && p<height &&  CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
					{
						count++;
						pos[1]=1;
						break;
					}
				}
				for( q=i_main+gap_min+(avg_staff_space+avg_line_width);notfound && q<i_main+gap_max+(avg_staff_space+avg_line_width)+avg_line_width*1.25;q++)
				{
					if	(q>0 && q<height && CV_IMAGE_ELEM(thinned_image,uchar,q,j) == 0  )
					{
						count++;
						pos[2]=1;	
						break;
					}
				}
				for(r=i_main+gap_min+2*(avg_staff_space+avg_line_width);notfound && r<i_main+gap_max+2*(avg_staff_space+avg_line_width)+avg_line_width*1.25;r++)
				{
					if	( r>0 && r<height &&  CV_IMAGE_ELEM(thinned_image,uchar,r,j) == 0  )
					{
						count++;
						pos[3]=1;
						break;
					}
				}
				for( s=i_main+gap_min+3*(avg_staff_space+avg_line_width);notfound && s<i_main+gap_max+3*(avg_staff_space+avg_line_width)+avg_line_width*1.25;s++)
				{
					if	( s>0 && s<height &&  CV_IMAGE_ELEM(thinned_image,uchar,s,j) == 0  )
					{
						count++;
						pos[4]=1;
						break;
					}
				}
				if(count>=2)
				{
					notfound=0;
					//if(pos[0]==1)
					vpoints.push_back(i_main);
					if(pos[1]==1)vpoints.push_back(p);
					if(pos[2]==1)vpoints.push_back(q);
					if(pos[3]==1)vpoints.push_back(r);
					if(pos[4]==1)
					{
						vpoints.push_back(s);
						for(int z=1;z<=3;z++)
						{
							if(pos[z]==0)
							{
								for(p=i_main+gap_min2+(z-1)*(avg_staff_space+avg_line_width);p<i_main+gap_max2+(z-1)*(avg_staff_space+avg_line_width)*1.25;p++)
									if	(p>0 && p<height &&  CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
										cvSetReal2D(lined_image,p,j,0);
							}
						}
					}
					if(pos[4]==0)
					{
						for(p=i_main+gap_min2+3*(avg_staff_space+avg_line_width);p<i_main+gap_max2+3*(avg_staff_space+avg_line_width)*1.25;p++)
								if	(p>0 && p<height &&  CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
										cvSetReal2D(lined_image,p,j,0);
						for(p=i_main+gap_min2-1*(avg_staff_space+avg_line_width);p<i_main+gap_max2-1*(avg_staff_space+avg_line_width)*1.25;p++)
								if	(p>0 && p<height &&  CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
										cvSetReal2D(lined_image,p,j,0);
						if(pos[3]==0)
						{
							for(p=i_main+gap_min2+4*(avg_staff_space+avg_line_width);p<i_main+gap_max2+4*(avg_staff_space+avg_line_width)*1.25;p++)
								if	(p>0 && p<height &&  CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
										cvSetReal2D(lined_image,p,j,0);
							for(p=i_main+gap_min2-2*(avg_staff_space+avg_line_width);p<i_main+gap_max2-2*(avg_staff_space+avg_line_width)*1.25;p++)
								if	(p>0 && p<height &&  CV_IMAGE_ELEM(thinned_image,uchar,p,j) == 0  )
										cvSetReal2D(lined_image,p,j,0);
					

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
	printf("\nCall scanfor5curve.");
	cvSaveImage(out_dir_path,lined_image);
#endif
	return lined_image;
}