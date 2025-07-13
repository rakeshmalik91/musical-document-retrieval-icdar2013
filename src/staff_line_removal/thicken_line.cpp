#include "common.h"

IplImage * thicken_line(IplImage *lined_image,IplImage *bin_image,int line_width,char file_name[100])
{
	IplImage *processed_lined_image=0;
	int i,j=0,k,l,height,width;
	char out_dir_path[100];

	//processed_lined_image=cvCreateImage(cvGetSize(lined_image),8,1);

	height=lined_image->height;
	width=lined_image->width;

	strcpy(out_dir_path,"output\\process_line\\out_");
	strcat(out_dir_path,file_name);

	processed_lined_image=cvCloneImage(lined_image);


	for(i=2;i<height-2;i++)
	{
		for(j=2;j<width-2;j++)
		{
			if(CV_IMAGE_ELEM(lined_image,uchar,i,j) == 0)
			{
				k=i;
				l=i;
				while(CV_IMAGE_ELEM(bin_image,uchar,(k-1),j)==0 && k>3)
					k--;
				while(CV_IMAGE_ELEM(bin_image,uchar,(l+1),j)==0 && l<(height-3))
					l++;
				if((l-k)<=ceil(line_width*1.0)){
					while(k!=i){
						if(CV_IMAGE_ELEM(lined_image,uchar,k,j)!=0)
							cvSetReal2D(processed_lined_image,k,j,0);
						k++;
					}
					while(l!=i){
						if(CV_IMAGE_ELEM(lined_image,uchar,l,j)!=0)
							cvSetReal2D(processed_lined_image,l,j,0);
						l--;
					}
				}
				else
					cvSetReal2D(processed_lined_image,i,j,255);
					//cvSetReal2D(processed_lined_image,i,j,CV_IMAGE_ELEM(bin_image,uchar,i,j));
			}
		}
	}

#ifdef _DEBUG
	cvSaveImage(out_dir_path,processed_lined_image);
	printf("\nCall Process_line_image.");
#endif
	return processed_lined_image;
}