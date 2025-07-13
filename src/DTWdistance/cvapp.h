#include "TypeDef.h"
#include "StatTemplate.h"
#include "dirent.h"
#include "limits"
#include <vector>

class ImageProcessor{

	public:
		IplImage *src_image;
		ofstream filewrite;

		ListComp *Ctxt;
		ListComp *Cdsh;
		ListComp *Cgrf;
		ListComp *Cgrftxt;
		ListSegment *Cseg;
		IplImage **img_stack;
		string	ImgName;
		double Gcount;
		char SaveDir[200];
		CvRNG rng;

		ImageProcessor(){			
			rng = cvRNG((unsigned)-1);
			filewrite.open("C:\\Work\\saveimg\\Line.txt", ofstream::app);
			strcpy( SaveDir, "C:\\Work\\saveimg\\");
			Ctxt = new ListComp();	Ctxt->clear();
			Cdsh = new ListComp();	Cdsh->clear();
 			Cgrf = new ListComp();	Cgrf->clear();
			Cgrftxt = new ListComp();	Cgrftxt->clear();
			Cseg = new ListSegment();	Cseg->clear();
			ImgName = "GlobalName";
		}

		// color convertion //
		IplImage *Col2BinImg( IplImage *, int x, bool);	// true for adding , false for remove
		IplImage *Bin2BinImg( IplImage *src, int x, bool);
		ListPix  GrayImgData( IplImage *src, IplImage *mask);
		IplImage *Gray2ColImg( IplImage *src);
		ListColPix ColImgData( IplImage *src, IplImage *mask);
		int		 BinImgPixelValue( IplImage *src, CvPoint V);
			

		IplImage *ImgNeg(IplImage *);

		IplImage *AddBorder( IplImage *, int len, int color);
		IplImage *CropImage( IplImage *src, int mincol,int minrow, int width,int height);
		IplImage *Resize( IplImage *, int width, int height);
		IplImage *Resize( IplImage *src, int T );
		IplImage *ResizeRadius( IplImage *src, float RadReqd);		
		void	 RemoveBorder( IplImage *, int, CvScalar );
		IplImage *CreateMask(IplImage *);
		IplImage *PepperSaltRemove(IplImage *, int win_size, int threshold);
		IplImage *CropBlackBorder( IplImage *);
		IplImage *SmoothingBWpixels( IplImage *, int);
		IplImage *BinImagePixel_01( IplImage *);
		CvMat    *SobelMagnitude( CvMat* A, CvMat* B);
		CvMat	 *SobelOrientation( CvMat* A, CvMat* B);
		IplImage *Rotate( IplImage *src, double, bool ColFlag);
		IplImage *Rotate90(IplImage *src, int);
		IplImage *MirrorImage( IplImage *src);
		CvPoint  *GaussianSmooth1Dpoint( CvPoint*, int N, int G);
		int      *GaussianSmooth1Dint( int*, int N, int G, bool flag);
		ListPix  PixChange( ListPix LP,  int Xoffset, int Yoffset);
		CvPoint  PixChange( CvPoint A, int Xoffset, int Yoffset);

		
		//matrix//
		void	Img2Mat(IplImage *, int **);
		void	Mat2Img(int **, IplImage *);
		void	ColImg2Mat(IplImage *, int ***);
		void	Mat2ColImg(int ***, IplImage *);
		void	Img2BinMat(IplImage *, int **);
		void	BinMat2Img(int **, IplImage *);
		void	BinMat2Gray(int **);
		IplImage *DisplayMat(int **, int width, int height);
		int		**MatrixThreshold( int **, int width, int height, int threshold);
		void	CVMatDisplay( CvMat *, char *);
		CvMat	*Img2CVBinMat(IplImage *);

		// computational geometry

