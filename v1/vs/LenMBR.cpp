/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXN 95000000

using namespace std;

ofstream fout("E:\\TC\\OUTPUT.TXT");

int nodeInRegion[2000000],edgeInRegion[2000000];
//double latMax2 = 47.30329376225394, latMin2 = 46.83389173208538;
//double longMax2 = -121.827392578125, longMin2 = -121.9557373046875;
double latMax2 = 47.86523461231467, latMin2 = 47.427622733649606;
double longMax2 = -122.213037109375, longMin2 =  -122.43576049804688;

union Int2Binary {
	int value;
	unsigned char data[4];
};

union Int2Binary int2Binary;

union Double2Binary {
	double value;
	unsigned char data[8];
};

union Double2Binary double2Binary;

void writeInt(FILE* file, int value) {
	int2Binary.value = value;
	fwrite(int2Binary.data,sizeof(unsigned char), 4,file);
}

int readInt(FILE* file) {
	//unsigned char buf[4];
	if (fread(int2Binary.data,sizeof(unsigned char), 4,file) < 4) return -1;
	return int2Binary.value;
}

void writeDouble(FILE* file, double value) {
	double2Binary.value = value;
	fwrite(double2Binary.data,sizeof(unsigned char), 8,file);
}

double readDouble(FILE* file) {
	if (fread(double2Binary.data,sizeof(unsigned char), 8,file) < 8) return -1;
	return double2Binary.value;
}

//============
int nodeIndex[1000000][2];		//0：文件   1：偏移
int spSize;						//到多少个点有最短路
int spPre[1000000];				//每个点的上一条边,-1表示不存在
int path[100000];				//记录原始SPATIAL路径
int ansSize;					//压缩后实体个数
int ansList[10000];			//压缩后的轨迹，测试解压用

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];
int save[10000];

//计算几何
/**
 * Get Distance between two points
 */
double circleDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}
/**
 *description:计算球面一点到一条路径的距离
 *param: double double 点坐标  int 边标号  double* 最近点距离边的前端点距离  double* 道路长度
 *return:地球面距离，单位米
 */
double nodeToEdgeDistanceAndNodeSide(double nodeX,double nodeY,int edgeId,double *sideLen,double *roadLen){
	int i,j;
	double tmpSideLen=0,tmpRoadLen=0;
	double result=1e80,tmp=0;
	for (i=edgeStart[edgeId];i<edgeStart[edgeId+1];++i){
		double x=coordNet[i << 1],y=coordNet[(i << 1)+1],x2=coordNet[(i << 1)+2],y2=coordNet[(i << 1)+3];
		double dist=circleDistance(x,y,nodeX,nodeY);
		if (dist<result){
			result=dist;
			tmpSideLen=tmp;
		}
		if (i<edgeStart[edgeId+1]-1) {
			double vecX1=x2-x,vecY1=y2-y,
					vecX2=nodeX-x,vecY2=nodeY-y,
					vecX3=nodeX-x2,vecY3=nodeY-y2;
			if (vecX1*vecX2+vecY1*vecY2>0 && -vecX1*vecX3-vecY1*vecY3>0 && (vecX1!=0 || vecY1!=0)){
				double rate=((nodeX-x2)*vecX1+(nodeY-y2)*vecY1)/(-vecX1*vecX1-vecY1*vecY1);
				double nearX=rate*x+(1-rate)*x2,nearY=rate*y+(1-rate)*y2;
				double dist=circleDistance(nearX,nearY,nodeX,nodeY);
				if (dist<result){
					result=dist;
					tmpSideLen=tmp+circleDistance(x,y,nearX,nearY);
				}
			}
			tmpRoadLen+=circleDistance(x,y,x2,y2);
		}
		tmp+=circleDistance(x,y,x2,y2);
	}

	*sideLen=tmpSideLen;
	*roadLen=tmpRoadLen;
	return result;
}

//两路口间路径长度
double pathLen(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);

	double res = 0, sideLen,roadLen;

	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[node1][node2];
		if (save[count] == -1) break;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i) {
		nodeToEdgeDistanceAndNodeSide(0,0,save[i],&sideLen,&roadLen);
		res += roadLen;
	}
	return res;
}

//两路口间mbr
void edgeMBR(int edgeId, double* x1, double* x2, double* y1, double* y2) {
	int i,j;
	double minX = 1e100, minY = 1e100, maxX = -1e100, maxY = -1e100;

	for (i=edgeStart[edgeId];i<edgeStart[edgeId+1];++i){
		double x=coordNet[i << 1],y=coordNet[(i << 1)+1],x2=coordNet[(i << 1)+2],y2=coordNet[(i << 1)+3];
		if (x > maxX) maxX = x;
		if (x < minX) minX = x;
		if (x2 > maxX) maxX = x2;
		if (x2 < minX) minX = x2;
		if (y > maxY) maxY = y;
		if (y < minY) minY = y;
		if (y2 > maxY) maxY = y2;
		if (y2 < minY) minY = y2;
	}
	*x1 = minX;
	*x2 = maxX;
	*y1 = minY;
	*y2 = maxY;
}
void pathMBR(int node1, int node2, double* x1, double* x2, double* y1, double* y2) {
	//printf("\n%d %d\n",node1,node2);
	double mx1, mx2, my1, my2;
	double res = 0, sideLen,roadLen;
	double minX = 1e100, minY = 1e100, maxX = -1e100, maxY = -1e100;
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[node1][node2];
		if (save[count] == -1) break;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i) {
		edgeMBR(save[i], &mx1, &mx2, &my1, &my2);
		if (mx1 < minX) minX = mx1;
		if (mx2 > maxX) maxX = mx2;
		if (my1 < minY) minY = my1;
		if (my2 > maxY) maxY = my2;
	}
	*x1 = minX;
	*x2 = maxX;
	*y1 = minY;
	*y2 = maxY;
}

