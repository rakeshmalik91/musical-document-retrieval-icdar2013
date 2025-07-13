#include "common.h"

int binarize(const IplImage * gray_image,IplImage * bin_image,int &BlackBackground,char file_name[100])
{
	IplImage *bin1_image=0;
	int hist_count[256]={0};
	int i,j;
	int height,width;
	int xifi=0,fi=0,mean=0;
	int max_lval=0,max_rval=0,min_val=50000;

	char out_dir_path[100];
	strcpy(out_dir_path,"output\\binary\\out_");
	strcat(out_dir_path,file_name);

	height=cvGetSize(gray_image).height;
	width=cvGetSize(gray_image).width;
	int black_count=0;
	//bin1_image=cvCreateImage(cvGetSize(gray_image),8,1);

	//Histogram
	//memset(hist_count,0,256);

	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			hist_count[CV_IMAGE_ELEM(gray_image,uchar,i,j)]++;

	if (hist_count[0]+hist_count[255]==height*width)
	{
		//Binary Image
		if(hist_count[0]>hist_count[255])//Black Background
		{
			cvXorS(gray_image,cvScalar(255),bin_image); // Copy Invert
			BlackBackground=1;
			black_count=hist_count[255];
		}else
		{
			cvCopy(gray_image,bin_image); //Normal Copy
			black_count=hist_count[0];
		}
	}
	else
	{
		for(i=0;i<256;i++)
		{
			xifi+=i*hist_count[i];
			fi+=hist_count[i];
		}

		mean=xifi/fi;
		mean=mean-60;
	
		#ifdef _DEBUG 
			printf("\n\t Mean=%d",mean);
		#endif
		cvThreshold(gray_image,bin_image,mean,256,CV_THRESH_BINARY);
		black_count=height*width-(int)cvSum(bin_image).val[0]/255;

	}
#ifdef _DEBUG
	printf("\nCall Binary -- Black=%d",black_count);
	cvSaveImage(out_dir_path,bin_image);

#endif
	return black_count;
}