//查询某时刻每条轨迹在哪里，返回坐标对集合

#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>

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


int answerSize;
double answer[100000][3];		//id x,y
int path[100000];
double stamp[100000];

int main(int argc, char *argv[]){
	//时空数据文件
	FILE* st = fopen("./SpatioTemporalMMTC2.txt","rb");
	//结果
	FILE* result = fopen("./SpatioTemporalMMTC_Final.txt","wb");

	readInt(st);
	int i;
	for (i = 0; i < 10000; ++i) {
		printf("%d\t",i);
		int spNumber = readInt(st);
		//printf("%d\t",spNumber);
		int j,k;
		for (j = 1; j < spNumber; ++j) {
			path[j] = readInt(st);
			stamp[j] = readDouble(st);
		}
		for (k = 0; k < 1000; ++k) {
			writeInt(result, spNumber);
			for (j = 1; j < spNumber; ++j) {
				writeInt(result, path[j]);
				writeDouble(result, stamp[j]);
			}
		}
	}
	
	fclose(st);
	fclose(result);
	return 0;
}
