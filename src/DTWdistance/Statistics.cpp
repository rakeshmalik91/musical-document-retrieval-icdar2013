#include "stdafx.h"
#include "string"
using namespace std;

template <typename T>bool CheckBetweenNumber( T A, T B, T X){
	if( A >= B && X >= B && X <= A) return(TRUE);
	else if(A <= B && X >= A && X <= B) return(TRUE);
	else return(FALSE);
}

template <typename T>double Rad2Deg( T Rad){
	double Deg = Rad*180/PI;
	return( Deg);
}

template <typename T>string  ToStr( T N){
	ostringstream s;	s <<N;
	string str = s.str();
	return(str);
}

template <typename T>void MatInitialize( T **matrix, int width, int height, T Val){
	for( int i = 0; i < height; i++){
		for( int j = 0; j < width; j++){
			matrix[i][j] = Val;
		}
	}
}

template <typename T>double *Correlation( T *A, T *B, int N){
	double xy = 0;
	for( int i = 0; i < N; i++)	xy += A[i]*B[i];
	xy = xy/N;
	return( xy);
}

template <typename T>double	**CovarianceMatrix( T **Mat, int R, int C){
	double **Res = 	(double**)malloc(C*sizeof(double*));
	for(int i= 0; i<C;i++)		Res[i] = new double[C];

	double *mean = new double[C];
	for( int j = 0; j < C; j++){
		mean[j] = 0;
		for( int i = 0; i < R; i++)	mean[j] += Mat[i][j];
		mean[j] /= (double)R;
	}

	double **Mid = 	(double**)malloc(R*sizeof(double*));
	for(int i= 0; i<R;i++)		Mid[i] = new double[C];

	for( int i = 0; i < R; i++){
		for( int j = 0; j < C;	j++){
			Mid[i][j] = Mat[i][j] - mean[j];
		}
	}	

	for(int j1 = 0; j1 < C; j1++){
		for(int j2 = j1; j2 < C; j2++){
			Res[j1][j2] = 0;
			for(int i = 0; i < R; i++){
				Res[j1][j2] += Mid[i][j1]*Mid[i][j2];
            }
			Res[j1][j2] /= R-1;
			Res[j2][j1] = Res[j1][j2];
        }
    }
	free(Mid);
	return( Res);
}

template <typename T>double	**CorrelationMatrix( T **Mat, int R, int C){
	double **Res = 	(double**)malloc(C*sizeof(double*));
	for(int i= 0; i<C;i++)		Res[i] = new double[C];

	double eps = 0.0005;
	double *mean = new double[C];
	double *stddev = new double[C];

    for(int j = 0; j < C; j++){
		mean[j] = 0;
		for( int i = 0; i < R; i++)	mean[j] += Mat[i][j];
	    mean[j] /= (float)R;
    }

	// Determine standard deviations of column vectors of data matrix.
	for(int j = 0; j < C; j++){
		stddev[j] = 0.0;
		for(int i = 0; i < R; i++)      stddev[j] += (( Mat[i][j] - mean[j])*( Mat[i][j] - mean[j]));
        stddev[j] /= (float)R;
        stddev[j] = sqrt(stddev[j]);
        // The following in an inelegant but usual way to handle  near-zero std. dev. values, which below would cause a zero-divide.
        if (stddev[j] <= eps) stddev[j] = 1.0;
    }

	double **Mid = 	(double**)malloc(R*sizeof(double*));
	for(int i= 0; i<R;i++)		Mid[i] = new double[C];

	double x;
	for( int i = 0; i < R; i++){
		for( int j = 0; j < C;	j++){
			Mid[i][j] = Mat[i][j] - mean[j];
	        x = sqrt((float)R);
		    x *= stddev[j];
			Mid[i][j] /= x;
        }
    }

// Calculate the m * m correlation matrix.
    for(int j1 = 0; j1 < C-1; j1++){
        Res[j1][j1] = 1;
        for(int j2 = j1+1; j2 < C; j2++){
			Res[j1][j2] = 0.0;
			for (i = 0; i < R; i++)	Res[j1][j2] += ( Mid[i][j1] * Mid[i][j2]);
	        Res[j2][j1] = Res[j1][j2];
        }
    }
    Res[C-1][C-1] = 1;
	free(Mid);
	return(Res);
}

