#include "common.h"
//#define _FORCE_SAVE

int compare_show(const IplImage *image1,IplImage *image2,char *file)
{


	int CompressionOptionParam[3];
	//CompressionOptionParam[0] = CV_IMWRITE_JPEG_QUALITY;	// CV_IMWRITE_PNG_COMPRESSION 	   CV_IMWRITE_JPEG_QUALITY
	//CompressionOptionParam[1] = 100;
	//CompressionOptionParam[2] = 0;
	char out_dir_path[100]="output\\cmp_out.png";
	int count=0;
	if(file!=NULL)
	{
		strcpy(out_dir_path,"output\\cmp\\out_");
		strcat(out_dir_path,file);
		//strcat(out_dir_path,".jpg");
	}
	int height=cvGetSize(image1).height;
	int width=cvGetSize(image1).width;
	int i,j,image1_val,image2_val;


#if defined(_DEBUG) || defined(_FORCE_SAVE) 
	IplImage *cmp_image1=cvCreateImage(cvGetSize(image1),8,1);
	IplImage *cmp_image2=cvCreateImage(cvGetSize(image1),8,1);
	IplImage *cmp_image3=cvCreateImage(cvGetSize(image1),8,1);
	IplImage *cmp_imageColor=cvCreateImage(cvGetSize(image1),8,3);
	cvSetZero(cmp_image3);
	cvSetZero(cmp_image2);
	cvSetZero(cmp_image1);
	cvAbsDiff(image1,image2,cmp_image3);
	cvXorS(cmp_image3,cvScalar(255),cmp_image3);
	cvCopy(cmp_image3,cmp_image1);
	cvCopy(cmp_image3,cmp_image2);
#endif
	//cvAbsDiff(image2,image1,cmp_image2);
//#pragma omp parallel for reduction(+:count)
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			image1_val=CV_IMAGE_ELEM(image1,uchar,i,j);		
			image2_val=CV_IMAGE_ELEM(image2,uchar,i,j);

			if(image1_val==255 && image2_val==0)
			{
#if defined(_DEBUG) || defined(_FORCE_SAVE) 
				cvSetReal2D(cmp_image1,i,j,255);
#endif
				count++;
			}
			else if(image1_val==0 && image2_val==255)
			{
#if defined(_DEBUG) || defined(_FORCE_SAVE)  
				cvSetReal2D(cmp_image2,i,j,255);
#endif
				count++;
			}
		}


#if defined(_DEBUG) || defined(_FORCE_SAVE) 
		cvMerge(cmp_image3,cmp_image1,cmp_image2,NULL,cmp_imageColor);
		//	cvNamedWindow("Compare Image",WIND_TYPE);
		//	cvShowImage("Compare Image",cmp_imageColor);
		cvSaveImage(out_dir_path,cmp_imageColor,CompressionOptionParam);
#else


#endif
#if defined(_DEBUG) || defined(_FORCE_SAVE) 
		cvReleaseImage(&cmp_image1);
		cvReleaseImage(&cmp_image2);
		cvReleaseImage(&cmp_image3);
		cvReleaseImage(&cmp_imageColor);
#endif
		return count;
}