/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
		之用路网进行划分
*/
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

//============
int nodeIndex[1000000][2];		//0：文件   1：偏移
int spSize;						//到多少个点有最短路
int spPre[1000000];				//每个点的上一条边,-1表示不存在
int path[100000];				//记录原始SPATIAL路径
int ansSize;					//压缩后实体个数
int ansList[10000];			//压缩后的轨迹，测试解压用
//============

int save[10000];
void writePath(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = spPre[node2];
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i)
		printf("%d ",save[i]);
}

int streetNumber;
int road2Street[2000000][2];	//i号路段的街道编号和街道偏移
int street2road[50000][200];	//i号街道j号偏移所对应路段

void loadStreet() {
	memset(street2road,255,sizeof(street2road));
	memset(road2Street,255,sizeof(road2Street));
	FILE* file = fopen("./Street/Street.txt","r");
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

int main(int argc, char *argv[])
{
	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	loadData(argv[1]);
	
	//载入街道信息
	loadStreet();
	
	long bbb = clock();
	
	//1 读入轨迹文件
	FILE* file = fopen(argv[2],"rb");
	
	int tjNumber = readInt(file);	//轨迹条数
	
	FILE* result = fopen(argv[3],"wb");	//用街道编号压缩
	writeInt(result,tjNumber);
	//printf("%d\n",tjNumber);
	int i;
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数
		
		//--
		//printf("%d\n",spNumber);
		ansSize = 0;
		
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//路径上的边
		
		int nowStreet = road2Street[path[0]][0];	//起始道路
		int nowBias = road2Street[path[0]][1];	//起始偏移
		
		//--
		ansList[ansSize] = path[0];
		++ansSize;
		writeInt(result,path[0]);
		
		int nowEdge = 1;
		while (nowEdge < spNumber-1) {
			++nowBias;
			if (street2road[nowStreet][nowBias]!=path[nowEdge]) {
				nowStreet = road2Street[path[nowEdge]][0];	//道路
				nowBias = road2Street[path[nowEdge]][1];	//偏移
				//--
				//ansList[ansSize] = path[nowEdge];
				//++ansSize;
				writeInt(result,path[nowEdge]);			
			}
			++nowEdge;
		}
		
		nowStreet = road2Street[path[nowEdge]][0];	//道路
		nowBias = road2Street[path[nowEdge]][1];	//偏移

		
		writeInt(result,path[nowEdge]);	

		
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(result);
	fclose(file);
	FILE* fpp = fopen(argv[4],"a+");
	fprintf(fpp,"%ld\n",clock() - bbb);
	fclose(fpp);
	return 0;
}