		IplImage *ListPix2Img( ListPix LP);
		CvMat	*ListPix2cvMat( ListPix LP);
		double	PolarAngle( CvPoint A);
		CvPoint *MinRectangle( ListPix);
		MPix	MinCircle( ListPix P);
		double	RefinedAxisWord(IplImage *src, ListReservoir LRes, double Mang);
		ListPix ConvexHullKeyPoint( ListPix);
		ListReservoir HullComp( CvSeq* contour, IplImage *src);
		ListReservoir ResidueSmall(ListReservoir LRes, IplImage *src);
		ListPix JoinKeyPoints( ListPix, IplImage *, bool flag);
		MBoundingBox BoundingBox( ListPix );
		ListPix BinImgData( IplImage *, int);
		ListPix MatData( int **, int val, int mincol,int minrow, int maxcol, int maxrow);

		//Contour Process
		CvSeq*	  ContourSeq( IplImage *src, bool flag, CvMemStorage* storage);
		double	  PolyGonReference( CvSeq *contour, CvPoint P, bool flag);
		ListPix   ContourPixelAll(IplImage *src, bool flag);
		ListPix   *ContourPixelSet(IplImage *src, bool flag, int *set);

		
		//connected component//		
		void	BinImg2Comp( IplImage *, ListComp *, int col, bool col_flag);
		void	CompMinAreaBox( MComp *);
		ListPix CompContour( IplImage *src, CvPoint MinRowV);
		bool	CompTop( ListComp *comp, int *TopArr, MComp *CompT);
		void	CompContourPixels( ListComp *comp, IplImage *src);
		MComp	PixNearestComponent( ListComp *comp, CvPoint V);
		void	CompAreaFilter(ListComp *comp, int Area, bool MinMaxFlag);
		void	CompRemove(ListComp *comp, MComp Mdel);
		void	CompRemove(ListComp *comp, bool flag);
		void	CompHWFilter(ListComp *comp, int len, bool);
		void	CompShift( ListComp *, CvPoint TopLft);
		CvSeq	*Mat2Sequence( CvMat *vector, CvContour* contour_header, CvSeqBlock* block);
		IplImage *Comp2Img( MComp, int);
		IplImage *Pix2Img( ListPix LP, int mincol, int minrow, int wdt, int hgt, int Border);
		IplImage *Region2Img(IplImage *parent, IplImage *child, int,int);
		void	SetComponentColor( ListComp *);
		IplImage *DrawCompBoundary( ListComp *, char *);
		void	DrawBox(IplImage *src, CvPoint A, CvPoint B, CvPoint C, CvPoint D, CvScalar Col);
		void	DrawBox(IplImage *src, CvPoint A, CvPoint C, CvScalar Col, int radius);

		MComp	ComponentProperty(ListPix LP);

		//File write //
		void	Img2File( IplImage *);
		void	BinImg2File( IplImage *img);
		void	CVMat2File( CvMat* );

