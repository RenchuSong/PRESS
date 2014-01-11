#include <stdio.h>
#include <time.h>
#include "define.h"
#include "geometry.h"
#include "file.h"
#include "index.h"
#include "core.h"
#include <dirent.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <math.h>

void getInputPathSet(char* path)
{
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPathSet[dataNumber],path);
		strcat(inPathSet[dataNumber],"/");
		strcat(inPathSet[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

void setOutputPathSet(char* path)
{
	int i,j,k;
	for (i=0;i<dataNumber;++i){
		strcpy(outPathSet[i],path);
		strcat(outPathSet[i],"/output");
		int len=strlen(inPathSet[i]);
		for (j=len-1;j>0;--j) if (inPathSet[i][j]=='_') break;
		int len2=strlen(outPathSet[i]);
		for (k=j;k<len;++k) outPathSet[i][len2+k-j]=inPathSet[i][k];
		outPathSet[i][len2+k-j]=0;
	}
}

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
	/*unsigned char buf[4];
	buf[0] = value % 256;
	value /= 256;
	buf[1] = value % 256;
	value /= 256;
	buf[2] = value % 256;
	value /= 256;
	buf[3] = value % 256;
	*/
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

//两条路同轨迹的概率公式
double direction(int r1, int r2) {
	double x1 = coordNode[other[r1]*2]-coordNode[thisSide[r1]*2];
	double y1 = coordNode[other[r1]*2+1]-coordNode[thisSide[r1]*2+1];
	double x2 = coordNode[other[r2]*2]-coordNode[thisSide[r2]*2];
	double y2 = coordNode[other[r2]*2+1]-coordNode[thisSide[r2]*2+1];
	//printf("%.4lf %d\n",len[r2],cost[r2]);
	return ((cosTangle(x1, y1, x2, y2) + 1) + 1e-4) * len[r2] / cost[r2];
}

int roadSet[6000][100];
double probMul[6000][100];
int preLink[6000][100];

double velocity = 10;
//acceleration = 20;
//速度为道路速度限制的20%~90%随机，并且与上一道路平均速度相差不超过20%，除非突破速度限制
void updateVelocity(int edgeId) {
	//根据上一个道路的速度随机增减20%
	velocity = velocity + (double)rand()/RAND_MAX *(velocity*2 / 5) - velocity / 5;
	if (velocity < len[edgeId]*100 / cost[edgeId]/5) velocity = (len[edgeId]*100 / cost[edgeId]/5);
	if (velocity > len[edgeId]*100 / cost[edgeId]*9/10) velocity = (len[edgeId]*100 / cost[edgeId]*9/10);
	//printf("%.4lf\t",velocity);
}

int tmpList[10000],roadList[10000],timeList[10000],spCount[10000];
double prob[10000];

char inPath[20000][100];
char outPath[20000][100];

int path[1000000];

int main(int argc, char *argv[])
{
	if (argc < 2) {
		puts("Argument Exception!");
		return 1;
	}
	
	//Load Road Network from File
	loadData(argv[1]);
	
	
	//根据地图匹配轨迹生成Spatial轨迹数据文件
	FILE* out = fopen("./DBTrajectory_Spatial.txt","wb");
	//FILE* out2 = fopen("./DBTrajectory_Temporal.txt","wb");

	int count = 0;
	FILE* fp1 = fopen("F:/新加坡轨迹/try2.txt","r");
	//FILE* fp2 = fopen("F:/新加坡轨迹/try2.txt","r");
	
	char a[1000];
	while (fscanf(fp1,"%s",a)!=EOF) {
		puts(a);
		strcpy(inPath[count++],a);
		puts(a);
	}
	//for (int i = 0; i < count; ++i)
	//	fscanf(fp2,"%s",outPath[i]);
	fclose(fp1);//fclose(fp2);

	writeInt(out,count);
	printf("%d\n",count);
	
	int i,j;
	//printf("%d\n",RAND_MAX);
	for (i = 0; i < count; ++i) {
		//printf("1111%d\n",i);
		int spNumber = 0;
		fp1 = fopen(inPath[i],"r");
		while (fscanf(fp1,"%d",&path[spNumber++])!=EOF);
		
		//printf("8888%d\n",i);
		//输出序列段个数
		writeInt(out,spNumber);
		
		//采样点个数
		int number = 0;
		
		//输出采样序列
		for (j = 0; j < spNumber; ++j) {
			//输出道路
			writeInt(out,path[j]);
		}
		
		/*
		writeInt(out2,number);
		double dist = 0;
		for (j = 0; j < spNumber; ++j) {
			double dt,dd;
			if (spCount[j] > 0){
				dt = (timeList[j+1]-timeList[j]) / (spCount[j]+1);
				dd = len[roadList[j]] / (spCount[j]+1);
			}
			int k;
			double ft = timeList[j];
			double fd = dist;
			for (k = 1; k <= spCount[j]; ++k){
				ft += dt + dt*0.2*rand()/RAND_MAX;
				fd += dd + dd*0.2*rand()/RAND_MAX;
				writeDouble(out2,fd);
				writeDouble(out2,ft);
			}
			dist += len[roadList[j]];
		}*/
	}
	
	fclose(out);
	//fclose(out2);
	
	return 0;
}
