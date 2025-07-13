#if !defined(AFX_COMMON_H__INCLUDED_ACTV_BIN)
#define AFX_COMMON_H__INCLUDED_ACTV_BIN
//#define _MT
//#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include <opencv2/opencv.hpp>
//#include <opencv2/legacy/legacy.hpp>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
//#include "dirent.h"
#include <dirent.h>
//#include <omp.h>
//#include <vld.h>

using namespace std;
using namespace cv;
 #pragma comment( lib, "opencv_core220d.lib")
#pragma comment( lib, "opencv_highgui220d.lib")
#pragma comment( lib, "opencv_imgproc220d.lib")
//#pragma comment( lib, "cvblobslib.lib")
#pragma comment( lib, "opencv_legacy220.lib" )
#pragma comment( lib, "opencv_video220.lib" )


/**** Some Settings ***/
#define WIND_TYPE CV_WINDOW_NORMAL //CV_WINDOW_AUTOSIZE CV_WINDOW_NORMAL 
//**** Some Settings ***/

void listFile(char*);

int binarize(const IplImage * gray_image,IplImage * bin_image,int &BlackBackground,char file_name[100]);
IplImage * process_bin_image(IplImage *,char file_name[100]);
IplImage * line_trace(IplImage *,IplImage *,int,int,char file_name[100]);
int cal_avg_staff_width(IplImage *);
IplImage * thicken_line(IplImage *,IplImage *,int,char file_name[100]);
int cal_avg_staff_space(IplImage *,int);
IplImage * remove_false_line(const IplImage *,int,int,int min_area,int radius,char file_name[100],int x_priority =0);
IplImage * intersect_image(IplImage *,IplImage *,char file_name[100]);
IplImage * process_image(IplImage *,IplImage *);
int compare_show(const IplImage *image1,IplImage *image2,char *file_name=0);
int VerticalThinning(IplImage *inp,IplImage *out,int &staff_height,int &staff_space,char file_name[100]);
IplImage * verticalScan(const IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100]);
IplImage * scanfor5(IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100]) ;
char type_detect(IplImage* image,int avg_line_width,int avg_staff_space,int positions[200],int &Staff_count,float &Variation,char file_name[100]);

IplImage * HorizontalScan(const IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,int staff_positions[200],int Staff_count,char &type,float Variation,char file_name[100]);
IplImage * remove_detected_lines(IplImage *,IplImage *,char file_name[100]);	

float rotation_curvature_analysis(const IplImage * image,IplImage * lined_image,int avg_line_width,int avg_staff_space,char &type,char file_name[100]) ;
IplImage * rotated_staff(const IplImage * image,IplImage *lined_image,int avg_line_width,int avg_staff_space,float angle,char file_name[100]);

IplImage * scanfor5curve(IplImage * image,IplImage * thinned_image,int avg_line_width,int avg_staff_space,char file_name[100]);

static int image_no;
static int BlackBackground;
static char names[6050][100],c;

  typedef struct start_end
{
	int start;
	int end;
}SE;
int check_and_fill(const IplImage * image,IplImage * lined_image,int x,vector <SE> up,vector <SE> down,int top,int bottom);

//static char file[256];
//static char* name_current;
//static CvMemStorage* 	storage = NULL;
//static char folder_input[200],folder_output[200];
//char subfolder[]= "video_set1";


// Record the execution time of some code, in milliseconds.
#define DECLARE_TIMING(s)  int64 timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0
#define START_TIMING(s)    timeStart_##s = cvGetTickCount()
#define STOP_TIMING(s) 	   timeDiff_##s = (double)(cvGetTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s) 	   (double)(timeDiff_##s / (cvGetTickFrequency()*1000.0))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s * cvGetTickFrequency()*1000.0) : 0)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0

#include "binarize.cpp"
#include "compare_show.cpp"
#include "HorizontalScan.cpp"
#include "HorizontalScan_check_and_fill.cpp"
#include "intersect_image.cpp"
#include "line_trace.cpp"
#include "remove_detected_lines.cpp"
#include "remove_false_line.cpp"
#include "rotated_staff.cpp"
#include "rotation_curvature_analysis.cpp"
#include "scanfor5.cpp"
#include "scanfor5curve.cpp"
#include "thicken_line.cpp"
#include "type_detect.cpp"
#include "verticalScan.cpp"
#include "VerticalThinning.cpp"

#endif // !defined(AFX_COMMON_H__INCLUDED_ACTV_BIN)