		//statistical//
		CvPoint MidPoint( CvPoint , CvPoint );
		int		RandomNumber(int N);
		ListPix ListMidPoint( ListPix2 LP2);
		double	AverageListInt( ListInt LI);
		double  EuclideanDist( CvPoint, CvPoint);
		ListInt DistanceListLine( ListPix2 LP);
		CvPoint LinesIntersection( CvPoint A1, CvPoint A2, CvPoint B1, CvPoint B2);
		double	CyclicDiff( double x, double y, double Max);
		double  Angle_AB_AC( CvPoint A, CvPoint B, CvPoint C);
		double  Angle2Vectors( CvPoint A, CvPoint B, CvPoint C, CvPoint D);
		double	MaxDistance(ListPix LP, CvPoint V);
		MPix	MinDistance(ListPix LP, CvPoint V);
		CvPoint Derivative(CvPoint A2, CvPoint x1);
        CvPoint PixelAngleLength( CvPoint A, double len, double angle);
		CvPoint LineAngleLength( CvPoint A, CvPoint B, double len, double angle);
		double	LineRefLineAngle( CvPoint A, CvPoint B, CvPoint C, CvPoint D);
		MPix2	LineEndPoints( CvPoint M, double angle, double distance, CvSize rect);
		CvPoint PixelAngleLengthCondn( CvPoint A, double len, double angle, CvSize rect);
		double  LineAngle( CvPoint , CvPoint );
		double  HalfCircleLineAngle( CvPoint A, CvPoint B);
		double  DividerAngle_AB_AC( CvPoint A, CvPoint B, CvPoint C);
		bool    ParallelLine( CvPoint L1S, CvPoint L1F, CvPoint L2S, CvPoint L2F);
		bool	Overlap2Line( MPix2 A, MPix2 B, double *part);
		double  Curvature( double x1, double y1, double x2, double y2);
		int     Point_refin2line( CvPoint P, CvPoint L1, CvPoint L0, CvPoint L2);
		CvPoint PixelDirCode(CvPoint V, int d);
		int     PixelPixelDirCode(CvPoint V, CvPoint R);
		ListPix SortPixel( ListPix LP, bool flag);
		double  MomentAngle( ListPix, CvPoint CG);
		CvPoint MassPoint( ListPix);
		CvPoint PartialMassPoint( ListPix LP, int col, bool flag);
		bool	ReflectiveMass( ListPix LP, CvPoint L1, CvPoint L2);
		int		IndexPrev( int N, int i, int diff);
		int		IndexNext( int N, int i, int diff);
		ListPix PixelListSet2List( ListPix *LS, int N);
		CvPoint AntiClockTracing( IplImage *src, CvPoint Vcur, CvPoint Vprev);
		CvPoint AntiClockTracing( IplImage *src, CvPoint Vcur);
		CvPoint ClockTracing( IplImage *src, CvPoint Vcur, CvPoint Vprev);
		CvPoint ClockTracing( IplImage *src, CvPoint Vcur);
		ListPix Tracing( IplImage *src, CvPoint st);
		ListPix Tracing( IplImage *src, CvPoint st, CvPoint fn);
		ListPix Tracing(CvPoint CS, CvPoint CF, CvPoint *CP, int N);
		CvPoint VerticalDown( IplImage *src, CvPoint V, int val);
		int		RunLengthLastPixel( int **MAT, int width, int height, CvPoint V, int dir);
		bool 	ImgDivision( IplImage *src, MPix2 MP, MComp *M1, MComp *M2, int Lw);
		CvPoint NearestPixelCol(IplImage *src, CvPoint V, bool flag);
		
		
		ListPix EdgePixels( CvPoint A, CvPoint B, IplImage *);
		ListPix LinePixels( CvPoint A, CvPoint B, IplImage *src);
		int		LinePixelsNum( CvPoint S, CvPoint T, IplImage *src, int);
		CvPoint LineClipPoint( CvPoint A, double angle, CvSize rect);
		CvPoint *ChainPixels( Graph *, int v, int e, int *, IplImage *);
		CvPoint LastPixelAngle( IplImage *, CvPoint, double angle, int col_val);
		CvPoint OppBorderPixel( CvPoint A, double angle, IplImage *src, int col);
		ListPix CvPointArraySelection( int init, int fin, CvPoint *CP, int N);

		//Display & Selection//
		void	DisplayMovie( string str);
		void	DisplayMsg(string str);		
		void	DisplayMsg(double N);
		void	DisplayMsg( ListStr strlist);
		void	Stop();
		void	DisplayImg(IplImage *src, char *name);		
		void	DisplayImg(IplImage *src, int N);
		void	DisplayImg(IplImage *src);
		void	DisplayColorValue( IplImage *);
		IplImage *DisplayLineWordSeparation(IplImage *);
		void	DrawListPixel( IplImage *, ListPix, int, CvScalar);
		void	DrawPixel( IplImage *, CvPoint, int, CvScalar);
		void	DrawLine( IplImage *, CvPoint, CvPoint, int, CvScalar);
		void	DrawLine2Pix(IplImage *src, CvPoint V1, CvPoint V2, CvScalar Col);
		void	DrawListLine( IplImage *src, ListPix2 LP, int radius, CvScalar Col);
		void	DrawMatLine( int **Mat, CvPoint A, CvPoint B, int val, bool HVflag);
		IplImage *DrawCircularListPixel( ListDouble);
		void	MousePoint( IplImage *);
		IplImage *MouseImgSelect( IplImage *);
		CvPoint GetMousePosition( IplImage *);
		void	DrawImgAngle( IplImage *src, double angle, int val, CvScalar Col);

	
		//Operate in Directory 
		string	RemoveDirectoryName( string file);
		string  FindDirectoryName( string file);
		ListStr ReadFileData( string filename);		
		void	ListImgComparisonDriver(ListStr strlist);
		ListStr FindFullAddress(ListStr strlist, ListStr filename);
		void	CreateRotatedCharDatabase( IplImage *src, int N);
		double	**ReadFileMatrix(string, int *, int *);

