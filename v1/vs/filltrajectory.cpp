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
int ansList[1000][2];			//压缩后的轨迹，测试解压用

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];

void loadSPAll() {
	int t;
	int s;
	int f;
	int c = 0;

	//memset(SPLINK,255,sizeof(SPLINK));
	//SPLINK = (int**)malloc(40000*sizeof(int));
	SPLINK = new int*[30000];

	for (t = 0; t < 30000; ++t)
		//SPLINK[t] = (int*)malloc(40000*sizeof(int));
	{
		SPLINK[t] = new int[30000];
	}
	for (t = 0; t < 30000; ++t)
		for (s = 0; s < 30000; ++s) 
			SPLINK[t][s] = -1;
	char fileName[100]="F:/新加坡轨迹/ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		//--
		//cout << f << endl;

		fileName[29] = (f / 10) + 48;
		fileName[30] = (f % 10) + 48;
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
			//++c;
		}
		//fseek(file,nodeIndex[nodeId][1],0);
		//spSize = readInt(file);
		fclose(file);
	}
	
}

char inPath[20000][100];
char outPath[20000][100];

int save[100000];
void writePath(FILE* fp, int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
		if (save[count] == -1) break;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i)
		//printf("%d ",save[i]);
		fprintf(fp,"%d ",save[i]);
		//cout << save[i] << " ";
}



int main(int argc, char *argv[])
{
	
	//Load Road Network from File
	char sss[20] = "";
	
	loadData(sss);
	
	//===============
	int nodeUseful = 0;
	int i;

	for (i = 0; i < nodeNumber; ++i) 
		{//if (coordNode[i << 1] > latMin2 && coordNode[i << 1] < latMax2 && coordNode[(i << 1)+1] > longMin2 && coordNode[(i << 1)+1] < longMax2) {
			nodeInRegion[i] = 1;
			MAP[i] = nodeUseful;
			++nodeUseful;
		}
	
	//cout << nodeUseful << endl;
	
	//============Part 1 用最短路压缩轨迹=====================
	//全内存载入最短路
	
	loadSPAll();

	int count = 0;
	FILE* fp1 = fopen("./try1.txt","r");
	FILE* fp2 = fopen("./try2.txt","r");
	char a[100];
	while (fscanf(fp1,"%s",a)!=EOF) {
		strcpy(inPath[count++],a);
	}
	for (int i = 0; i < count; ++i)
		fscanf(fp2,"%s",outPath[i]);
	fclose(fp1);fclose(fp2);

	for (int i = 0;i < count; ++i) {
		cout <<i << endl;
		int t,t2 = -1;int r,r2 = -1;

		fp1 = fopen(inPath[i],"r");
		fp2 = fopen(outPath[i],"w");

		while (fscanf(fp1,"%d,%d,1.00",&t,&r)!=EOF) {
			if (r == -1) continue;
			if (t2 == -1) {
				fprintf(fp2,"%d ",r);
				//cout << r << " ";
			}else {
				if (r != r2) {
					//cout << other[r2] << "-" << thisSide[r] << endl; 
					writePath(fp2,other[r2],thisSide[r]);
					fprintf(fp2,"%d ",r);
					//cout << r << " ";
				}
			}
			r2 = r; t2 = t;
		}

		fclose(fp1);fclose(fp2);
	}
	
	return 0;
}