template <typename T>double Covariance( T *A, T *B, int N){
	double xy = 0;
	for( int i = 0; i < N; i++)	xy += A[i]*B[i];
	xy = xy/N;
	double mA = (double)ArrayAvg( A, N);
	double mB = (double)ArrayAvg( B, N);
	double cov = xy - mA*mB;
	return(cov);
}

//Sums-of-squares-and-cross-products matrix
template <typename T>double	**SumSqrCroProMatrix( T **Mat, int R, int C){
	double **Res = 	(double**)malloc(C*sizeof(double*));
	for(int i= 0; i<C;i++)		Res[i] = new double[C];

	for(int j1 = 0; j1 < C; j1++){
		for(int j2 = j1; j2 < C; j2++){
            Res[j1][j2] = 0;
            for(int i = 0; i < R; i++)	Res[j1][j2] += Mat[i][j1] * data[i][j2];
	        Res[j2][j1] = Res[j1][j2];
        }
    }
	return( Res);
}

template <typename T>void Swap(T &x, T &y){
	T t = x; x = y; y = t; 
}

template <typename T>T *ZeroPadding( T *A, int N, int R){
	T *B = new T[R];
	for( int i = 0; i < N; i++)		B[i] = A[i];
	for( int i = N; i < R; i++)		B[i] = 0;
	return( B );
}

template <typename T>T Bin2Dec( T *A, int N){
	int D = 0;
	for( int i = 0; i < N; i++){
		D += (int)pow(2, N-i-1)*A[i];
	}
	return( D);
}

template <typename T>int *HistArray( T *A, int N, T low, T high, int Bin){
	int *B = new int[Bin];
	for( int i = 0; i < Bin; i++)	B[i] = 0;

	double interval = (double)(high-low+0.001)/Bin;
	for( i = 0; i < N; i++){
		double value = (A[i]-low)/interval;
		int index = cvFloor( value );
		if( index < Bin)	B[index]++;
	}
	return( B );
}

template <typename T>int GlobalThreshold( T *P, int Lmax){
	int Othresh = 0;
	double sw_min = 99999;

	for( int k = 0; k < Lmax; k++){
		double q1 = 0 , q2 = 0;
		for(int i = 0; i < k; i++) q1 += P[i];
		for( i = k; i < Lmax; i++) q2 += P[i];

		double m1 = 0 , m2 = 0;
		for( i = 0; i < k; i++) m1 += (i*P[i])/q1;
		for( i = k; i < Lmax; i++) m2 += (i*P[i])/q2;

		double s1 = 0 , s2 = 0;
		for( i = 0; i < k; i++) s1 += (pow((i - m1),2)*P[i])/q1;
		for( i = k; i < Lmax; i++) s2 += (pow((i - m2),2)*P[i])/q2;

		double sw = q1*s1 + q2*s2;
		if( sw_min > sw ){
			sw_min = sw; Othresh = k;
		}
	}
	return(Othresh);
}

template <typename T>T Max3( T x, T y, T z){
	T p = x;
	if( p < y) p = y;
	if( p < z) p = z;
	return p;
}

template <typename T>T Min3( T x, T y, T z){
	T p = x;
	if( p > y) p = y;
	if( p > z) p = z;
	return p;
}

template <typename T>double Diagonal( T height, T width){
	return( sqrt( pow(height,2) + pow( width,2)));
}

template <typename T>void Array2File( T *A, int N){
	for(int i = 0; i < N; i++)		proc->filewrite<<A[i]<<endl;
}

