#include "common.h"
#define CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(lined_image,uchar,i,j) != 0 


IplImage * verticalScan(const IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100])
{
	IplImage *lined_image=0;
	int i,j,i_main,j_main,notfound=1;
	int height,width,find_radius;
	char out_dir_path[100];
	strcpy(out_dir_path,"output\\verticalScan\\out_");
	strcat(out_dir_path,file_name);
	lined_image=cvCreateImage(cvGetSize(image),8,1);

	height=image->height;
	width=image->width;
	//int find_radius=width/80;
	//if(find_radius<5)find_radius=5;
	int min_length_staff=width/3;
	
	find_radius=(int)avg_staff_space*0.9;
	int gap_min=(int)avg_staff_space*0.9-avg_line_width*0.25;
	int gap_max=(int)avg_staff_space*1.1+avg_line_width*1.25;


	vector <int> vpoints;
	int c_width,c_up,c_down;
	cvSet(lined_image,cvScalar(255));
	try{

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
				for(i=i_main+gap_min;notfound && i<i_main+gap_max && i<height;i++)
					 if	( CV_IMAGE_ELEM(image,uchar,i,j) == 0 || CV_IMAGE_ELEM(image,uchar,i,j-1) == 0 ||CV_IMAGE_ELEM(image,uchar,i,j+1) == 0  )
					 {
						 notfound=0;
						 c_down=1;
					 }
				notfound=1;
				c_up=0;
				for(i=i_main-gap_min;notfound && i>i_main-gap_max && i>=0;i--)
					 if	( CV_IMAGE_ELEM(image,uchar,i,j) == 0  || CV_IMAGE_ELEM(image,uchar,i,j-1) == 0 ||CV_IMAGE_ELEM(image,uchar,i,j+1) == 0  )
					 {
						 notfound=0;
						 c_up=1;
					 }
				if(c_up==1 || c_down==1)
				{
					//vpoints.push_back(i_main);
					c_down=0;
					c_up=0;
					for(i=i_main;i<i_main+avg_line_width &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i++);
					c_down=i;
					for(i=i_main;i>i_main-avg_line_width &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i--);
					c_up=i;

					if((c_down-c_up-1)<avg_line_width*1.7+avg_staff_space*0.2 && (c_down-c_up-1)>=avg_line_width*0.5)
						vpoints.push_back(i_main);	
				}
			}
		}



		for(i_main=0;i_main<vpoints.size();i_main++)
		{
			cvSetReal2D(lined_image,vpoints[i_main],j,0);
		}
		vpoints.clear();
	}
	}catch( cv::Exception& e )
		{
			const char* err_msg = e.what();
			cout << "\nCaught an exception --Vertical Scan "<<err_msg<<"\n";
		}
	catch(...)
	{
		cout << "\nCaught an exception --Vertical Scan _DEFAULT\n";
	}

#ifdef _DEBUG
	printf("\nCall verticalScan.");
	cvSaveImage(out_dir_path,lined_image);
#endif
	return lined_image;
}