#include "common.h"

IplImage * intersect_image(IplImage *bin_image,IplImage *processed_staff_image,char file[100])
{
	IplImage *copy_bin_image=0;
	int i,j,k1,k2,l1,l2;
	//int i11,i12,i21,i22,j11,j12,j21,j22;
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
				k1=i;
				l1=i;
				k2=i;
				l2=i;
				while(CV_IMAGE_ELEM(processed_staff_image,uchar,(k1-1),j)==0 && (k1-1)>2)
					k1--;
				while(CV_IMAGE_ELEM(processed_staff_image,uchar,(l1+1),j)==0 && (l1+1)<height-2)
					l1++;
				while(CV_IMAGE_ELEM(bin_image,uchar,(k2-1),j)==0 && (k2-1)>2)
					k2--;
				while(CV_IMAGE_ELEM(bin_image,uchar,(l2+1),j)==0 && (l2+1)<height-2)
					l2++;
				if((l2-k2)>(l1-k1)+3)//TODO: replace 3 by percentage
					cvSetReal2D(copy_bin_image,i,j,0);
				else
					cvSetReal2D(copy_bin_image,i,j,255);
			}
		}
	}



//	cvNamedWindow("Intersected Image",WIND_TYPE);

//	cvShowImage("Intersected Image",copy_bin_image);
#ifdef _DEBUG
	cvSaveImage(out_dir_path,copy_bin_image);
	//cvReleaseImage(&copy_bin_image);

	printf("\nCall Intersect_Image");
#endif
	return copy_bin_image;
}