/**
	计算任意两点间最短路，并输出为一个二进制文件 每1024个点一个文件，SP+编号0~38+.txt
		文件格式：
		本文件点数
		点标号1 存在唯一最短路点个数1 到点1编号 最短路的上一条边 到点2编号 最短路的上一条边……
		点标号2 存在唯一最短路点个数1 到点1编号 最短路的上一条边 到点2编号 最短路的上一条边……
		……
		如果一对点不存在，证明最短路不唯一
	
	将路网进行分割
		文件格式：
		道路标号1 路段Id1 路段Id2…… -1
		道路标号2 路段Id1 路段Id2…… -1
		……
		-1
*/

#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/**
 * Output Result
 */
void outputResult()
{
	int tt, i;
	FILE *fp;
	for (tt = 0; tt < dataNumber; ++tt) {
		fp = fopen(outPathSet[tt], "w");
		for (i = 0; i < nowTime[tt]; ++i)
			fprintf(fp, "%d,%d,1.00\n", timeSequence[tt][i], ans[tt][i]);
		fclose(fp);
	}
}

int nodeInRegion[2000000],usefulList[2000000],edgeInRegion[2000000];
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


//两向量余弦
double cosTangle(double x1, double y1, double x2, double y2) {
	return (x1*x2+y1*y2)/(sqrt(x1*x1+y1*y1)*sqrt(x2*x2+y2*y2));
}

//两条道路的夹角Cosin
double cosRoad(int r1, int r2) {
	double x1 = coordNode[other[r1]*2]-coordNode[thisSide[r1]*2];
	double y1 = coordNode[other[r1]*2+1]-coordNode[thisSide[r1]*2+1];
	double x2 = coordNode[other[r2]*2]-coordNode[thisSide[r2]*2];
	double y2 = coordNode[other[r2]*2+1]-coordNode[thisSide[r2]*2+1];
	return cosTangle(x1, y1, x2, y2);
}

//连续两线段的夹角Cosin
double cosPoint(int p1, int p2, int p3) {
	double x1 = coordNode[p2*2]-coordNode[p1*2];
	double y1 = coordNode[p2*2+1]-coordNode[p1*2+1];
	double x2 = coordNode[p3*2]-coordNode[p2*2];
	double y2 = coordNode[p3*2+1]-coordNode[p2*2+1];
	return cosTangle(x1, y1, x2, y2);
}

//两条路同轨迹的概率公式
double direction(int r1, int r2) {
	double x1 = coordNode[other[r1]*2]-coordNode[thisSide[r1]*2];
	double y1 = coordNode[other[r1]*2+1]-coordNode[thisSide[r1]*2+1];
	double x2 = coordNode[other[r2]*2]-coordNode[thisSide[r2]*2];
	double y2 = coordNode[other[r2]*2+1]-coordNode[thisSide[r2]*2+1];
	return (cosTangle(x1, y1, x2, y2) + 1 + 1e-4) * len[r2] / cost[r2];
}

#define USEFUL 40000
#define NODESIZE 550000
int head,tail;				//队首队尾

int nodeInQueue[NODESIZE];	//每个点是否在队列里
int multi[NODESIZE];			//最短路是否唯一 0表示惟一，1表示不唯一
double length[NODESIZE];		//最短路长度
int father[NODESIZE];			//更新距离的边

int result[1024][USEFUL];	//结果
int queue[USEFUL];			//队列

//===============
int streetNumber = 0;	//街道条数
int split[2000000];		//道路被划分到哪个街道，没划分是0
int endpoint1[2000000],endpoint2[2000000];//街道的两头的路段
int next[150000],indi[150000];	//同道路下一个，是否独立	
int edgeUsefulList[200000];

