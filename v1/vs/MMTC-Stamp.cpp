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

using namespace std;

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


//============
int nodeIndex[1000000][2];		//0：文件   1：偏移
int spSize;						//到多少个点有最短路
int spPre[1000000];				//每个点的上一条边,-1表示不存在
int path[100000];				//记录原始SPATIAL路径
int ansSize;					//压缩后实体个数
int ansList[100000][3];			//压缩后的轨迹，测试解压用

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];
double accLen[1000000];

int save[10000];
void writePath(double * result, int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
		if (save[count] == -1) return;
		node2 = thisSide[save[count]];
		++count;
	}
	double sideLen, roadLen;
	int i;
	for (i = count-1; i >=0; --i) {
		//printf("%d ",save[i]);
		int road = save[i];
		nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
		//printf("%d ",road);
		*result += roadLen;
	}
		//fout << save[i] << " ";
}

//最短路
void loadSPAll2() {
	int t;
	int s;
	int f;
	int c = 0;

	SPLINK = new int*[30000];

	for (t = 0; t < 30000; ++t){
		SPLINK[t] = new int[30000];
	}
	for (t = 0; t < 30000; ++t)
		for (s = 0; s < 30000; ++s) 
			SPLINK[t][s] = -1;
	char fileName[100]="F:/新加坡轨迹/ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		fileName[29] = (f / 10) + 48;
		fileName[30] = (f % 10) + 48;
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
				SPLINK[MAP[id]][MAP[q]] = readInt(file);
			}
		}
		fclose(file);
	}
	
}

//街道
int streetNumber;
int road2Street[2000000][2];	//i号路段的街道编号和街道偏移
int street2road[50000][200];	//i号街道j号偏移所对应路段

void loadStreet() {
	memset(street2road,255,sizeof(street2road));
	memset(road2Street,255,sizeof(road2Street));
	FILE* file = fopen("D:/temp/Street/Street.txt","r");
	while (1) {
		int number;
		fscanf(file,"%d",&number);
		if (number == -1) break;

		int q = 0;
		while (1) {
			int p;
			fscanf(file,"%d",&p);
			street2road[number][q] =p;
			road2Street[p][0]=number;
			road2Street[p][1] = q;
			++q;
			if (p ==-1) break;
		}
	}
	fclose(file);
}

//频繁轨迹
int fre_pre[200000], fre_last[60000], fre_other[200000][2];//0:哪个子轨迹 1：子轨迹中第几个项
int fre_number, item = 0;

//0：在哪个子轨迹 1：起始偏移 2：终止偏移
int tmp_item[200000][3],tmp_mm[200000][3];
int tmp_num, tmp_num2;
int fre[10000][200];

void loadFrequency() {
	FILE* file = fopen("D:/temp/Subtrajectory/sub_final.txt","r");
	memset(fre_last,255,sizeof(fre_last));
	fscanf(file,"%d",&fre_number);
	int i;
	for (i = 0; i < fre_number; ++i) {
		int num;
		fscanf(file,"%d",&num);
		int j,e;
		for (j = 0; j < num; ++j) {
			fscanf(file, "%d",&e);
			fre[i][j] = e;
			fre_other[item][0] = i;
			fre_other[item][1] = j;
			fre_pre[item] = fre_last[e];
			fre_last[e] = item;
			++item;
		}
	}
	
	fclose(file);
}

int main(int argc, char *argv[])
{
	
	if (argc < 3) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	char sss[20] = "";
	
	loadData(sss);
	
	//===============
	int nodeUseful = 0;
	int i;

	for (i = 0; i < nodeNumber; ++i) {
		nodeInRegion[i] = 1;
		MAP[i] = nodeUseful;
		++nodeUseful;
	}
	
	//============载入相关文件=====================
	//全内存载入最短路
	loadSPAll2();
	//载入街道编号
	loadStreet();
	//载入频繁轨迹
	loadFrequency();


	//2 读取轨迹并处理
	FILE* file = fopen(argv[1],"rb");
	
	
	int tjNumber = readInt(file);	//轨迹条数
	cout << tjNumber << endl;

	FILE* result = fopen(argv[2],"wb");

	writeInt(result,tjNumber);
	
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数

		//读入这条压缩后道路
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//路径上的边
		
		/*
		//writeInt(result,ansSize);
		for (int j = 0; j < ansSize; ++j) {
			if (ansList[j][0] == 1) {
				writeInt(result,(1 << 30) | (ansList[j][1] % 32768)*32768 + (ansList[j][2] % 32768));
			}
			if (ansList[j][0] == 2) {
				writeInt(result,(2 << 30) | (ansList[j][1] % 32768)*32768 + (ansList[j][2] % 32768));
			}
			if (ansList[j][0] == 3) {
				writeInt(result,ansList[j][1]);
			}
		}*/
		
		//for (j = 0; j < spNumber; ++j) printf("%d\t",path[j]);puts("");
		ansSize = spNumber;

		double sideLen, roadLen;

		for (j = 0; j < spNumber; ++j) {
			accLen[j] = 0;

			ansList[j][0] = (path[j] >> 28) & 15;
			if (ansList[j][0] == 15) ansList[j][0] = 2; else
			if (ansList[j][0] == 14) ansList[j][0] = 3; else
				ansList[j][0] = 1;

			if (ansList[j][0] == 1) {
				ansList[j][1] = (path[j] >> 16) & 65535;
				ansList[j][2] = path[j] & 65535;
			}
			if (ansList[j][0] == 2) {
				ansList[j][1] = (path[j] >> 8) & 65535;
				ansList[j][2] = path[j] & 255;
			}
			if (ansList[j][0] == 3) {
				ansList[j][1] = path[j] & ((1 << 27)-1);
			}
			//printf("[%d %d %d]\t",ansList[j][0],ansList[j][1],ansList[j][2]);
			
			if (ansList[j][0] == 3) {
				//printf("[3] ");
				int left = (ansList[j][1] >> 7) & 127;
				int right = ansList[j][1] & 127;
				for (int k = left; k <= right; ++k) {
					//printf("%d ",fre[(ansList[j][1] >> 14 ) & 32767][k]);
					int road = fre[(ansList[j][1] >> 14 ) & 32767][k];
					nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
					//printf("%d ",road);
					accLen[j] += roadLen;
				}
			}
			if (ansList[j][0] == 1) {
				//printf("[1] ");
				//printf("%d ",ansList[j][1]);
				int road = ansList[j][1];
				nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
					//printf("%d ",road);
				accLen[j] += roadLen;
				writePath(&accLen[j],other[ansList[j][1]],other[ansList[j][2]]);
			}
			if (ansList[j][0] == 2) {
				//printf("[2] ");
				int nowStreet = road2Street[ansList[j][1]][0];	//起始道路
				int nowBias = road2Street[ansList[j][1]][1];	//起始偏移
				for (int k = 0; k< ansList[j][2]; ++k) {
					int road = street2road[nowStreet][nowBias];
					nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
					//printf("%d ",road);
					accLen[j] += roadLen;
					++nowBias;
				}
			}
		}

		//for (j = 1; j < spNumber; ++j) accLen[j] += accLen[j-1];
		//for (j = 0; j < spNumber; ++j) printf("[%lf]\t",accLen[j]);
		//puts("");
		writeInt(result,spNumber);
		for (j = 0; j < spNumber; ++j) {
			writeInt(result,path[j]);
			writeDouble(result,accLen[j]);
		}
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(result);
	fclose(file);
	
	return 0;
}