		char	EngCharMapping( int num, bool);
		int		NameDupConvert( int cname, bool);

		float	*ProjectionProfile( int **, int height, int width, int Nfactor);
		float	*UpperProfile( int **, int height, int width, int Nfactor);
		float	*LowerProfile( int **, int height, int width, int Nfactor);
		float	*TransitionFG( int **, int height, int width, int Nfactor);
		float	DTW_Compare(float *sig1, float *sig2, int N1, int N2, int Window);
		float   **ImgFeatureSignal(IplImage *img);
		IplImage *ImgNormaizeDTW(IplImage *img, int, int, bool);

		
		void ImgRelease(){
			cvReleaseImage( &src_image ); 
			cvReleaseImage( img_stack);
		}

		~ImageProcessor(){
		}
}Proc;


void ImageProcessor::DisplayImg(IplImage *src,char *name){
	cvDestroyWindow(name);
	cvNamedWindow( name, 1 ); 			
	cvShowImage( name, src);
	cvWaitKey(0);
}


IplImage *ImageProcessor::ImgNeg(IplImage *src){
	IplImage *tgt = cvCreateImage( cvGetSize(src), src->depth, src->nChannels );
	unsigned char *data1=(unsigned char *)(src->imageData);
	unsigned char *data2=(unsigned char *)(tgt->imageData);

	int ncol= src->width * src->nChannels;

	for (int i=0; i<src->height; i++) {
		for (int j=0; j<ncol; j++) {
			data2[j] = 255 - data1[j];			
		}
		data2+=tgt->widthStep;
		data1+=src->widthStep;
	}
	return(tgt);
}

void ImageProcessor::DrawLine(IplImage *src, CvPoint V1, CvPoint V2, int radius, CvScalar Col){
	cvLine( src, V1, V2, Col, radius, 8, 0);
}


void ImageProcessor::RemoveBorder( IplImage *src, int len, CvScalar Col){
	if( len > 0){
		DrawLine( src, cvPoint( 0,0), cvPoint( src->width -1,0), len, Col);
		DrawLine( src, cvPoint( 0,0), cvPoint( 0, src->height-1), len, Col);
		DrawLine( src, cvPoint( src->width-1,src->height-1), cvPoint( src->width-1,0), len, Col);
		DrawLine( src, cvPoint( src->width-1,src->height-1), cvPoint( 0,src->height-1), len, Col);
	}
}


void ImageProcessor:: Img2BinMat(IplImage *img, int **matrix){
	unsigned char *data=(unsigned char *)(img->imageData);
	for(int i=0;i< img->height;i++){
		for( int j=0;j<img->width;j++){
			if( data[j] < 127 ) 	matrix[i][j]=0;			
			else matrix[i][j]=1;			
		}
		data+=img->widthStep;
	}
}

void ImageProcessor::BinMat2Img(int **matrix, IplImage *img){
	unsigned char *data=(unsigned char *)(img->imageData);
	for(int i=0; i< img->height; i++){
		for( int j=0; j<img->width; j++){
			if( matrix[i][j] == 1 ) data[j] = 255;
			else data[j] = 0;
		}
		data+=img->widthStep;		
	}
}

void ImageProcessor::DrawBox(IplImage *src, CvPoint A, CvPoint B, CvPoint C, CvPoint D, CvScalar Col){
	if( A.x == C.x && A.y == C.y && B.x == D.x && B.y == D.y){
		DrawLine( src, A, cvPoint( A.x, B.y), 0, Col);
		DrawLine( src, cvPoint( A.x, B.y), B, 0, Col);
		DrawLine( src, B, cvPoint( B.x, A.y), 0, Col);
		DrawLine( src, cvPoint( B.x, A.y), A, 0, Col);
	}
	else{
		DrawLine( src, A, B, 0, Col);	DrawLine( src, B, C, 0, Col);
		DrawLine( src, C, D, 0, Col);	DrawLine( src, D, A, 0, Col);
	}
}