void outStreet(int stId) {
	int rd = endpoint1[stId];
	printf("%d ",rd);
	while (rd!=endpoint2[stId]) {
		int p = last[other[rd]];
		while (p > -1){
			if (split[p] == stId) {
				rd = p;break;
			}
			p = pre[p];
		}
		printf("%d ",rd);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		puts("Argument Exception!");
		return 1;
	}
	
	//Load Road Network from File
	loadData(argv[1]);
	
	//输出为可以用的路网
	//有用节点 记录点的Id
	int nodeUseful = 0;
	int i;
	for (i = 0; i < nodeNumber; ++i) 
		if (coordNode[i << 1] > latMin2 && coordNode[i << 1] < latMax2 && coordNode[(i << 1)+1] > longMin2 && coordNode[(i << 1)+1] < longMax2) {
			nodeInRegion[i] = 1;
			usefulList[nodeUseful] = i;
			++nodeUseful;
		}

	//有用边
	int edgeUseful = 0; 
	for (i = 0; i < edgeNumber; ++i)
		if (nodeInRegion[thisSide[i]] && nodeInRegion[other[i]]) {
			edgeInRegion[i] = 1;
			edgeUsefulList[edgeUseful] = i;
			++edgeUseful;
		}
	
	//printf("%d\n",edgeUseful);
	//先用类似FloodFill方法划分道路
	for (i = 0; i < edgeUseful; ++i)
		//未分割
		if (!split[edgeUsefulList[i]]) {
			//新的街道
			++streetNumber;
			split[edgeUsefulList[i]] = streetNumber;
			//街道起点
			endpoint1[streetNumber] = edgeUsefulList[i];
			//临时变量
			int tmpRoad = edgeUsefulList[i];
			//染色
			int flag = 1;
			while (flag) {
				flag = 0;
				int p = last[other[tmpRoad]];
				
				double mid = 0.1;
				
				while (p > -1) {
					//找在范围内，未被划分的道路
					if (edgeInRegion[p] && !split[p]) {
						double tangle = cosPoint(thisSide[edgeUsefulList[i]],thisSide[p],other[p]);
						//找角度最小的
						if (tangle > mid) {
							mid = tangle;
							flag = 1;
							tmpRoad = p;
						}
					}
						//道路的拐弯不超过60度，与道路起始位置的转角不超过80度
						//if (cosRoad(tmpRoad,p) > 0.5 && cosRoad(edgeUsefulList[i],p)> 0.173648) {
						//	flag = 1;split[p] = streetNumber; tmpRoad = p; break;
						//}
					p = pre[p];
				}
				
				//--
				if (flag) {
					split[tmpRoad] = streetNumber;
				}
			}
			//街道终点
			endpoint2[streetNumber] = tmpRoad;
		}
	
	int j;
	
	memset(next,0,sizeof(next));
	memset(indi,0,sizeof(indi));
	//再进行道路的合并
	for (i =1;i <= streetNumber; ++i) if (next[i] == 0){
		double mid = 0.2;
		int rec = -1;
		//不同的轨迹，j没被插过
		for (j = 1; j <= streetNumber; ++j) if (i!=j && !indi[j]) {
			if (other[endpoint2[i]] == thisSide[endpoint1[j]]) {
				double tangle = cosPoint(thisSide[endpoint1[i]],other[endpoint2[i]],other[endpoint2[j]]);
				if (tangle > mid){
					mid = tangle;
					rec = j;
				}
			}
		}
		if (rec > -1){
			next[i] = rec;indi[rec] = i;
		}
	}
	
	int stCount = 0;
	for (i =1;i <= streetNumber; ++i)
		if (!indi[i]) {
			++stCount;
			printf("%d ",stCount);
			int tmpStreet = i;
			outStreet(tmpStreet);
			while (next[tmpStreet]) {
				tmpStreet = next[tmpStreet];
				outStreet(tmpStreet);
			}
			printf("-1\n");
		}
	printf("-1\n");
	/*
	//SPFA对每个有用点计算单源最短路
	char fileName[30]="./ShortestPath/SP00.txt";
	FILE* file = fopen(fileName,"wb");
	
	if (nodeUseful >1023) writeInt(file,1024);
	else writeInt(file,nodeUseful);
	
	int count = 0,fn = 0;
	
	for (i = 0; i < nodeUseful; ++i) {
		
		//预处理
		int src = usefulList[i];
		memset(nodeInQueue,0,sizeof(nodeInQueue));
		memset(multi,0,sizeof(multi));
		int j;
		for (j = 0; j < nodeUseful; ++j) length[usefulList[j]] = 1e100;
		length[src] = 0;
		head = 0, tail = 1;
		queue[1] = src;nodeInQueue[src] = 1;
		father[src] = -1;

		//Modified SPFA
		
		while (head!=tail) {
			head = (head+1) % USEFUL;
			src = queue[head];
			int p = last[src];
			while (p > -1) {
				int ylq = other[p];
				
				//如果不在路网需要范围内，直接跳过
				if (!nodeInRegion[ylq]) {
					p = pre[p];
					continue;
				}
				
				//是否需要加入队列
				int flag = 0;
				
				//长度可以更新
				if (length[src]+len[p] < length[ylq]) {
					father[ylq] = p;
					length[ylq] = length[src]+len[p];
					if (!nodeInQueue[ylq]) flag = 1;
					multi[ylq] = multi[src];
				}else
				//长度相等
				if (length[src]+len[p] == length[ylq]) {
					//之前是单条最短路径，现在看来显然不是
					if (!multi[ylq]) {
						multi[ylq] = 1;
						if (!nodeInQueue[ylq]) flag = 1;
					}
				}
				
				if (flag) {
					tail = (tail + 1) % USEFUL;
					queue[tail] = ylq;
					nodeInQueue[ylq] = 1;
				}
				
				p = pre[p];
			}
			nodeInQueue[src] = 0;
		}
		
		//源点
		writeInt(file, usefulList[i]);
		//统计最短路惟一个数
		int single = 0;
		for (j = 0; j < nodeUseful; ++j)
			if (!multi[usefulList[j]] && length[usefulList[j]] < 1e20) ++single;
		//输出
		writeInt(file,single);
		//if (single < 39448) {
		//printf("%d: ",single);
		//输出每个最短路惟一的点及其前驱道路
		for (j = 0; j < nodeUseful; ++j)
			if (!multi[usefulList[j]] && length[usefulList[j]] < 1e20) {
				writeInt(file,usefulList[j]);
				writeInt(file,father[usefulList[j]]);
				//printf("%d %d; ",usefulList[j],father[usefulList[j]]);
				//if (length[usefulList[j]] < 100)
				//printf("%.1lf ",length[usefulList[j]]);
			}

		//每1024个点换一个文件记录
		++count; 
		if (count > 1023){
			count = 0;
			fclose(file);
			++fn;
			fileName[17] = (fn / 10) + 48;
			fileName[18] = (fn % 10) + 48;
			file = fopen(fileName,"wb");
			
			if (nodeUseful - i - 1 >1023) writeInt(file,1024);
			else writeInt(file,nodeUseful - i - 1);
			
			printf("\t%d DONE\n",fn);
		}
		if (i % 32 == 0) printf("+");
	}
	
	*/
	
	
	
	return 0;
}
