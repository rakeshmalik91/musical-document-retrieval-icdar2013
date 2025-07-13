#include "common.h"
#define CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(lined_image,uchar,i,j) != 0 

IplImage * HorizontalScan(const IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,int staff_positions[200],int Staff_count,char &type,float Variation,char file_name[100])
{
	IplImage *lined_image=0;
	int i,j,i_main,j_main;
	int height,width,searh_h,line_max_width,line_min_width;
	char out_dir_path[100];
	vector <SE> up,down;
	SE up_pt,dw_pt;
	strcpy(out_dir_path,"output\\horizontalScan\\out_");
	strcat(out_dir_path,file_name);
	lined_image=cvCreateImage(cvGetSize(image),8,1);
	//	int staff_positions_last[200];
	height=image->height;
	width=image->width;
	try{

		int min_length_staff=width/3;
		int line_top=0,line_bottom=0,diff=0,b_count_t=0,b_count_b=0,up_start=0,up_end=0,down_start=0,down_end=0,j_temp;
		if(Variation>4.55)
		{
			searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2);	//avg_staff_space/6 is optimized

			line_max_width= (int)ceil((float)avg_line_width*1.1);
			line_min_width=(int)floor((float)avg_line_width*0.9);
			if(Staff_count%5!=0)
			{
				line_max_width= (int)ceil((float)avg_line_width*1.7);
				line_min_width=(int)floor((float)avg_line_width*0.5);
			}
			/*	if(avg_line_width>=5)
			{
			searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2);
			line_max_width= (int)ceil((float)avg_line_width*1.7);
			line_min_width=(int)floor((float)avg_line_width*0.5);
			}*/
		}else
			if(Variation>2.6)
			{
				searh_h=(int)ceil((float)avg_staff_space/6);	//Y variation, typeset-emulation	 
				line_max_width= (int)ceil((float)avg_line_width*1.2);
				line_min_width=(int)floor((float)avg_line_width*0.8);
				/*	if(avg_line_width>=5)
				{
				searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2);
				line_max_width= (int)ceil((float)avg_line_width*1.7);
				line_min_width=(int)floor((float)avg_line_width*0.5);
				}*/
			}else
			{
				if(Variation>=1.5)
				{
					if(avg_line_width==3)//kanungo 2.045430474	1.5	2.577778	3	25
					{
						searh_h=(int)ceil((float)avg_staff_space/9);	//9
						line_max_width= (int)ceil((float)avg_line_width*1.8);		//1.7-1.9
						line_min_width=(int)floor((float)avg_line_width*0.6);		//0.2-0.6
					}
					if(avg_line_width==2)	//typeset-emulation	  3.01635887	1.36	4.8	2	26
					{
						searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2); //5-6
						line_max_width= (int)ceil((float)avg_line_width*1.9);	//1-2.5
						line_min_width=(int)floor((float)avg_line_width*0.4);
					}
				}else 
				{
					searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2);	
					if(avg_line_width>=4)		//staffline-thickness-variation
					{
						searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2);
						line_max_width=(int)ceil((float)avg_line_width*1.6); 
						line_min_width=(int)floor((float)avg_line_width*0.4);
					}
					else if(avg_line_width>=3)		//staffline-thickness-variation
					{
						searh_h=(int)ceil((float)avg_staff_space/8+avg_line_width/2);
						line_max_width=(int)ceil((float)avg_line_width*2.1); 
						line_min_width=(int)floor((float)avg_line_width*0.4);
					}
					else	 //ideal whitespeckles
					{
						searh_h=(int)ceil((float)avg_staff_space/6+avg_line_width/2);
						line_max_width= (int)ceil((float)avg_line_width*1.2);
						line_min_width=(int)floor((float)avg_line_width*0.8);
					}
				}
				if(type=='n')
				{
					searh_h=(int)ceil((float)avg_staff_space/8+avg_line_width/2);
					line_max_width= (int)ceil((float)avg_line_width*1.1);
					line_min_width=(int)floor((float)avg_line_width*0.9);
				}

			}
			vector <int> vpoints;
			int c_up,c_down;
			cvSet(lined_image,cvScalar(255));

			if(avg_line_width<12){
				//Thin Lines
				for(int Staff_count_index=0;Staff_count_index<Staff_count;Staff_count_index++)
				{

					for(j_main=0;j_main<width;j_main++)
					{
						i_main=staff_positions[Staff_count_index];
						i=i_main;
						j=j_main;
						up_start=0;
						up_end=0;
						down_start=0;
						down_end=0;
						int diff=0;

						if( CV_IMAGE_ELEM(image,uchar,i_main,j) != 0  )
						{
							int found=0;
							for(diff=1;( diff<=searh_h) &&  (found==0);diff++)
							{
								if((i+diff<height-1) && (CV_IMAGE_ELEM(image,uchar,i+diff,j)==0))
								{
									found=1;
									i_main=	i+diff;
								}else if((i-diff>0) && (CV_IMAGE_ELEM(image,uchar,i-diff,j)==0))
								{
									found=1;
									i_main=	i-diff;
								}
							}
						}
						if( CV_IMAGE_ELEM(image,uchar,i_main,j) == 0  )
						{

							c_down=0;
							c_up=0;
							for(i=i_main;i<height && i<i_main+avg_staff_space &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i++);
							c_down=i;
							for(i=i_main;i>0 && i>i_main-avg_staff_space &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i--);
							c_up=i;

							if((c_down-c_up-1)<line_max_width && (c_down-c_up-1)>=line_min_width)
							{
								//vpoints.push_back(i_main);
								for(i=c_up;i<=c_down && i<height ;i++)
								{cvSetReal2D(lined_image,i,j,0); }
								//if(Variation>2.6 && Staff_count%5!=0)
								//{
								//	int found=0;
								//	for(diff=1;( diff<=searh_h) &&  (found==0);diff++)
								//	{
								//		if((c_down+diff<height-1) && (CV_IMAGE_ELEM(image,uchar,c_down+diff,j)==0))
								//		{
								//			found=1;
								//			i_main=	c_down+diff;
								//		}else if((c_up-diff>0) && (CV_IMAGE_ELEM(image,uchar,c_up-diff,j)==0))
								//		{
								//			found=1;
								//			i_main=	c_up-diff;
								//		}
								//	}
								//	if( CV_IMAGE_ELEM(image,uchar,i_main,j) == 0  )
								//	{

								//		c_down=0;
								//		c_up=0;
								//		for(i=i_main;i<i_main+avg_staff_space &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i++);
								//		c_down=i;
								//		for(i=i_main;i>i_main-avg_staff_space &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i--);
								//		c_up=i;

								//		if((c_down-c_up-1)<=line_max_width && (c_down-c_up-1)>=line_min_width)
								//		{
								//			//vpoints.push_back(i_main);
								//			for(i=c_up;i<=c_down;i++)
								//				cvSetReal2D(lined_image,i,j,0);
								//		}
								//	}
								//}
							}

						}
					}

				}
			}
			else
			{	//Thick Lines
				type='t';
				for(int Staff_count_index=0;Staff_count_index<Staff_count;Staff_count_index++)
				{
					line_top=0;
					line_bottom=0;
					diff=0;
					i_main=staff_positions[Staff_count_index];
					while(line_top==0)
					{
						b_count_t=0;
						if(diff<avg_line_width*2 && i_main-diff > 1)	diff++;
						else 	line_top= i_main-diff+1;
						for(j_main=0;j_main<width;j_main++)
							if(line_top==0 && CV_IMAGE_ELEM(image,uchar,i_main-diff,j_main)==0)			b_count_t++;
						if(b_count_t<width*0.7 && b_count_t!=0)	line_top= i_main-diff+1;  
					}
					diff=0;
					while(line_bottom==0)
					{
						b_count_b=0;
						if(diff<avg_line_width*2 && i_main+diff < height-1)	diff++;
						else 	 line_bottom= i_main+diff-1;
						for(j_main=0;j_main<width;j_main++)
							if(line_bottom==0 && CV_IMAGE_ELEM(image,uchar,i_main+diff,j_main)==0) 		b_count_b++;
						if(b_count_b<width*0.7 && b_count_b!=0)	line_bottom= i_main+diff-1;
					}

					up_pt.start=0;
					up_pt.end=0;
					dw_pt.start=0;
					dw_pt.end=0;

					for(j_main=0;j_main<width;j_main++)
					{
						i_main=staff_positions[Staff_count_index];
						i=i_main;
						j=j_main;
						diff=0;

						if( CV_IMAGE_ELEM(image,uchar,i_main,j) != 0  )
						{
							int found=0;
							for(diff=1;( diff<=searh_h) &&  (found==0);diff++)
							{
								if((i_main+diff<height-1) && (CV_IMAGE_ELEM(image,uchar,i+diff,j)==0))
								{
									found=1;
									i_main=	i+diff;
								}else if((i_main-diff>0) && (CV_IMAGE_ELEM(image,uchar,i-diff,j)==0))
								{
									found=1;
									i_main=	i-diff;
								}
							}
						}
						c_down=0;
						c_up=0;
						if( CV_IMAGE_ELEM(image,uchar,i_main,j) == 0  )
						{
							//i_main=staff_positions[Staff_count_index];
							c_down=0;
							c_up=0;
							for(i=i_main;i<i_main+avg_staff_space &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i++);
							c_down=i-1;
							for(i=i_main;i>i_main-avg_staff_space &&( CV_IMAGE_ELEM(image,uchar,i,j) == 0  );i--);
							c_up=i+1;
							if(c_up>=line_top)
							{
								for(i=c_up;i<=i_main;i++)
									cvSetReal2D(lined_image,i,j,0);
								if(up_pt.start!=0)
								{
									up_pt.end=j-1;
									up.push_back(up_pt);
									up_pt.start=0;
								}
							}else if(up_pt.start==0)
							{
								up_pt.start=j;
							}
							if(c_down<=line_bottom)
							{
								for(i=c_down;i>=i_main;i--)
									cvSetReal2D(lined_image,i,j,0);
								if(dw_pt.start!=0)
								{
									dw_pt.end=j-1;
									down.push_back(dw_pt);
									dw_pt.start=0;
								}
							}else if(dw_pt.start==0)
							{
								dw_pt.start=j;
							}
						}
					}
					check_and_fill(image,lined_image,staff_positions[Staff_count_index],up,down,line_top,line_bottom);

					up.clear();
					down.clear();
				}
			}
	}
	catch (...) {                       // catch an error
		cout << "Caught an exception --Horizontal Scan SR";
	}
#ifdef _DEBUG
	printf("\nCall HorizontalScan.");
	cvSaveImage(out_dir_path,lined_image);
#endif
	return lined_image;
}