void ImageProcessor::DrawBox(IplImage *src, CvPoint A, CvPoint C, CvScalar Col, int radius){
		DrawLine( src, A,  cvPoint( A.x, C.y), radius, Col);
		DrawLine( src, cvPoint( A.x, C.y), C, radius, Col);
		DrawLine( src, C, cvPoint( C.x, A.y), radius, Col);
		DrawLine( src, cvPoint( C.x, A.y), A, radius, Col);
}

void ImageProcessor::DrawListPixel(IplImage *src, ListPix P, int radius, CvScalar Col){
	for( ListPix::iterator Pi = P.begin(); Pi != P.end(); ++Pi)		DrawPixel( src, (*Pi).V, radius, Col);
}

void ImageProcessor::DrawPixel(IplImage *src, CvPoint V, int radius, CvScalar Col){
	cvCircle( src, V, radius, Col, 1);
}

ListPix ImageProcessor::BinImgData( IplImage *src, int val){
	ListPix LP;	MPix Pix;
	unsigned char *data= reinterpret_cast<unsigned char *>(src->imageData);

	for (int i=0; i < src->height; i++){
		for (int j=0; j<src->width; j++){
			if( data[j] == val){
				Pix.V.x = j;	Pix.V.y = i;
				LP.insert( LP.end(), Pix);
			}
		}
		data+=src->widthStep;
	}
	return( LP);
}


int ImageProcessor::BinImgPixelValue( IplImage *src, CvPoint V){
	int val = 0;
	if( V.x >= 0 && V.x < src->width && V.y >= 0 && V.y < src->height){
		val = cvGet2D( src, V.y, V.x).val[0];
	}
	return( val);
}

ListPix ImageProcessor::PixelListSet2List( ListPix *LS, int N){
	ListPix LP;	LP.clear();
	for( int i = 0; i < N; i++){
		for( ListPix::iterator Li = LS[i].begin();	Li != LS[i].end();	++Li){
			LP.insert( LP.end(), *Li);
		}
	}
	return(LP);
}

MBoundingBox ImageProcessor::BoundingBox( ListPix LP){
	MBoundingBox Box;
	ListPix::iterator Pi = LP.begin();
	Box.BotRgtV.x = Box.TopLftV.x = (*Pi).V.x;	Box.BotRgtV.y = Box.TopLftV.y = (*Pi).V.y;
	Box.MinRowV = Box.MaxRowV = Box.MinColV = Box.MaxColV = (*Pi).V;

	for( ; Pi != LP.end(); ++Pi){
		if( (*Pi).V.x < Box.TopLftV.x){	Box.TopLftV.x = (*Pi).V.x; Box.MinColV = (*Pi).V;}
		if( (*Pi).V.x > Box.BotRgtV.x){	Box.BotRgtV.x = (*Pi).V.x;	Box.MaxColV = (*Pi).V;}
		if( (*Pi).V.y < Box.TopLftV.y){	Box.TopLftV.y = (*Pi).V.y;	Box.MinRowV = (*Pi).V;}
		if( (*Pi).V.y > Box.BotRgtV.y){	Box.BotRgtV.y = (*Pi).V.y;	Box.MaxRowV = (*Pi).V;}
	}
	Box.HW.height = Box.BotRgtV.y - Box.TopLftV.y + 1;
	Box.HW.width = Box.BotRgtV.x - Box.TopLftV.x + 1;
	return( Box);
}

