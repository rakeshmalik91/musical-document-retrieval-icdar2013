#include "string"
#include "math.h"
#include "complex"
#include "fstream"
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "stdlib.h"
#include "time.h"
#include "list"

using namespace std;

#define BLACK 0
#define WHITE 255
#define RGB_RED cvScalar(0,0,255)
#define RGB_GREEN cvScalar(0,255,0)
#define RGB_BLUE cvScalar(255,0,0)
#define RGB_WHITE cvScalar(255,255,255)
#define RGB_BLACK cvScalar(0,0,0)
#define RGB_PINK cvScalar(255,0,255)
#define RGB_YELLOW cvScalar(0,255,255)
#define RGB_SKY cvScalar(255,255,0)

#define PI 3.14159265359

typedef struct MCol{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
}MCol;

typedef struct MPix{
	CvPoint V;
	int		VI;
	double	VD;
}MPix;
typedef list<MPix> ListPix;

typedef struct MVal{
	//char	Ch;
	int		VI;
	int		VII;
	double	VD;
}MVal;
typedef list<MVal> ListVal;
typedef list<ListVal> ListListVal;

typedef struct MColPix{
	CvPoint V;
	MCol	C;
}MColPix;
typedef list<MColPix> ListColPix;

typedef struct MPix2{
	CvPoint V[2];
	int		VI;
	double  VD;
}MPix2;
typedef list<MPix2> ListPix2;

typedef struct NGB{
	int ngb_comp;
	NGB *next;
}NGB;

typedef struct connectedEdge{
	int edge;
	int opp_edge;
	bool flag;
	connectedEdge *next;
}connectedEdge;

typedef struct G_VT{
	CvPoint V;
	int degree;
	connectedEdge *ptrEdge;
	bool flag;
}G_VT;	

typedef struct G_ED{
	int V1;
	int V2;
	double width;
	bool flag;
}G_ED;

typedef struct Graph{
	G_VT *ptrV;
	int Vnum;
	G_ED *ptrE;
	int Enum;
}Graph;

typedef struct MLine{
	CvPoint V[2];	
	double length;
	double width;
	double angle;
	MLine *next;
}MLine;
typedef list<MLine> ListLine;

typedef struct MVector{
	MLine *L;
	ListLine line;
	double length;
	double width;
	MVector *next;
	CvPoint MinAreaRect[4];
	CvSize HW;
	double size;
	MPix EndVtx[2];
	bool Degree1Vector;
}MVector;
typedef list<MVector> ListVector;

typedef struct MBoundingBox{
	CvPoint TopLftV;
	CvPoint BotRgtV;
	CvPoint MinRowV;
	CvPoint MinColV;
	CvPoint MaxRowV;
	CvPoint MaxColV;
	CvSize HW;
}MBoundingBox;

typedef struct MReservoir{
	ListPix LP;
	int area;
	MPix DirSt;
	MPix DirFn;
	double height;
	double width;
	CvPoint Vlow[2];
	MBoundingBox B_Box;
}MReservoir;
typedef list<MReservoir> ListReservoir;

typedef struct MLoop{
	ListPix LP;
	CvPoint MinAreaRect[4];
	CvSize HW;
	MBoundingBox B_Box;
}MLoop;
typedef list<MLoop> ListLoop;

typedef struct MFeature{
	int CountReservoir;
	int CountLoop;
	CvPoint VRef;
}MFeature;

typedef struct MComp{
	int area;
	MBoundingBox B_Box;
	CvSize MinBoxHW;
	CvPoint MinBoxPt[4];
	ListPix Pt;
	ListPix Cntr;
	ListReservoir	LRes;
	MFeature PropList;
	MCol RGB;
	double size;
	int TXT_type;	//1-> isolated char, 2-> connected
	int color;	//assign a number to each color
	bool flag;
}MComp;
typedef list<MComp> ListComp;

typedef struct MSegment{
	ListPix pix;
	ListPix2 edge;	
	CvSize HW;
	double size;
	CvPoint MinBoxPt[4];
	bool flag;
}MSegment;
typedef list<MSegment> ListSegment;

typedef struct MGroupSegment{
	ListSegment *segN;
	ListPix pix;
	CvPoint MinBoxPt[4];
	MBoundingBox B_Box;
	CvSize HW;
	bool flag;
}MGroupSegment;
typedef list<MGroupSegment> ListGroupSegment;

typedef struct {
	int x[5];
}MInt;
typedef list<MInt> ListInt;
typedef list<ListInt> ListListInt;

typedef struct GraphGen{
	ListInt LVtx;
	ListInt LEdg;
}GraphGen;

typedef struct StringInt{
	int ind;
	float data;
	string desc;
}StringInt;

typedef struct MDouble{
	double x[5];
}MDouble;
typedef list<MDouble> ListDouble;
typedef list<ListDouble> ListListDouble;

typedef list<string>ListStr;
typedef list<ListStr>ListListStr;

typedef struct ImgArray{
	IplImage *img;
}ImgArray;
typedef list<ImgArray> ListImg;

typedef list<IplImage *>ListImg1;