template <typename T>T ArrayAvg( T *A, int N){
	T total= 0;
	for(int i = 0; i < N; i++)		total += A[i];
	T avg = total/N;
	return( avg);
}

template <typename T>T ArrayMax( T *A, int N){
	T max = A[0];
	for(int i = 1; i < N; i++){
		if( max < A[i]) max = A[i];
	}
	return( max);
}

template <typename T>T ArrayMax( T **A, int R, int C){
	T max = A[0][0];
	for(int i = 0; i < R; i++){
		for( int j = 0; j < C; j++){
			if( max < A[i][j]) max = A[i][j];
		}
	}
	return( max);
}

template <typename T>T ArrayMin( T *A, int N){
	T max = A[0];
	for(int i = 1; i < N; i++){
		if( max > A[i]) max = A[i];
	}
	return( max);
}

template <typename T>int ArrayStdDev( int *A, int N, int mean){
	double  total= 0;
	for(int i = 0; i < N; i++){
		int diff = abs( A[i] - N) ;
		total += diff*diff;
	}
	double sigma = total/N;
	sigma = sqrt( sigma);
	return( sigma );
}

template <typename T>T *CircularArrayShift( T *A,int N, int shift){
	T *B = new T[N];
	for( int i = 0; i < N; i++){
		int j = (i + shift)%N;
		B[i] = A[j];
	}
	return( B );
}

template <typename T>T *ArrayNormalize( T *Array,int N){
	T maxV = ArrayMax( Array, N);
	float *B = new float[N];
	for(int i = 0; i < N; i++){
		if( Array[i] < 0)	B[i] = 0;
		else B[i] = ((float)Array[i])/maxV;
	}
	return(B);
}

template <typename T>T ArraySubtraction( T *A, T *B, int num){
	T sum = 0;
	for( int i = 0; i < num; i++){
		T diff = A[i] - B[i];	if( diff < 0 ) diff = -diff;
		sum += diff;
	}
	return( sum );
}

template <typename T>T ArrayEuclideanDistance( T *A, T *B, int num){
	T sum = 0;
	for( int i = 0; i < num; i++){
		T diff = pow((A[i] - B[i]),2);
		sum += diff;
	}
	T Res = sqrt( sum);
	return( Res );
}

template <typename T>void SortMulti( T *array, int N, int c, int data, bool flag){
	if(flag == TRUE){	//desc
		for( int i = 0; i < N-1; i++){
			for( int j = i+1; j < N; j++){
				if( array[i].x[c] > array[j].x[c]){
					for( int k = 0; k < data; k++)		Swap( array[i].x[k], array[j].x[k]);
				}
			}
		}
	}
	else{
		for( int i = 0; i < N-1; i++){
			for( int j = i+1; j < N; j++){
				if( array[i].x[c] < array[j].x[c]){
					for( int k = 0; k < data; k++)		Swap( array[i].x[k], array[j].x[k]);
				}
			}
		}
	}
}

template <typename T>void Mat2File( T **mat, int height, int width){
	/*
	T a,b;
	for (int i=0; i < height; i++){
		b = -99;
		for (int j=0; j<width; j++){
			a = mat[i][j];
			if( b< 0)	proc->filewrite<<" "<<a;
			else{
				if( b<10) proc->filewrite<<"   "<<a;
				else if( b<100) proc->filewrite<<"  "<<a;
				else if( b<1000) proc->filewrite<<" "<<a;
				else proc->filewrite<<" "<<a;
			}			
			b = a;
		}		
		proc->filewrite<<endl;	
	}
	proc->filewrite<<endl;
	*/
	for (int i=0; i < height; i++){
		for (int j=0; j<width; j++){
			T a = mat[i][j];
			if( a < 10) proc->filewrite<<a<<"   ";
			else if( a < 100) proc->filewrite<<a<<"  ";
			else if( a < 1000) proc->filewrite<<a<<" ";
			else proc->filewrite<<a;
		}
		proc->filewrite<<endl;
	}
}

