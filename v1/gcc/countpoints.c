#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
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

int roadSet[6000][100];
double probMul[6000][100];
int preLink[6000][100];

double velocity = 10;

//int tmpList[10000],roadList[10000],timeList[10000],spCount[10000];
//double prob[10000];

char inPath[20000][100];
char inPath2[20000][100];

int path[10000000];
double accDist[1000000];

int ansSize = 0;
int timeList[1000000];//0:时间	1:距离
double distList[1000000];

int dataNumber;
//输入路径
void getInputPathSet(char* path)
{
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath[dataNumber],path);
		strcat(inPath[dataNumber],"/");
		strcat(inPath[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

int main(int argc, char *argv[]){

	if (argc < 3) {
		puts("Argument Exception!");
		return 1;
	}
	
	//原始采样点文件路径
	getInputPathSet(argv[1]);
	int i;
	char tmp[10000];
	int count = 0;
	for (i = 0; i < dataNumber; ++i) {
		if (i % 100 == 0) printf("+");
		FILE* file = fopen(inPath[i],"r");
		while (fscanf(file,"%s",tmp)!=EOF) ++count;
		fclose(file);
	}
	FILE* fp = fopen(argv[2],"a+");
	fprintf(fp,"%d\n",count);
	fclose(fp);
	return 0;
}