CvSeq*	ImageProcessor::ContourSeq( IplImage *src, bool flag, CvMemStorage *storage){
	CvSeq* contour = 0;
	IplImage *src_cpy = cvCloneImage( src);
	if( flag == TRUE)
	cvFindContours( src_cpy, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	else cvFindContours( src_cpy, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
	cvReleaseImage( &src_cpy );
	return( contour);
}

ListPix *ImageProcessor::ContourPixelSet(IplImage *src, bool flag, int *N){
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq*	contour = ContourSeq( src, flag, storage);
	MPix P;
	int NList = 0;
	CvSeq *contourtemp = contour;
	for( ; contour != 0; contour = contour->h_next )	NList++;
	*N = NList;
	contour = contourtemp;

	ListPix *LP = new ListPix[NList];
	int set = 0;
	for( ; contour != 0; contour = contour->h_next ){
		CvSeq* result = contour;
		for( int i = 0; i < result->total; i++){
			CvPoint *CP = (CvPoint*)cvGetSeqElem( result, i );
			P.V.x = (*CP).x;	P.V.y = (*CP).y;	P.VI = set;
			LP[set].insert( LP[set].end(), P);
		}
		set++;
	}
	cvReleaseMemStorage( &storage );
	return( LP);
}


ListPix ImageProcessor::ContourPixelAll(IplImage *src, bool flag){
	int N;
	ListPix *LS = ContourPixelSet(src, flag, &N);		
	ListPix LP = PixelListSet2List( LS, N);
	return( LP);
}

void ImageProcessor::DisplayImg(IplImage *src){
	string in_name = RemoveDirectoryName( ImgName);
	DisplayImg( src, (char *)in_name.c_str());
}

string ImageProcessor::RemoveDirectoryName( string file){
	int file_length = (int)file.length();
	int index_d = (int)file.find_last_of( '\\', file_length);
	int index_p = (int)file.find_last_of( '.', file_length);
	string file_out = file.substr( index_d+1, index_p-index_d-1);
	return( file_out);
}

bool Compare_Ext( string filename, string file_ext){
	int file_length = (int)filename.length();
	string filename_ext = filename.substr( file_length - 3,3);
	int match_flag = strcmp( filename_ext.c_str(), file_ext.c_str());
	if( match_flag == 0) return( true);
	else return(false);
}


IplImage *ImageProcessor::CropBlackBorder( IplImage *src){
	int minrow = src->height;	int mincol = src->width;
	int maxrow = 0;	int maxcol = 0;

	unsigned char *data= reinterpret_cast<unsigned char *>(src->imageData);
	for(int i=0;i< src->height;i++){
		for( int j=0;j< src->width;j++){
			if( data[j] == WHITE){
				if( minrow > i )minrow = i;
				if( mincol > j )mincol = j;
				if( maxrow < i) maxrow = i;
				if( maxcol < j) maxcol = j;
			}
		}
		data += src->widthStep;
	}
	IplImage *tgt = CropImage( src, mincol, minrow, maxcol-mincol+1, maxrow - minrow +1);
	return(tgt);
}

IplImage *ImageProcessor::CropImage( IplImage *src, int mincol,int minrow, int width,int height){
	IplImage *tgt = cvCreateImage( cvSize(width, height), src->depth , src->nChannels );
	cvZero(tgt);
	
	int ncol= src->width*src->nChannels;	
	unsigned char *tgtdata= reinterpret_cast<unsigned char *>(tgt->imageData);
	unsigned char *srcdata= reinterpret_cast<unsigned char *>(src->imageData);

	for (int i=0; i<src->height; i++){
		if( i >= minrow && i < minrow + height){
			int k = 0;
			for (int j=0; j<ncol; j+= src->nChannels){
				if( j >= mincol*src->nChannels && j < (mincol + width)*src->nChannels){
					tgtdata[k]=srcdata[j+0];
					tgtdata[k+1]=srcdata[j+1];
					tgtdata[k+2]=srcdata[j+2];
					k+=tgt->nChannels;
				}
			}
			tgtdata+=tgt->widthStep;
		}		
		srcdata+=src->widthStep;
	}
	return ( tgt );
}



float	*ImageProcessor::ProjectionProfile( int **Mat, int height, int width, int Nfactor){
	float *hist = new float[width];
	for( int j = 0; j < width; j++)	hist[j] = 0;
	for( int i = 0; i < height; i++){			
		for( int j = 0; j < width; j++){
			if( Mat[i][j] == 1 ) hist[j]++;
		}
	}
	for( j = 0; j < width; j++)	hist[j]/= Nfactor;
	return(hist);
}

float	*ImageProcessor::UpperProfile( int **Mat, int height, int width, int Nfactor){
	float *hist = new float[width];
	for( int j = 0; j < width; j++)	hist[j] = 0;
	for( j = 0; j < width; j++){			
		for( int i = 0; i < height; i++){
			if( Mat[i][j] == 1){
				hist[j] = height - i - 1;	break;
			}
		}
	}
	for( j = 0; j < width; j++)	hist[j]/=Nfactor;
	return(hist);
}

float	*ImageProcessor::LowerProfile( int **Mat, int height, int width, int Nfactor){
	float *hist = new float[width];
	for( int j = 0; j < width; j++)	hist[j] = 0;
	for( j = 0; j < width; j++){			
		for( int i = height-1; i >= 0; i--){
			if( Mat[i][j] == 1){
				hist[j] = i;	break;
			}
		}
	}
	for( j = 0; j < width; j++)	hist[j]/=Nfactor;
	return(hist);
}

float	*ImageProcessor::TransitionFG( int **Mat, int height, int width, int Nfactor){
	float *hist = new float[width];
	for( int j = 0; j < width; j++)	hist[j] = 0;
	for( j = 0; j < width; j++){			
		for( int i = 1; i < height; i++){
			if( Mat[i][j] == 1 && Mat[i-1][j] == 0)	hist[j]++;
		}
	}
	for( j = 0; j < width; j++)	hist[j]/=Nfactor;
	return(hist);
}

float ImageProcessor::DTW_Compare(float *sig1, float *sig2, int N1, int N2, int Window){
	float **DTW = 0; 
	DTW = new float *[N1+1];
	for( int i = 0 ; i <= N1 ; i++ )	DTW[i] = new float[N2+1];

	for(i = 0; i <= N1; i++)
		for(int j = 0; j <= N2; j++)
			DTW[i][j] = numeric_limits<float>::infinity();
	DTW[0][0] = 0.0;

	for(i = 1; i < N1+1; i++){		
		//for(int j = 1; j < N2+1; j++){
		for(int j = max(1, i - Window); j < min(N2+1, i + Window); j++){
			float cost= fabs(sig1[i-1] - sig2[j-1]);
			DTW[i][j] = cost + Min3(DTW[i-1][j], DTW[i][j-1], DTW[i-1][j-1]);  // insertion, deletion & match
		}
	}

	float R = DTW[N1][N2];
	for( i = 0 ; i <= N1 ; i++ )	delete [] DTW[i] ;
	delete [] DTW;
	return(R);	//return(0.0);
}

IplImage *ImageProcessor:: Resize( IplImage *src, int width, int height){
	IplImage *tgt = cvCreateImage( cvSize(width, height), src->depth, src->nChannels );
	cvResize( src, tgt, CV_INTER_LINEAR );
	return( tgt );
}

float **ImageProcessor::ImgFeatureSignal(IplImage *src){
	int **MAT =  (int**)malloc(src->height*sizeof(int*));
	for(int i = 0; i < src->height; i++) MAT[i] = new int[src->width];
	Img2BinMat(src, MAT);
	float **Signal =  (float**)malloc(4*sizeof(float*));
	Signal[0] = ProjectionProfile(MAT, src->height, src->width, src->height);
	Signal[1] = UpperProfile(MAT, src->height, src->width, src->height);
	Signal[2] = LowerProfile(MAT, src->height, src->width, src->height);
	Signal[3] = TransitionFG(MAT, src->height, src->width, 6);
	free(MAT);
	return(Signal);
}

IplImage *ImageProcessor::ImgNormaizeDTW(IplImage *img, int height, int width, bool Norm){
	IplImage *src = cvCloneImage(img);
	cvThreshold( src, src, 128, WHITE, CV_THRESH_OTSU); src = ImgNeg(src);
	ListPix CntrPix = ContourPixelAll(src, FALSE);
	MBoundingBox ImgBox = BoundingBox(CntrPix);
	src = CropImage(src, ImgBox.TopLftV.x, ImgBox.TopLftV.y, ImgBox.HW.width, ImgBox.HW.height);
	src = Resize(src, width, height);
	cvThreshold( src, src, 128, WHITE, CV_THRESH_OTSU);
	return(src);
}
