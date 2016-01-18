#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "geometry.h"
#include "index.h"
#include <windows.h>
#include <process.h>
#include <time.h>
#include <math.h>
int LATNUM=1976;														//纬度分成的数量
int LONGNUM=2876;														//经度分成的数量
double GRIDLAT=0.00179815;												//网格索引纬度分量
double GRIDLONG=0.002797408;											//网格索引经度分量
double CHANGELAT;
double CHANGELONG;
int cellSize=200;
int divSize;

void resetCellSize(){
	int newCellSize=200-20*dataNumber;
	if (newCellSize<75) newCellSize=75;
	
	GRIDLAT=GRIDLAT*newCellSize/cellSize;
	GRIDLONG=GRIDLONG*newCellSize/cellSize;

	LATNUM=(int)((maxLat-minLat)/GRIDLAT)+1;
	LONGNUM=(int)((maxLong-minLong)/GRIDLONG)+1;
	
	CHANGELAT=1/GRIDLAT;
	CHANGELONG=1/GRIDLONG;
	
	cellSize=newCellSize;
	divSize=cellSize-10;
}
 
int cmp ( const void *a , const void *b ) { 
	return *(int *)a - *(int *)b; 
} 

/**
 *description:找到一个指定点所可以匹配的路径(网格，还需进一步缩小到200M半径的园)
 *param:double double 点坐标，int* 结果数组,int* 个数
 *return:
 */
void getRelateEdgeSet(int centerGrid,int result[],int* resNum){
	int resNumber=0;
	int p,q,laIndex;
	for (p=-1;p<2;++p)
		for (q=-1;q<2;++q){
			//int nbGrid=getNeighbor(centerGrid,p,q);
			int nbGrid = centerGrid + p * LONGNUM + q;
			if (nbGrid>-1){
				laIndex=indexLast[nbGrid];
				while (laIndex>0){
					result[resNumber++]=indexOther[laIndex];
					laIndex=indexPre[laIndex];
				}
				laIndex=indexLast2[nbGrid];
				while (laIndex>0){
					result[resNumber++]=indexOther2[laIndex];
					laIndex=indexPre2[laIndex];
				}
			}
		}
	qsort(result,resNumber,4,cmp);
	int tmpNum=0;
	for (p=1;p<resNumber;++p)
		if (result[p]>result[tmpNum])
			result[++tmpNum]=result[p];
	*resNum=tmpNum+1;
}

int inWhichGrid(double nodeLat,double nodeLong){
	return (int)((nodeLat-minLat)*CHANGELAT)*LONGNUM+(int)((nodeLong-minLong)*CHANGELONG)+1;
}

/**
 *description:获取一个网格的边界
 *param:int 网格Id，double double 左上角点，double double 右下角点
 *return:
 */
void gridGetBound(int gridId,double* x1,double* y1,double* x2,double* y2){
	int xN=gridId/LONGNUM,yN=gridId % LONGNUM;
	*x1=xN*GRIDLAT+minLat;*x2=(xN+1)*GRIDLAT+minLat;
	*y1=yN*GRIDLONG+minLong;*y2=(yN+1)*GRIDLONG+minLong;
}


static void insertLineToIndex(int edgeId, double lat1, double long1, double lat2, double long2)
{
	int div=(int)fabs((lat2-lat1)*CHANGELAT);
	int div2=(int)fabs((long2-long1)*CHANGELONG);
	if (div2>div) div=div2;
	++div;
	double x=lat1, y=long1, dx=(lat2-lat1) / div, dy=(long2-long1) / div;
	int centerGrid, edgeLast;
	int i;
	for (i = 0; i <= div; ++i) {
		centerGrid = inWhichGrid(x,y);
		edgeLast = indexLast[centerGrid];
		if (!edgeLast || indexOther[edgeLast] != edgeId) {
			++crossSize;
			indexPre[crossSize] = edgeLast;
			indexLast[centerGrid] = crossSize;
			indexOther[crossSize] = edgeId;
		}
		x+=dx;y+=dy;
	}
}

static void insertLineToIndex2(int edgeId, double lat1, double long1, double lat2, double long2)
{
	int div=(int)fabs((lat2-lat1)*CHANGELAT);
	int div2=(int)fabs((long2-long1)*CHANGELONG);
	if (div2>div) div=div2;
	++div;
	double x=lat1, y=long1, dx=(lat2-lat1) / div, dy=(long2-long1) / div;
	int centerGrid, edgeLast;
	int i;
	for (i = 0; i <= div; ++i) {
		centerGrid = inWhichGrid(x,y);
		edgeLast = indexLast2[centerGrid];
		if (!edgeLast || indexOther2[edgeLast] != edgeId) {
			++crossSize2;
			indexPre2[crossSize2] = edgeLast;
			indexLast2[centerGrid] = crossSize2;
			indexOther2[crossSize2] = edgeId;
		}
		x+=dx;y+=dy;
	}
}


void workOdd(){
	int i;
	for (i=0;i<dataNumber;i+=2){
		while (!inputLoaded[i]) Sleep(50);
		coreDP(i, dp2, dpSize2, dpEdge2, dpLeft2, dpLen2, dpPre2);
	}
	++doneFlag;
}

void workEven(){
	int i;
	for (i=1;i<dataNumber;i+=2){
		while (!inputLoaded[i]) Sleep(50);
		coreDP(i, dp, dpSize, dpEdge, dpLeft, dpLen, dpPre);
	}
	++doneFlag;
}

/**
 * Build Gird Index
 */
void createGirdIndex2() {
	int i=0, j, p,dt=1;
	double x1, y1, x2, y2;

	for ( i=edgeNumber/2; i<edgeNumber; ++i) 
		for (j = edgeStart[i]; j < edgeStart[i + 1] - 1; ++j) {
			int q= j << 1;
			insertLineToIndex2(i, coordNet[q], coordNet[q+1], coordNet[q+2], coordNet[q+3]);
		}
	
	++gridFlag;
}
 
void createGirdIndex() {
	_beginthread( createGirdIndex2, 0, NULL );
	int i=0, j, p,dt=1;
	double x1, y1, x2, y2;
	int halfEdge=edgeNumber/2;
	for ( i=0; i<halfEdge; ++i) {
		if (i==247266){
				for (j = edgeStart[i]; j < edgeStart[i + 1] - 1; ++j) {
				int q= j << 1;
			}
		}
		for (j = edgeStart[i]; j < edgeStart[i + 1] - 1; ++j) {
			int q= j << 1;
			insertLineToIndex(i, coordNet[q], coordNet[q+1], coordNet[q+2], coordNet[q+3]);
		}
	}
	
	while (!gridFlag) Sleep(25);
	
	++indexCreateFlag;
	_beginthread( workOdd, 0, NULL );
	_beginthread( workEven, 0, NULL );
	
	_endthread();
}