template <typename T>T ArrayMode( T *A, int num){
	T max_len = ArrayMax( A, num );

	T *t_array = new T[max_len+2];
	for( int i = 0; i < max_len+2; i++) t_array[i] = 0;
	
	for( i = 0; i < num; i++){
		int temp = (int)A[i];
		t_array[temp]++;
	}

	int max_frequency = 1;
	int mode = 0;
	for( i = 0; i < max_len +2; i++){
		if( t_array[i] > max_frequency ){
			max_frequency = (int)t_array[i];
			mode = i;
		}
	}
	free( t_array);
	return( mode );
}

template <typename T>void powerA( T **A, T *lambda, T *X, int N, double EPSI, int *iter){
	double *D = new double[N];
	double *Z = new double[N];

	X[0] = 1;
	for( int i = 1; i < N; i++)	X[i] = X[i-1] + .1;

	int it = 0;
	E3:
	for( int i = 0; i < N; i++){
		D[i] = 0;
		for( int j = 0; j < N; j++)	D[i] += A[i][j]*X[j];
	}
	it++;

	for( int i = 0; i < N; i++)	Z[i] = D[i]/D[0];

	for( int i = 0; i < N; i++){
		double diff = X[i] - Z[i];
		if( fabs(diff) - EPSI * fabs(Z[i]) > 0)	goto E7;
	}
	goto E9;
	E7:
	for( int i = 0; i < N; i++)	X[i] = Z[i];
	if( it >= 200)	goto E10;
	goto E3;
	E9:
	for( int i = 0; i < N; i++)	X[i] = Z[i];
	E10:
	*lambda = D[0];
	*iter = it;
	free( D);
	free( Z);
}

template <typename T>void PCA( T **Mat, int R, int C, double *Eval, double **Evector){
	double **A = CovarianceMatrix( Mat, R, C);
	int SP1 = C;
	int N = C;
	double EPSI = 0.00000001;
	int iteration;
	double *lambda = new double[SP1];
	double *omega = new double[SP1];
	double *iter = new double[SP1];
	double *X = new double[N];
	double **XX = (double**)malloc(N*sizeof(double*));
	for( int i = 0; i < N; i++)	XX[i] = new double[SP1];

	double **XIXI = (double**)malloc(N*sizeof(double*));
	for( int i = 0; i < N; i++)	XIXI[i] = new double[N];
	double sum;	
	double prod;
	double Lprime;

	for(int II = 0; II < SP1; II++){
		powerA( A, &Lprime, X, N, EPSI, &iteration);
		iter[II] = iteration;
		lambda[II] = Lprime;
	//	filewrite<<endl<<Lprime<<endl;
		Eval[II] = Lprime;

		for( int i = 0; i < N; i++) XX[i][II] = X[i];		

		sum = 0;
		for( int i = 0; i < N; i++)	sum += X[i]*X[i];
		double NF = sqrt( sum);
		for( int i = 0; i < N; i++)	X[i] /= NF;

	//	for( int i = 0; i < N; i++)	filewrite<<X[i]<<"  ";
		for( int i = 0; i < N; i++)	Evector[II][i] = X[i];
		if( II == SP1-1)	continue;

		for( int i = 0; i < N; i++){
			for( int j = 0; j < N; j++){
				XIXI[i][j] = X[i]*X[j];
			}
		}
		for( int i = 0; i < N; i++){
			for( int j = 0; j < N; j++){
				A[i][j] = A[i][j] - XIXI[i][j]*lambda[II];
			}
		}
	}
/*
	prod = 0; 
	for( int i = 0; i < N; i++)	prod += XX[i][0]*XX[i][SP1-1];
	//cout<<prod;
*/
	free(A);
	free(lambda);
	free(omega);
	free(iter);
	free(X);
	free(XX);
	free(XIXI);
}