void loadSPAll2() {
	int t;
	int s;
	int f;
	int c = 0;

	//memset(SPLINK,255,sizeof(SPLINK));
	//SPLINK = (int**)malloc(40000*sizeof(int));
	SPLINK = new int*[30000];

	for (t = 0; t < 30000; ++t) {
		SPLINK[t] = new int[30000];
	}
	for (t = 0; t < 30000; ++t)
		for (s = 0; s < 30000; ++s) 
			SPLINK[t][s] = -1;
	char fileName[100]="./ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		//--
		//cout << f << endl;

		fileName[17] = (f / 10) + 48;
		fileName[18] = (f % 10) + 48;
		cout << fileName << endl;
		//--
		//printf("%d %s\n",nodeId,fileName);
		FILE* file = fopen(fileName,"rb");
		
		//点数
		int number = readInt(file);
		int j;
		for (j = 0; j < number; ++j) {
			//id
			int id = readInt(file);
			//MAP[id] = c;
			//个数
			int many = readInt(file);
			int i;
			//memset(spPre,255,sizeof(spPre));
			for (i = 0; i < many; ++i){
				int q = readInt(file);
				SPLINK[id][q] = readInt(file);
			}
			//++c;
		}
		//fseek(file,nodeIndex[nodeId][1],0);
		//spSize = readInt(file);
		fclose(file);
	}
	
}

int* son, *righting, *value, counting, *father;
double *trielen,*mbr1,*mbr2,*mbr3,*mbr4;
int e = 0,p,q;

int main(int argc, char *argv[]) {

	//Load Road Network from File
	char sss[20] = "";
	
	//读入路网
	loadData(sss);
	
	//===============
	int nodeUseful = 0;
	int i;

	
	//============Part 1 用最短路压缩轨迹=====================
	//全内存载入最短路
	
	//最短路表
	loadSPAll2();
	
	//读入Trie
	FILE* fp = fopen(argv[1],"r");

	son = (int*)malloc(MAXN*sizeof(int));
	righting = (int*)malloc(MAXN*sizeof(int));
	value = (int*)malloc(MAXN*sizeof(int));
	father = (int*)malloc(MAXN*sizeof(int));
	mbr1 = (double*)malloc(MAXN*sizeof(double));
	mbr2 = (double*)malloc(MAXN*sizeof(double));
	mbr3 = (double*)malloc(MAXN*sizeof(double));
	mbr4 = (double*)malloc(MAXN*sizeof(double));
	trielen = (double*)malloc(MAXN*sizeof(double));

	fscanf(fp,"%d",&e);
	for (int i = 0; i <= e; ++i)
		fscanf(fp,"%d%d%d%d%d",&son[i],&righting[i],&father[i],&value[i],&counting);
	fclose(fp);

	
	//计算Len与MBR
	trielen[0] = 0;
	//1: minx    2:maxx    3:miny    4:maxy
	mbr1[0] = mbr3[0] = 1e100;
	mbr2[0] = mbr4[0] = -1e100;
	double sideLen, roadLen,x1,x2,y1,y2;
	for (int i = 1; i <=e; ++i) {
		if (father[i] == 0) {
			nodeToEdgeDistanceAndNodeSide(0,0,value[i],&sideLen,&roadLen);
			trielen[i] = roadLen;
			edgeMBR(value[i], &mbr1[i], &mbr2[i], &mbr3[i], &mbr4[i]);
		}else {
			nodeToEdgeDistanceAndNodeSide(0,0,value[i],&sideLen,&roadLen);
			trielen[i] = trielen[father[i]] + roadLen + pathLen(other[value[father[i]]],thisSide[value[i]]);

			edgeMBR(value[i], &mbr1[i], &mbr2[i], &mbr3[i], &mbr4[i]);
			pathMBR(other[value[father[i]]],thisSide[value[i]], &x1, &x2, &y1, &y2);
			if (x1 < mbr1[i]) mbr1[i] = x1;
			if (x2 > mbr2[i]) mbr2[i] = x2;
			if (y1 < mbr3[i]) mbr3[i] = y1;
			if (y2 > mbr4[i]) mbr4[i] = y2;
			if (mbr1[father[i]] < mbr1[i]) mbr1[i] = mbr1[father[i]];
			if (mbr2[father[i]] > mbr2[i]) mbr2[i] = mbr2[father[i]];
			if (mbr2[father[i]] < mbr3[i]) mbr3[i] = mbr3[father[i]];
			if (mbr2[father[i]] > mbr4[i]) mbr4[i] = mbr4[father[i]];
		}
	}
	
	for (int i = 0; i <= e; ++i)
		printf("%.4lf\t\t%.4lf %.4lf %.4lf %.4lf\n", trielen[i], mbr1[i],mbr2[i],mbr3[i],mbr4[i]);

	return 0;
}
