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
int ansList[10000];			//压缩后的轨迹，测试解压用

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];

int save[10000];
void writePath(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i)
		//printf("%d ",save[i]);
		fout << save[i] << " ";
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
				SPLINK[MAP[id]][MAP[q]] = readInt(file);
			}
			//++c;
		}
		//fseek(file,nodeIndex[nodeId][1],0);
		//spSize = readInt(file);
		fclose(file);
	}
	
}

int main(int argc, char *argv[]) {

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
	
	//cout << nodeUseful << endl;
	
	//============Part 1 用最短路压缩轨迹=====================
	//全内存载入最短路
	
	loadSPAll2();
	
	
	
	long bbb = clock();
	//1 读入最短路索引
	FILE* file;
	//2 读取轨迹并处理

	file = fopen(argv[1],"rb");
	
	
	int tjNumber = readInt(file);	//轨迹条数
	cout << tjNumber << endl;
	
	FILE* result = fopen(argv[2],"wb");

	writeInt(result,tjNumber);
	//printf("%d\n",tjNumber);
	
	//cout << "DDD" << endl;
	
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数
		
		ansSize = 0;
		
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//路径上的边
		
		int lastOpt = 0;
		int node = thisSide[path[0]];	//轨迹起始点
		int nowEdge = 0;
		
		while (nowEdge < spNumber) {
			//载入当前位置的最短路
			//loadSP(node);
			
			//如果第一个就不能延伸
			//--
			//printf("\n%d %d\n",spPre[other[path[nowEdge]]],path[nowEdge]);
			if (SPLINK[MAP[node]][MAP[other[path[nowEdge]]]]!=path[nowEdge]) {
				//printf("\n%d %d %d %d\n",spPre[other[path[nowEdge]]],path[nowEdge],node,thisSide[spPre[other[path[nowEdge]]]]);
				//直接用原路径代替
				//printf("01:%d; ",path[nowEdge]);
				//fout << "01:"<< path[nowEdge] <<"; ";
				//ansList[ansSize][0] = 1;
				ansList[ansSize++] = path[nowEdge];
				//++ansSize;
				//{实现二进制写}
				//writeInt(result,(1 << 30) | path[nowEdge]);
				
				lastOpt = 1;
				//下一条边
				node = other[path[nowEdge]];
				++nowEdge;
			}else{
				//找到最短路的最大延伸
				while (nowEdge < spNumber && SPLINK[MAP[node]][MAP[other[path[nowEdge]]]]==path[nowEdge]) 
					++nowEdge;
				--nowEdge;
				if (lastOpt == 0){
					//生成最短路
					//printf("00:%d; ",node);
					//fout<<"00:"<<node<<": ";
					//ansList[ansSize][0] = 0;
					ansList[ansSize++] = node;
					//++ansSize;
					//{实现二进制写}
					//writeInt(result,node);
				}

				lastOpt = 0;
				node = other[path[nowEdge]];
				++nowEdge;
			}
		}
		//最后输出结束点的Id
		//printf("10:%d;\n",other[path[spNumber-1]]);
		//fout << "10:" << other[path[spNumber-1]] << ":"<<endl;
		//ansList[ansSize][0] = 2;
		//ansList[ansSize][1] = other[path[spNumber-1]];
		//{实现二进制写}
		writeInt(result,(1 << 31) | other[path[spNumber-1]]);
		
		

		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(result);
	fclose(file);
	
	return 0;
}
