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
int time[50000];
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
	FILE* spatial = fopen(argv[1],"rb");
	FILE* temporal = fopen(argv[2],"rb");
	
	int tjNumber = readInt(spatial);	//轨迹条数
	readInt(temporal);
	
	int i;
	int out = 0;
	
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(temporal);	//路径段数
		out += spNumber;
		int j;
		for (j = 0; j < spNumber; ++j) {
			dist[j] = readInt(temporal);	
			time[j] = readInt(temporal);	
		}
		
		spNumber = readInt(spatial);	//路径段数
		out += spNumber;

		for (j = 0; j < spNumber; ++j) {
			readInt(spatial);
		}
		
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	//fclose(result);
	fclose(temporal);
	fclose(temporal);
	
	//printf("%d\n",distAll / tjNumber);
	FILE* fpp = fopen(argv[3],"a+");
	fprintf(fpp,"%d\n",out);
	fclose(fpp);
	
	return 0;
}
