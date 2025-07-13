#include "math.h"
#include "Statistics.cpp"

template <typename T>
class Computation{
	public:
		T		Max3(T x, T y, T z);
		T		Min3(T x, T y, T z);
		double	Diagonal( T height, T width);
		T		ArrayAvg( T *A, int N);
		T		ArrayMax( T *A, int N);
		T		ArrayMax( T **A, int R, int C);
		T		ArrayMin( T *A, int N);
		T		ArrayMode(T *A, int N);
		int		ArrayStdDev( int *A, int N, int mean);
		T		ArraySubtraction( T *A, T *B, int N);
		T		*CircularArrayShift( T *A,int N, int shift);
		T		*ArrayNormalize( T *A,int N);
		IplImage *BarChart( T *A, int N);
		void	Array2File( T *A, int N);
		int		*HistArray( T *A, int N, T low, T high, int Bin);
		T		Bin2Dec( T *A, int n);
		T		*ZeroPadding( T *A, int n, int R);
		double	Covariance( T *A, T *B, int N);
		double	Correlation( T *A, T *B, int N);
		double	**CovarianceMatrix( T **Mat, int R, int C);
		double	**CorrelationMatrix( T **Mat, int R, int C);
		double	**SumSqrCroProMatrix( T **Mat, int R, int C);
		int		GlobalThreshold( T *P, int Lmax);
		void 	PCA( T **Mat, int R, int C, double *Eval, double **Evector);
		void	powerA( T **A, T *lambda, T *X, int N, double EPSI, int *iter);
		void	Mat2File( T **mat, int ht, int wt);
		void	MatInitialize( T **matrix, int width, int height, T Val);
		void	Swap(T &x, T &y);
		void	SortMulti( T *array, int N, int c, int data, bool flag);
		string  ToStr( T N);
		double  Rad2Deg( T Rad);
		bool	CheckBetweenNumber( T A, T B, T X);
};

template <typename T>IplImage *BarChart( T *Array, int N){
	int win_wid_len;
	if( N < 20)	win_wid_len = 10;	
	else	win_wid_len = 2;
	
	int win_wid = N*win_wid_len+40;
	int win_hgt = 100+40;

	IplImage *win = cvCreateImage( cvSize(win_wid, win_hgt), 8, 3 );	cvZero( win );
	T maxV = ArrayMax(Array, N);
	double *data_list = new double[N];

	for(int i = 0; i < N; i++){
		if( Array[i] < 0)	data_list[i] = 0;
		else data_list[i] = (((double)Array[i])/maxV)*100;
	}

	cvLine( win, cvPoint(20, win_hgt-20), cvPoint(win_wid-20, win_hgt-20), CV_RGB(255,255,255), 1, 8, 0 );
	
	for( int i = 0; i < N; i++){
		cvRectangle( win, cvPoint(20+i*win_wid_len, win_hgt-20), cvPoint((int)20+(i+1)*win_wid_len-win_wid_len/2, win_hgt-20-data_list[i]),CV_RGB(0,255,0),-1);
	}
	proc->DisplayImg(win,"Data");
	return( win);
}

