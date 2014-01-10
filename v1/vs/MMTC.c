/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
*/
#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

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

//载入某个点的最短路
void loadSP(int nodeId) {
	char fileName[30]="./ShortestPath/SP00.txt";
	fileName[17] = (nodeIndex[nodeId][0] / 10) + 48;
	fileName[18] = (nodeIndex[nodeId][0] % 10) + 48;
	//--
	//printf("%d %s\n",nodeId,fileName);
	FILE* file = fopen(fileName,"rb");
	fseek(file,nodeIndex[nodeId][1],0);
	spSize = readInt(file);
	int i;
	memset(spPre,255,sizeof(spPre));
	for (i = 0; i < spSize; ++i){
		int q = readInt(file);
		spPre[q] = readInt(file);
	}
	fclose(file);
}

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

int main(int argc, char *argv[])
{
	if (argc < 2) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	loadData(argv[1]);
	
	//============Part 1 用最短路压缩轨迹=====================
	//1 读入最短路索引
	FILE* file = fopen("./ShortestPath/SPIndex.txt","r");
	int nodeNumber;
	fscanf(file,"%d",&nodeNumber);
	int i;
	for (i = 0; i < nodeNumber; ++i) {
		int nodeId;
		fscanf(file, "%d", &nodeId);
		fscanf(file,"%d %d",&nodeIndex[nodeId][0],&nodeIndex[nodeId][1]);
		//--
		//printf("%d %d %d\t",nodeId,nodeIndex[nodeId][0],nodeIndex[nodeId][1]);
	}
	fclose(file);
	//2 读取轨迹并处理
	file = fopen("./MapMatchedTrajectory/DBTrajectory_Spatial_High_Sample_Rate.txt","rb");
	
	int tjNumber = readInt(file);	//轨迹条数
	
	FILE* result = fopen("./CompressedTrajectory/Spatial_High_Sample_Rate_SP.txt","wb");	//用最短路径压缩
	writeInt(result,tjNumber);
	//printf("%d\n",tjNumber);
	
	for (i = 1; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数
		
		//--
		//printf("%d\n",spNumber);
		ansSize = 0;
		
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//路径上的边
		
		//--
		//for (j = 0; j < spNumber; ++j)
		//	printf("%d ",path[j]);	//路径上的边
		//printf("\n");
		
		int lastOpt = 0;
		int node = thisSide[path[0]];	//轨迹起始点
		int nowEdge = 0;
		while (nowEdge < spNumber) {
			//载入当前位置的最短路
			loadSP(node);
			
			//如果第一个就不能延伸
			//--
			//printf("\n%d %d\n",spPre[other[path[nowEdge]]],path[nowEdge]);
			if (spPre[other[path[nowEdge]]]!=path[nowEdge]) {
				//printf("\n%d %d %d %d\n",spPre[other[path[nowEdge]]],path[nowEdge],node,thisSide[spPre[other[path[nowEdge]]]]);
				//直接用原路径代替
				//printf("01:%d; ",path[nowEdge]);
				//ansList[ansSize][0] = 1;
				//ansList[ansSize][1] = path[nowEdge];
				//++ansSize;
				//{实现二进制写}
				writeInt(result,(1 << 30) | path[nowEdge]);
				lastOpt = 1;
				//下一条边
				node = other[path[nowEdge]];
				++nowEdge;
			}else{
				//找到最短路的最大延伸
				while (nowEdge < spNumber && spPre[other[path[nowEdge]]]==path[nowEdge]) 
					++nowEdge;
				--nowEdge;
				if (lastOpt == 0){
					//生成最短路
					//printf("00:%d; ",node);
					//ansList[ansSize][0] = 0;
					//ansList[ansSize][1] = node;
					//++ansSize;
					//{实现二进制写}
					writeInt(result,node);
				}

				lastOpt = 0;
				node = other[path[nowEdge]];
				++nowEdge;
			}
		}
		//最后输出结束点的Id
		//printf("10:%d;\n",other[path[spNumber-1]]);
		//ansList[ansSize][0] = 2;
		//ansList[ansSize][1] = other[path[spNumber-1]];
		//{实现二进制写}
		writeInt(result,(1 << 31) | other[path[spNumber-1]]);
		/*
		//解压轨迹测试
		//--
		for (j = 0; j <=ansSize; ++j){
			if (j > 0 && ansList[j-1][0] == 1) {
				int node1 = other[ansList[j-1][1]],node2;
				if (ansList[j][0] !=1 ) node2 = ansList[j][1];else node2 = thisSide[ansList[j][1]];
				if (node1!=node2) {
					loadSP(node1);
					writePath(node1,node2);
				}
			}
			if (j<ansSize){
				if (ansList[j][0] == 1) {
					printf("%d ",ansList[j][1]);
				}else if (ansList[j][0] == 0)	{
					int node1,node2;
					node1 = ansList[j][1];
					if (ansList[j+1][0] != 1) node2 = ansList[j+1][1];
					else node2 = thisSide[ansList[j+1][1]];
					loadSP(node1);
					writePath(node1,node2);
				}
			}
		}
		printf("\n");
		*/
		
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(result);
	fclose(file);
	//============Part 2 用Street压缩轨迹=====================
	
	//============Part 3 用频繁子轨迹压缩轨迹=================
	
	//============Part 4 综合三者压缩轨迹，DP最小体积=========

	
	//Load Road Network from File
	loadData(argv[1]);

	
	
	return 0;
}
