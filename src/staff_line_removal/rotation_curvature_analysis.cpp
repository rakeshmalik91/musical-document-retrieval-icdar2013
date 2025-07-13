#include "common.h"
#define PI 3.14159265

#define CHECK_BLACK_NOT_COVERED(lined_image,tempImage,i,j) CV_IMAGE_ELEM(lined_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(tempImage,uchar,i,j) != 0 

float rotation_curvature_analysis(const IplImage * image,IplImage * lined_image,int avg_line_width,int avg_staff_space,char &type,char file_name[100])
{
	char out_dir_path[100];
	IplImage *tempImage=cvCreateImage(cvGetSize(image),8,1);
	IplImage *tempImage2=cvCreateImage(cvGetSize(image),8,1);
	cvSet(tempImage,cvScalar(255));
	cvSet(tempImage2,cvScalar(255));
	strcpy(out_dir_path,"output\\rotation_curvature_analysis\\out_");
	strcat(out_dir_path,file_name);
	int i,j,k,count,i_main,j_main,tmp;
	int height,width,find_radius;
	height=image->height;
	width=image->width;
	find_radius=5;
	char type_rc='u';
	int angle_hist[361],found=0,size;
	memset(angle_hist,0,361*sizeof(int));
	vector <CvPoint> points;
	float slope=0,tempslope,tempslope2,slope_sum=0,angle_sum=0,angle_count=0,angle_avg,angle,ans=0;

	vector <float> angles;
	//cvNamedWindow("Disp Image",WIND_TYPE);
	int slope_count=0,angle_sign_change_count=0,changeflag=0;
	try{
		for(j_main=0;j_main<width-find_radius && type_rc!='c';j_main++)
		{
			for(i_main=find_radius;i_main<height-find_radius && type_rc!='c';i_main++)
			{
				i=i_main;
				j=j_main;
				if(CV_IMAGE_ELEM(lined_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(tempImage,uchar,i,j) != 0 )
				{
					changeflag=0;
					do
					{
						cvSetReal2D(tempImage,i,j,0);
						points.push_back(cvPoint(i,j));
						size=points.size()-1;
						 cvSetReal2D(tempImage2,i,j,200);
						if(size>599 && size%150==0)
						{
							tempslope=-1*(float)(points[size].x-points[size-150].x)/(float)(points[size].y-points[size-150].y);
							tempslope2=-1*(float)(points[size-150].x-points[size-300].x)/(float)(points[size-150].y-points[size-300].y);
							if(tempslope2 * tempslope < 0 && fabs(tempslope2- tempslope)>0.03)
							{
								angle_sign_change_count++;
								changeflag=1;
								//cout<<"\n tempslope2 - tempslope"<<fabs(tempslope2- tempslope);
							}
							if(angle_sign_change_count>5) type_rc='c';
						}
						if(size>24 && size%25==0)
						{
							tempslope=-1*(float)(points[size].x-points[size-25].x)/(float)(points[size].y-points[size-25].y);
							//cout<<"\nslope "<<tempslope;
							if(slope==0)slope=tempslope;
							else if(fabs((float)tempslope-slope)<.1)slope=-1*(float)(points[size].x-points[0].x)/(float)(points[size].y-points[0].y);
							else changeflag=1;
							////cvShowImage("Disp Image",tempImage);
							//cvWaitKey();
						}
						found=0;
						for(int dist=0;found==0 && dist<find_radius;dist++)
						{
							for(int v_diff=1;found==0 && v_diff<=find_radius;v_diff++)
							{
								if(j+v_diff<(width-2)  && (i+dist<height) && CV_IMAGE_ELEM(lined_image,uchar,i+dist,j+v_diff) == 0 )
								{
									i=i+dist;
									j=j+v_diff;
									found=1;
								}
								else if(dist!=0 && j+v_diff<(width-2) && (i-dist>0) && CV_IMAGE_ELEM(lined_image,uchar,i-dist,j+v_diff)==0)
								{
									i=i-dist;
									j=j+v_diff;
									found=1;
								}
							}
						}	

					}while(found==1 );
					size=points.size();
					if(size>width/4 && changeflag==0)
					{
						slope=-1*(float)(points[size-1].x-points[0].x)/(float)(points[size-1].y-points[0].y);
						angle= atan(slope) * 180 / PI;
						//cout<<"\nslope "<<slope<<" angle "<<angle<<" line size "<<size;
						//cvShowImage("Disp Image",tempImage);
						//cvWaitKey(0);
						//slope_sum+=	slope;
						//slope_count++;
						//tmp=((int)angle)%360;
						tmp=(int)angle;
						if(tmp<0)
							tmp=360+tmp;
						//cout<<" tmp="<<tmp;
						angle_hist[tmp]++;
						angles.push_back(angle);
						for(i=0;i<size;i++)
							cvSetReal2D(tempImage2,points[i].x,points[i].y,0);

					}
				//	cout<<" points.size() "<<points.size() ;
					points.clear();
				}


			}
		}

		// if(angle_sign_change_count>8) type_rc='c';
		ans= (float)angle_sign_change_count;
		type=type_rc;
		if(type_rc!='c')
		{
			type='r';
			int max_occ=0,max_occ_ang=0;
			for(i=0;i<360;i++)
				if(angle_hist[i]>max_occ)
				{
					max_occ=angle_hist[i];
					max_occ_ang=i;
				}
				if(max_occ_ang>180)max_occ_ang=max_occ_ang-360;
				for(i=0;i<angles.size();i++)
				{
					if(fabs(angles[i]-(float)max_occ_ang)<=1)
					{
						angle_sum+=angles[i];
						angle_count++;
					}
				}
				if(angle_count>0)
				{
					angle_avg=angle_sum/angle_count;
					ans=angle_avg;
					if(fabs(ans)>=38)
					{
						ans=0;
						type='c';
					}
				}
				else
				{
					ans=0;
					type='c';
				}
		}

	}catch( cv::Exception& e )
	{
		const char* err_msg = e.what();
		cout << "\nCaught an exception --rotation_curvature_analysis "<<err_msg<<"\n";
	}
	catch(...)
	{
		cout << "\nCaught an exception --rotation_curvature_analysis _DEFAULT\n";
	}
#ifdef _DEBUG
	//cvShowImage("Disp Image",tempImage);
	if( type_rc=='c')				
		cout<<"\n Curved !! angle_sign_change_count "<<angle_sign_change_count;
	else
		cout<<"\n Rotated !!  angle_sign_change_count "<<angle_sign_change_count<<"average angle "<<angle_avg;
	printf("\nCall rotation_curvature_analysis.");
	//cvWaitKey(0);
	cvSaveImage(out_dir_path,tempImage2);
#endif
	cvReleaseImage(&tempImage);
	cvReleaseImage(&tempImage2);
	return ans;
}
