/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
		之用路网进行划分
*/
#include <stdio.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define myabs(x) ((x)>0?(x):(-(x))) 

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

//读入数据
int timing[50000];
int dist[50000];
int valid[50000];

int main(int argc, char *argv[])
{
	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	
	long bbb = clock();
	
	//1 读入时间轨迹文件
	FILE* temporal = fopen(argv[1],"rb");
	
	int tjNumber = readInt(temporal);	//轨迹条数
	
	FILE* result = fopen(argv[2],"wb");	//压缩结果
	
	writeInt(result,tjNumber);
	
	//时间同步路网距离误差允许的上界
	char* ttt = argv[3];
	int i;
	int tolerance = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance = tolerance * 10 + ttt[i] - 48;
	printf("%d\n",tolerance);
	
	//路网同步时间距离
	ttt = argv[4];
	int tolerance2 = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance2 = tolerance2 * 10 + ttt[i] - 48;
	printf("%d\n",tolerance2);
	
	
	
	int distAll = 0;
	
	double upper, downer;
	
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(temporal);	//路径段数
		
		int j;
		for (j = 0; j < spNumber; ++j) {
			dist[j] = readInt(temporal);	
			timing[j] = readInt(temporal);	
		}
		
		//for (j = 0; j < 20; ++j) printf("%d:[%d %d]          ",j,timing[j]-2678400,dist[j]);puts("");
		
		distAll += dist[spNumber -1];
		
		memset(valid,0,sizeof(valid));
		valid[0] = valid[spNumber - 1] = 1;
		
		int preNode = 0; upper = 1e100, downer = -1e100;
		while (preNode < spNumber - 1) {
			//printf("%d %d\t",preNode, spNumber);
			int flag = 1;
			for (j = preNode + 1; j < spNumber; ++j) {
				double d = (double)(dist[j] - dist[preNode]) / (timing[j] - timing[preNode]);
				if (d < downer || d > upper) {
					flag = 0;break;
				}
				d = (double)(dist[j] - dist[preNode] - tolerance) / (timing[j] - timing[preNode]);
				if (d > downer) downer = d;
				d = (double)(dist[j] - dist[preNode] + tolerance) / (timing[j] - timing[preNode]);
				if (d < upper) upper = d;
				
				d = (double)(dist[j] - dist[preNode])/ (timing[j] - timing[preNode] + tolerance2);
				if (d > downer) downer = d;
				if (timing[j] - timing[preNode] > tolerance2) {
					d = (double)(dist[j] - dist[preNode])/ (timing[j] - timing[preNode] - tolerance2);
					if (d < upper) upper = d;
				}
				/*
				if (upper < downer) {
					flag = 0;
					break;
				}*/
			}
			if (!flag) {
				valid[j-1] = 1;
				preNode = j-1;
				upper = 1e100; downer = -1e100;
			}else break;
		}

		int ansSize = 0;
		for (j = 0; j < spNumber; ++j) if (valid[j]) ++ansSize;
		writeInt(result,ansSize);
		for (j = 0; j < spNumber; ++j) if (valid[j]) {
			writeInt(result,dist[j]);
			writeInt(result,timing[j]);
		}
		//printf("[%d %d]",spNumber,ansSize);
		//puts("");
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	//fclose(result);
	fclose(temporal);
	
	//printf("%d\n",distAll / tjNumber);
	FILE* fpp = fopen(argv[4],"a+");
	fprintf(fpp,"%ld\n",clock() - bbb);
	fclose(fpp);
	
	return 0;
}
