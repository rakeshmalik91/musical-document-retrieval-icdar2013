#include "common.h"

IplImage * remove_detected_lines(IplImage *bin_image,IplImage *processed_staff_image,char file[100])
{
	IplImage *copy_bin_image=0;
	int i,j;

	int height,width;
	char out_dir_path[100];

	copy_bin_image=cvCloneImage(bin_image);

	height=bin_image->height;
	width=bin_image->width;

	strcpy(out_dir_path,"output\\intersect\\out_");
	strcat(out_dir_path,file);

	

	for(i=2;i<height-2;i++){
		for(j=2;j<width-2;j++){
			if(CV_IMAGE_ELEM(processed_staff_image,uchar,i,j)==0){
				
					cvSetReal2D(copy_bin_image,i,j,255);
			}
		}
	}

	
//	cvNamedWindow("remove_detected_lines Image",WIND_TYPE);

//	cvShowImage("Intersected Image",copy_bin_image);
#ifdef _DEBUG
	cvSaveImage(out_dir_path,copy_bin_image);
	//cvReleaseImage(&copy_bin_image);

	printf("\nCall remove_detected_lines");
#endif
	return copy_bin_image;
}