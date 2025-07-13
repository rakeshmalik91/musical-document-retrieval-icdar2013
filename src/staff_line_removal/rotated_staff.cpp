#include "common.h"
#define PI 3.14159265
IplImage * rotated_staff(const IplImage * image,IplImage *lined_image,int avg_line_width,int avg_staff_space,float angle,char file_name[100])
{
	char out_dir_path[100];
	IplImage *tempImage=cvCreateImage(cvGetSize(image),8,1);
	cvSet(tempImage,cvScalar(255));
	strcpy(out_dir_path,"output\\rotated_staff\\out_");
	strcat(out_dir_path,file_name);
	int i,j,k,count,i_main,j_main,tmp,height,width,size,find_radius=2,found,x,y,c_down,c_up,skip_flag;
	height=lined_image->height;
	width=lined_image->width;
	vector <CvPoint> points,points_after_disconect,points_skipped;
	vector <int> done;
	float slope,tempslope,tempslope1,tempslope2;
	slope=tan(angle *PI/180);
	int line_max_width=(int)ceil((float)avg_line_width*2); 
	int line_min_width=(int)floor((float)avg_line_width*0.3);
	int gap_min=(int)avg_staff_space*0.9-avg_line_width*0.25;
	int gap_max=(int)avg_staff_space*1.1+avg_line_width*1.25;


	try{
		for(j_main=0;j_main<width-find_radius ;j_main++)
		{
			for(i_main=find_radius;i_main<height-find_radius;i_main++)
			{
				i=i_main;
				j=j_main;
				if(CV_IMAGE_ELEM(lined_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(tempImage,uchar,i,j) != 0)
				{
					do
					{
						points.push_back(cvPoint(i,j));
						found=0;
						y=j++;
						x=points[0].x-((float)(y-points[0].y)*slope);
						size=points.size();
						//tempslope=-1*(float)(points[size-1].x-points[0].x)/(float)(points[size-1].y-points[0].y);
						if(y<(width-2)  && (x<height) && y>2 && x >2 )	
						{
							if( CV_IMAGE_ELEM(image,uchar,x,y) == 0 )
							{
								i=x;
								found=1;
							}
							else if( CV_IMAGE_ELEM(image,uchar,x-1,y) == 0 )
							{
								i=x-1;
								found=1;
							}
							else if( CV_IMAGE_ELEM(image,uchar,x+1,y) == 0 )
							{
								i=x+1;
								found=1;
							}else if( i>x-3 && i<x+3 && CV_IMAGE_ELEM(image,uchar,i,y) == 0  )
							{
								i=i;
								found=1;
							}else if(i-1>x-4 && i-1<x+4 &&  CV_IMAGE_ELEM(image,uchar,i-1,y) == 0)
							{
								i=i-1;
								found=1;
							}
							else if(i+1>x-4 && i+1<x+4 &&  CV_IMAGE_ELEM(image,uchar,i+1,y) == 0 )
							{
								i=i+1;
								found=1;
							}

						}	

					}while(found==1 );
					size=points.size();
					int lasty=0,lastx=0,flag_done=0;
					
					if(size>width/5)
					{
						//cout<<" points.size() "<<points.size() ;
					
						int startingPt=points[int(size)/5].x-((float)(2-points[int(size)/5].y)*slope);	  //x for y=2
						int startingPt2=points[int(size)*2/5].x-((float)(2-points[int(size)*2/5].y)*slope);	  //x for y=2
						int startingPt3=points[int(size)*3/5].x-((float)(2-points[int(size)*3/5].y)*slope);	  //x for y=2
						int startingPt4=points[int(size)*4/5].x-((float)(2-points[int(size)*4/5].y)*slope);	  //x for y=2
						startingPt=(startingPt+startingPt2+startingPt3+startingPt4)/4;
						for(k=0;k<done.size();k++)
							if(done[k]==startingPt) flag_done=1;
						if(flag_done==0)
						{

							done.push_back(startingPt);
							c_down=0;
							c_up=0;
							for(k=2;k<=width-2;k++)
							{
								y=k;
								x=startingPt-((float)(k-2)*slope);
								if(x>=height)continue;
								//cvSetReal2D(tempImage,x,y,180);
								 if(y==341 && x<970)
									y=k;
								found=0;
								if(x<height && x>0 && lasty!=y-i && k<points[10].y)
								{
									if(CV_IMAGE_ELEM(image,uchar,x,y) == 0 )
									{ 
										found=1;
									}
									else for(int p=0; p<16 && found==0;p++)
									{
										if(x-p>0 && CV_IMAGE_ELEM(image,uchar,x-p,y) ==0 )
										{
											found=1;
											x=x-p;
										}
										if(x+p< height && CV_IMAGE_ELEM(image,uchar,x+p,y) ==0 )
										{
											found=1;
											x=x+p;
										}
									}
								}
								else
								{
									for(int p=0; p<10 && found==0;p++)
									{
										if((x-p)<height && (x-p)>0 && (x-p)>=(lastx-2*line_max_width) && CV_IMAGE_ELEM(image,uchar,x-p,y) ==0 )
										{
											found=1;
											x=x-p;
										}					    
										if((x+p)>=0 && (x+p)<height && (x+p)<=(lastx+2*line_max_width) && CV_IMAGE_ELEM(image,uchar,x+p,y) ==0 )
										{
											found=1;
											x=x+p;
										}
									}

								}

								if(found==1)
								{
									//cvSetReal2D(tempImage,x,y,0);
									c_down=0;
									c_up=0;
									lasty=y;
									lastx=x;
									for(i=x;i<height && i>0 &&i<=x+2*line_max_width &&( CV_IMAGE_ELEM(image,uchar,i,y) == 0  );i++);
									c_down=i;
									for(i=x;i>0 && i<height && i>=x-2*line_max_width &&( CV_IMAGE_ELEM(image,uchar,i,y) == 0  );i--);
									c_up=i;

									if((c_down-c_up-1)>line_min_width ) //((c_down-points[k].x)<=avg_line_width/2 && (points[k].x-c_up)<=avg_line_width/2) )
									{  
										if((c_down-c_up-1)<=line_max_width)
										{	//cvSetReal2D(tempImage,x,y,0);
											for(i=c_up+1;i<c_down;i++)
												cvSetReal2D(tempImage,i,y,0);


										}
									/*	else if((c_down-x-1)<line_max_width)
											cvSetReal2D(tempImage,x,y,100);
										else if((x-c_up-1)<line_max_width)	 
											cvSetReal2D(tempImage,x,y,100);*/
											
										/*	for(i=c_up+1;i<c_down;i++)
										cvSetReal2D(tempImage,i,y,0);*/
									}
								}
							}
							//for(k=0;k<size;k++){
							//	cvSetReal2D(tempImage,points[k].x,points[k].y,0);
							/*c_down=0;
							c_up=0;

							for(i=points[k].x;i<=points[k].x+4*line_max_width &&( CV_IMAGE_ELEM(image,uchar,i,points[k].y) == 0  );i++);
							c_down=i;
							for(i=points[k].x;i>i_main-4*line_max_width &&( CV_IMAGE_ELEM(image,uchar,i,points[k].y) == 0  );i--);
							c_up=i;

							if((c_down-c_up-1)>line_min_width && (c_down-c_up-1)<line_max_width) //((c_down-points[k].x)<=avg_line_width/2 && (points[k].x-c_up)<=avg_line_width/2) )
							{
							//vpoints.push_back(i_main);
							for(i=c_up+1;i<c_down;i++)
							cvSetReal2D(tempImage,i,points[k].y,0);
							} */
							//}
							//cvSetReal2D(tempImage,points[i].x,points[i].y,0);
						}

					}

					points.clear();
				}


			}
		}
	}catch( cv::Exception& e )
	{
		const char* err_msg = e.what();
		cout << "\nCaught an exception --rotated_staff "<<err_msg<<"\n";
	}
	catch(...)
	{
		cout << "\nCaught an exception --rotated_staff _DEFAULT\n";
	}
#ifdef _DEBUG

	printf("\nCall rotated_staff");
	cvSaveImage(out_dir_path,tempImage);

#endif
	return tempImage;
}