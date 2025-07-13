#include "common.h"
#define CHECK_BLACK_NOT_COVERED(thinned_image,lined_image,i,j) CV_IMAGE_ELEM(thinned_image,uchar,i,j) == 0 && CV_IMAGE_ELEM(lined_image,uchar,i,j) != 0 
void fill_open(IplImage *lined_image,SE pt1,SE pt2,int top,int bottom)
{
	int j_temp,i,j;
   try
   {
	int start=pt1.start<pt2.start?pt1.start:pt2.start;
	int end=pt1.end>pt2.end?pt1.end:pt2.end;
	for(i=top;i<=bottom;i++)
	{
		float mul=(i-top)/(float)(bottom-top);
		int mid1=0,mid2=0;
		if(pt2.start>pt1.start)
			mid1=(pt2.start-pt1.start)*mul+pt1.start;
		else 
			mid1=(pt1.start-pt2.start)*(1-mul)+pt2.start;	

		if(pt2.end<pt1.end)
			mid2=pt2.end-(pt2.end-pt1.end)*(1-mul);
		else 
			mid2=pt1.end-(pt1.end-pt2.end)*(mul);

		for(j=start;j<mid1;j++)
		{
			cvSetReal2D(lined_image,i,j,0);
		}
		for(j=mid1;j<=mid2;j++)
		{
			cvSetReal2D(lined_image,i,j,255);
		}
		for(j=mid2+1;j<=end;j++)
		{
			cvSetReal2D(lined_image,i,j,0);
		}
	}

	//	}
   }
	catch(...)
	{
		cout << "\nCaught an exception --fill_open _DEFAULT i="<<i<<" j="<<j<<"\n";
	}
}
void fill(IplImage *lined_image,int x,int top_bottom,SE pt1)
{									
   try{
	float mul=(float)1.2,j_temp,i,j,ratio,mul2;
	int val,first,last;
	ratio=(float)abs(pt1.end-pt1.start)/abs(x-top_bottom);
    if(ratio<1 )mul=0.5  ;
	if(ratio>1.5 )mul=1.5+(ratio-1.5)*0.75;
	//if(abs(x-top_bottom)*2.2<abs(pt1.end-pt1.start) )mul=2 ;
	//if(abs(x-top_bottom)*3<abs(pt1.end-pt1.start) )mul=2.6 ;
	
	if(top_bottom<x)
	{//top

		first=top_bottom;
		last=x;
	}
	else
	{ //bottom
		first=x;
		last=top_bottom;
	}
	for(j_temp=pt1.start;j_temp<=pt1.end;j_temp++)
		for(i=first;i<=last;i++)
		{
			val=abs(i-top_bottom)*mul;
			if(val>j_temp-pt1.start || val>pt1.end-j_temp )
				cvSetReal2D(lined_image,i,j_temp,0);
			else
				cvSetReal2D(lined_image,i,j_temp,255);
		}
   }
		catch(...)
	{
		cout << "\nCaught an exception --fill _DEFAULT\n";
	}

}
int check_and_fill(const IplImage * image,IplImage * lined_image,int x,vector <SE> up,vector <SE> down,int top,int bottom)
{

	int up_start=0,up_end=0,down_start=0,down_end=0,open=0,i,j,j_temp,ans=0,up_size,down_size,up_c=0,dw_c=0,val;
	int	height=image->height;
	int	width=image->width;
	int tnc=7; //tolerance
	float mul;
	up_size=up.size();
	down_size=down.size();
	SE up_pt,dw_pt,o_up,o_dw;
	try{

		//cout<<endl<<"i= "<<x<<" up_size="<<up_size<<" down_size="<<down_size;
		while(up_c<up_size && dw_c<down_size)
		{
			//	cout<<" "<<up_c<<","<<dw_c<<" "; 
			if((down[dw_c].start>=up[up_c].start  || down[dw_c].end>=up[up_c].start )&& up_c<up_size)
			{
				int con=0,loc=0,loc2=0,flag=0,flag2=0;
				up_pt=up[up_c];
				while(dw_c+loc>0 && down[dw_c+loc].end>=up_pt.start-tnc )
					loc--;
				while(dw_c+loc<down_size && down[dw_c+loc].end<=up_pt.start-tnc )
					loc++;
				if(dw_c+loc<down_size)dw_pt=down[dw_c+loc];
				while(dw_c+loc<down_size && dw_pt.start<=up_pt.end+tnc )
				{				

					loc2=0;
					flag2=0;

					while(up_c+loc2>0 && up[up_c+loc2].end>=dw_pt.start-tnc )
						loc2--;
					while(up_c+loc2<up_size && up[up_c+loc2].end<=dw_pt.start-tnc )
						loc2++;
					if(up_c+loc2<up_size )up_pt=up[up_c+loc2];
					while(up_c+loc2<up_size && up_pt.start<=dw_pt.end+tnc)
					{
						con++;
						if(con==1)
						{
							o_up=up_pt;
							o_dw=dw_pt;
						}
						loc2++;
						if(up_c+loc2<up_size)up_pt=up[up_c+loc2];	
					}
					if(con>1)break;
					loc++;
					if(dw_c+loc<down_size)dw_pt=down[dw_c+loc];
					up_pt=up[up_c];
				}

				switch(con)
				{
				case 0://closed
					fill(lined_image, x,top, up[up_c]);
					break;
				case 1://open
					fill_open(lined_image,o_up,o_dw,top,bottom);
					break;
					//default: //multiple open
					//no action required, default line drawn before function call is ok
				}
				//dw_c+=con;
				up_c++;
			}
			else{
				int con2=0,flag=0; 
				if(up_c!=0) con2=-1; 
				dw_pt=down[dw_c];
				while(flag==0 && up_c+con2<up_size && up[up_c+con2].start<=down[dw_c].end+tnc)
				{
					up_pt=up[up_c+con2];
					if(up_pt.start<=dw_pt.end+tnc && up_pt.start+tnc>=dw_pt.start-tnc) 			flag=1;
					else if(up_pt.end<=dw_pt.end+tnc && up_pt.end>=dw_pt.start-tnc)				flag=1;
					else if(up_pt.start<=dw_pt.start-tnc && up_pt.end>=dw_pt.end+tnc)			flag=1;
					con2++;
				}
				if(flag==0)
				{
					fill(lined_image, x,bottom, down[dw_c]);
				}
				dw_c++;
			}
		}

	}catch( cv::Exception& e )
		{
			const char* err_msg = e.what();
			cout << "\nCaught an exception --check_and_fill "<<err_msg<<"\n";
		}
	catch(...)
	{
		cout << "\nCaught an exception --check_and_fill _DEFAULT\n";
	}
	return ans;
}