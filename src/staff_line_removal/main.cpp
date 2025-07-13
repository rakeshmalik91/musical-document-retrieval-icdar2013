#include "common.h"
#define AUTO_BROWSE 1
#define SHOW_GT 0
#define COMPARE_GT 0
//Any one of the following
#define CVCMUSCIMA_SR 0
//#define CONTEST_SR 0
//#define SMALL_TEST_SR 0
#define _FORCE_SAVE

int main(int argc, char* argv[])
{
	if(argc<3) {
		cerr<<"Arguments missing"<<endl
			<<"Usage : main <input_directory> <output_directory>"<<endl;
		exit(0);
	}
	char input_dir_path[100],input_filename[100],out_binary_dir_path[100],out_lined_dir_path[100],out_final_dir_path[100],input_filename_save[100];
	char file[200],gt_dir_path[100],out_temp_dir_path[100],out_file_name[50];
	int avg_staff_space,avg_line_width,total_black;
	char Staff_line_type='u';
	int staff_positions[200],Staff_count;
	float p_min=100,p_max=0,p_sum=0,p_count=0,rotation_angle=0,Variation=0;
	//FILE *fp;

	DECLARE_TIMING(totalTimer);
	START_TIMING(totalTimer);
	DECLARE_TIMING(myTimer);
	//cvSetNumThreads(4);
	try 
	{
#if CVCMUSCIMA_SR == 1
		char *CVCMUSCIMA_SR_types[]= {"ideal","interrupted","curvature","rotated","kanungo","staffline-thickness-variation-v1",
			"staffline-thickness-variation-v2","staffline-y-variation-v1","staffline-y-variation-v2","thickness-ratio",
			"typeset-emulation","whitespeckles",0};
		int  CVCMUSCIMA_SR_grp_count=50;
		for(int type_index=0;CVCMUSCIMA_SR_types[type_index]!=0 && type_index <=12 ;type_index++)
		{	  // if(type_index==2)type_index+=2;
			//strcpy(out_file_name,"out_");
			//strcat(out_file_name,CVCMUSCIMA_SR_types[type_index]);
			//strcat(out_file_name,".csv");
			printf("\n type %d %s",type_index,CVCMUSCIMA_SR_types[type_index])  ;
			//fp=fopen(out_file_name,"a+");
			//fprintf(fp,"\nInput folder path CVCMUSCIMA_SR\\%s",CVCMUSCIMA_SR_types[type_index]);
			//fclose (fp);
			p_min=100;p_max=0;p_sum=0;p_count=0;
			for(int CVCMUSCIMA_SR_grp_index=1;CVCMUSCIMA_SR_grp_index<=CVCMUSCIMA_SR_grp_count/* && CVCMUSCIMA_SR_grp_index<=1 */;CVCMUSCIMA_SR_grp_index++)
			{
				sprintf(input_dir_path,"CVCMUSCIMA_SR\\%s\\w-%02d\\image\\",CVCMUSCIMA_SR_types[type_index],CVCMUSCIMA_SR_grp_index);
				sprintf(gt_dir_path,"CVCMUSCIMA_SR\\%s\\w-%02d\\symbol\\",CVCMUSCIMA_SR_types[type_index],CVCMUSCIMA_SR_grp_index);
#else
		sprintf(input_dir_path, "%s\\", argv[1]);
		strcpy(gt_dir_path,"ground_truth\\gt-");		

		//strcpy(out_file_name,"out.csv");
#endif

#if AUTO_BROWSE == 1
#if CVCMUSCIMA_SR == 1
#else
		//fp=fopen(out_file_name,"a+");
		//fprintf(fp,"\nInput folder path %s",input_dir_path);
		//fclose (fp);
		p_min=100;p_max=0;p_sum=0;p_count=0;
#endif

		listFile(input_dir_path);
		int image_count=0;
		for(image_no = 0; names[image_no][0] !='\0' /*&&image_no<500*/; image_no++ )image_count++  ;
		//#pragma omp parallel for private(input_filename,input_filename_save,Staff_line_type,avg_line_width,avg_staff_space,staff_positions,Staff_count) 	  reduction(+: p_sum ,p_count) 	shared ( p_min,p_max)
		for(image_no = 0; image_no<image_count; image_no++ )
		{
			IplImage *gray_image=0,*bin_image=0;
			IplImage *lined_image=0,*lined_image1=0,*process_line_image=0,*processed_staff_image=0;
			IplImage *gray_diff_image=0,*nostaff_image=0,*final_image=0,*setof5lines=0,*gt_image=0;

			strcpy(input_filename,names[image_no]);
#else
		printf("\n\n Enter file name with extension : ");
		scanf("%[^\n]",input_filename);
#endif

		//fp=fopen(out_file_name,"a+");
		sprintf(file, "%s%s", input_dir_path,input_filename);
#if CVCMUSCIMA_SR ==1
		printf("\n\n %d Processing... %s%s",(CVCMUSCIMA_SR_grp_index-1)*10+image_no, input_dir_path,input_filename);
#else
		printf("\n\n %d Processing... %s%s",image_no, input_dir_path,input_filename);
#endif
		/*******PATH Settings*****/
		strcpy(out_binary_dir_path,"output\\binary\\out-");
		strcpy(out_lined_dir_path,"output\\lined\\out-");
		sprintf(out_final_dir_path,"%s\\", argv[2]);
		strcpy(out_temp_dir_path,"output\\temp\\out-");
#if CVCMUSCIMA_SR ==1

		sprintf(input_filename_save,"%s-w-%02d-%s",CVCMUSCIMA_SR_types[type_index],CVCMUSCIMA_SR_grp_index,input_filename);
#else
		strcpy(input_filename_save,input_filename);
#endif
		strcat(out_binary_dir_path,input_filename_save);
		strcat(out_temp_dir_path,input_filename_save);
		strcat(out_lined_dir_path,input_filename_save);
		strcat(out_final_dir_path,input_filename_save);

		/*******Image init*****/
		gray_image=cvLoadImage(file,CV_LOAD_IMAGE_GRAYSCALE);
		if( !gray_image )
		{
			printf("\nCouldn't load %s\n",file);
			//			continue;
		}
		int height=cvGetSize(gray_image).height;
		int width=cvGetSize(gray_image).width;
		bin_image=cvCreateImage(cvGetSize(gray_image),8,1);
		//	IplImage *disp_image=cvCreateImage(cvGetSize(gray_image),8,3);

		/*******=====Algo Starts Here=====*****/

		START_TIMING(myTimer);
		/******Preprocessing******/

		total_black=binarize(gray_image,bin_image,BlackBackground,input_filename_save);
		//cvMerge(bin_image,bin_image,bin_image,NULL,disp_image);

		/***  Vertical thinning and find staff height and space my mode ***/
		IplImage* thinned_image=cvCreateImage(cvGetSize(gray_image),8,1);	
		VerticalThinning(bin_image,thinned_image,avg_line_width,avg_staff_space,input_filename_save);
		Staff_line_type=type_detect(thinned_image,avg_line_width,avg_staff_space,staff_positions,Staff_count,Variation,input_filename_save);
		//cout<<"\nStaff_count"<<Staff_count   ;
		
		if(Staff_line_type=='s' ||Staff_line_type=='n' ||Staff_line_type=='y' && false)
		{
			process_line_image=HorizontalScan(bin_image,thinned_image,avg_line_width,avg_staff_space,staff_positions,Staff_count,Staff_line_type,Variation,input_filename_save);
			if(Staff_line_type!='n' && Staff_line_type!='t' )
			{
				processed_staff_image=remove_false_line(process_line_image,avg_line_width,avg_staff_space,4,5,input_filename_save);
				nostaff_image=remove_detected_lines(bin_image,processed_staff_image,input_filename_save);
			}
			else
				nostaff_image=remove_detected_lines(bin_image,process_line_image,input_filename_save);
		}
		else
		{
			lined_image=verticalScan(bin_image,thinned_image,avg_line_width,avg_staff_space,input_filename_save);
			rotation_angle=rotation_curvature_analysis(bin_image,lined_image,avg_line_width,avg_staff_space,Staff_line_type,input_filename_save);


			if(Staff_line_type=='r' )
			{
				process_line_image=rotated_staff(bin_image,lined_image,avg_line_width,avg_staff_space,rotation_angle,input_filename_save);

			}else
			{
				//setof5lines=scanfor5(bin_image,lined_image,avg_line_width,avg_staff_space,input_filename_save); 
				setof5lines=scanfor5curve(bin_image,lined_image,avg_line_width,avg_staff_space,input_filename_save);
				// find any horizontal stretch greater than width/60 
				//lined_image=line_trace(thinned_image,lined_image,avg_line_width,avg_staff_space,input_filename_save);
				//-----If height of black pixel in bin_image for any black pixel at line_trace <2*line_width, set all found vertical pixels black, else set current pixel white 
				process_line_image=thicken_line(setof5lines,bin_image,avg_line_width,input_filename_save);
			
				//-----(if staff hieght is abrupt and space above/below/both side are less than half of avg, remove edge black pixel of that side or both) 
				//-----OR 
				//-----(if space both sides are less than 50% of average space, set both  edge of black pixels as white)------------
				//processed_staff_image=remove_false_line(process_line_image,avg_line_width,avg_staff_space,input_filename_save);
				//----- No-staff image is created with staff removed leaving out intersection -----
			//	nostaff_image=intersect_image(bin_image,processed_staff_image,input_filename_save);
			}
				processed_staff_image=remove_false_line(process_line_image,avg_line_width,avg_staff_space,7,3,input_filename_save,4);
					cvErode(processed_staff_image, processed_staff_image, NULL ,1);
					cvDilate(processed_staff_image, processed_staff_image,NULL,1);
				nostaff_image=remove_detected_lines(bin_image,processed_staff_image,input_filename_save);
		
		}
		
		STOP_TIMING(myTimer);
		printf("\nExecution time: %f ms.", GET_TIMING(myTimer) );
#pragma omp critical (fprintf)
		{
			//fprintf(fp,"\n%s,%5.0f",input_filename,GET_TIMING(myTimer));
		}

#ifdef _DEBUG
		cvNamedWindow("Image",WIND_TYPE);
		//cvNamedWindow("Binarized Image",WIND_TYPE);
		//cvNamedWindow("Disp Image",WIND_TYPE);
		cvNamedWindow("1thinned_image",WIND_TYPE);
		//cvNamedWindow("2Lined Image",WIND_TYPE);
		cvNamedWindow("3process_line_image",WIND_TYPE);
		//cvNamedWindow("4processed_staff_image",WIND_TYPE);
		//cvNamedWindow("5nostaff_image",WIND_TYPE);

		cvShowImage("Image",gray_image);
		//cvShowImage("Binarized Image",bin_image);
		//cvShowImage("Disp Image",disp_image);
		cvShowImage("1thinned_image",thinned_image);
		//cvShowImage("2Lined Image",lined_image);
		cvShowImage("3process_line_image",process_line_image);
		//cvShowImage("4processed_staff_image",processed_staff_image);
		//cvShowImage("5nostaff_image",nostaff_image);

		//cvSaveImage(out_lined_dir_path,lined_image);
		//cvSaveImage(out_final_dir_path,nostaff_image);
///c = cvWaitKey(0);
		printf("\n Run Completes.");

#endif							 
#ifdef _FORCE_SAVE
		//bool imwrite(const string& filename, const Mat& img, const vector<int>& params=vector<int>())
		//vector <int> param;
		//param.push_back(CV_IMWRITE_PNG_COMPRESSION);
		//param.push_back(9);
		//param.push_back(0);
		IplImage * saveImage=cvCreateImage(cvGetSize(gray_image),8,1);
		cvXorS(nostaff_image,cvScalar(255),saveImage);
		//Mat cvSaveMat=saveImage;
		int CompressionOptionParam[3];
		CompressionOptionParam[0] = CV_IMWRITE_PNG_COMPRESSION;	// CV_IMWRITE_PNG_COMPRESSION 	   CV_IMWRITE_JPEG_QUALITY
		CompressionOptionParam[1] = 9;
		CompressionOptionParam[2] = 0;
		cvSaveImage(out_final_dir_path,saveImage,CompressionOptionParam);

		//imwrite(out_final_dir_path,cvSaveMat, param) ;
		cvReleaseImage(&saveImage);
		//cvSaveMat.deallocate();
		//cvSaveImage(out_final_dir_path,nostaff_image);
#endif	
#if SHOW_GT == 1
		sprintf(file, "%s%s", gt_dir_path,input_filename);
		gt_image=cvLoadImage(file,CV_LOAD_IMAGE_GRAYSCALE);
		if( !gt_image )
		{
			printf("\nCouldn't find/load Ground Truth %s\n",file);
		}else
		{

			if(BlackBackground==1)
				cvXorS(gt_image,cvScalar(255),gt_image); // Invert
#ifdef _DEBUG
			//	cvNamedWindow("GT Image",WIND_TYPE);
			//	cvShowImage("GT Image",gt_image);
#endif

#if COMPARE_GT == 1

			final_image=cvCloneImage(nostaff_image);// until image is processed
			int changed_pixel_count=compare_show(final_image,gt_image,input_filename_save);
			float error_rate=(float)changed_pixel_count/total_black;
			float p_correct=(1-error_rate)*100;

			cout<<"\ndiff/total black pixels="<<error_rate<<" correct percent= "<<p_correct;
#pragma omp critical (fprintf)
			{
				//fprintf(fp,",%f",error_rate);
				//fprintf(fp,",%f",p_correct);
				//fprintf(fp,",%c,%d,%d",Staff_line_type,avg_line_width,avg_staff_space);
				//if(Staff_line_type=='r' || Staff_line_type=='c')fprintf(fp,",%6.3f",rotation_angle);
				//fprintf(fp,",%f",Variation);
			}
			if(p_correct<p_min)	p_min=p_correct;
			if(p_correct>p_max) p_max=p_correct;
			p_sum+=p_correct;
			p_count++;
#endif
		}

#endif
		//c = cvWaitKey(0);
		cvReleaseImage(&gray_image);
		cvReleaseImage(&bin_image);
		cvReleaseImage(&setof5lines);
		cvReleaseImage(&thinned_image);
		cvReleaseImage(&lined_image);
		cvReleaseImage(&process_line_image);
		cvReleaseImage(&processed_staff_image);
		cvReleaseImage(&nostaff_image);	
		cvReleaseImage(&gt_image);
		cvReleaseImage(&final_image);

		//fclose (fp);


#if AUTO_BROWSE == 1
#ifndef _OPENMP
		if( (char)c == 'r' )
			image_no--;
		if( (char)c == 27 )
			break;
#endif
	}
#if CVCMUSCIMA_SR ==1
#else
	printf("\nCorrectness Average %5.2f%% Max %5.2f%% Min %5.2f%%",p_sum/p_count,p_max,p_min);
	//fp=fopen(out_file_name,"a+");
	//fprintf(fp,"\n\nCorrectness Average %5.2f%% Max %5.2f%% Min %5.2f%%",p_sum/p_count,p_max,p_min);
	//fclose (fp);
#endif
#endif
	//	cvReleaseImage(&image1);
	//	cvReleaseImage(&gray_image);
	cvDestroyAllWindows();
#if CVCMUSCIMA_SR ==1
}
printf("\nCorrectness Average %9.4f%% Max %5.2f%% Min %5.2f%%",p_sum/p_count,p_max,p_min);
//fp=fopen(out_file_name,"a+");
//fprintf(fp,"\n\nCorrectness Average %9.4f%% Max %5.2f%% Min %5.2f%%",p_sum/p_count,p_max,p_min);
//fclose (fp);
  }
#endif
	}
	catch (...) {                       // catch an error
		cout << "Caught an exception --MAIN SR";
	}



	STOP_TIMING(totalTimer);
	float ms,ms_without_min;
	int minute;				 
	ms=	(float)GET_TIMING(totalTimer)/1000;
	minute=(int)GET_TIMING(totalTimer)/60000;
	ms_without_min=ms-60*minute;
	printf("\nTotal Time %dmin %9.3fsec",minute,ms_without_min);
	printf("\nAverage Time %6.2fms",GET_AVERAGE_TIMING(myTimer));
	//fp=fopen(out_file_name,"a+");

	//fprintf(fp,"\nTotal Time %dmin %9.3fsec ",minute,ms_without_min);
	//fprintf(fp,"\nAverage Time %6.2fms",GET_AVERAGE_TIMING(myTimer));
	//fprintf(fp,"\n***************************\n");
	//fclose (fp);
	//getch();
	cout<<endl;
	return 0;
}

void listFile(char *path){
	DIR *pDIR;
	struct dirent *entry;
	//	char *f_name, *f_ext;
	int i=0;
	if( pDIR=opendir(path) ){
		while(entry = readdir(pDIR)){
			if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&strcmp(strrchr(entry->d_name,'.'),".db")!= 0 &&strcmp(strrchr(entry->d_name,'.'),entry->d_name)!= 0 /*&& entry->d_type==32768*/)
			{
				//cout << entry->d_name<<"";
				//cout << " ext"<<strrchr(entry->d_name,'.') << "\n";
				strcpy(names[i++],entry->d_name);
			}
		}
		names[i][0]='\0';
		closedir(pDIR);
	}
}
