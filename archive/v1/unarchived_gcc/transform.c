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
	return (cosTangle(x1, y1, x2, y2) + 1 + 1e-4) * len[r2] / cost[r2];
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		puts("Argument Exception!");
		return 1;
	}
	
	//Load Road Network from File
	loadData(argv[1]);
	
	//Get Input and Output Path
	//getInputPathSet(argv[2]);
	//setOutputPathSet(argv[3]);
	
	
	//输出为可以用的路网
	//有用节点
	int nodeUseful = 0;
	int i;
	for (i = 0; i < nodeNumber; ++i) 
		if (coordNode[i << 1] > latMin2 && coordNode[i << 1] < latMax2 && coordNode[(i << 1)+1] > longMin2 && coordNode[(i << 1)+1] < longMax2) {
			nodeInRegion[i] = 1;
			++nodeUseful;
		}
	/*
	printf("%d\n",nodeUseful);
	for (i = 0; i< nodeNumber; ++i)
		if (nodeInRegion[i])
			printf("%d %.8lf %.8lf\n",i,coordNode[i << 1],coordNode[(i << 1)+1]);
	printf("\n");
	*/
	//有用边
	int edgeUseful = 0; 
	for (i = 0; i < edgeNumber; ++i)
		if (nodeInRegion[thisSide[i]] && nodeInRegion[other[i]]) {
			edgeInRegion[i] = 1;
			++edgeUseful;
		}
	/*
	printf("%d\n",edgeUseful);
	int j;
	for (i = 0; i < edgeNumber; ++i)
		if (edgeInRegion[i]) {
			printf("%d %d\n",thisSide[i],other[i]);
			for (j = edgeStart[i]; j < edgeStart[i+1];++j)
				printf("%.8lf %.8lf ",coordNet[j << 1],coordNet[(j << 1)+1]);
			printf("-1 -1\n");
		}
	*/
	
	
	int stNumber = 51760;
	
	FILE* input = fopen("./Street.txt","r");
	
	int single = 0;
	int countE = 0;
	//printf("0\n%d\n",stNumber);
	//for (i = 0; i < stNumber; ++i){
	while (1) {
		int p;
		fscanf(input,"%d",&p);
		if (p == -1) break;
		//printf("1 1\n");
		
		int rr = 0;
		
		while (1) {
			fscanf(input,"%d",&p);
			if (p == -1) break;
			int j;
			++countE;
			++rr;
			//for (j = edgeStart[p]; j < edgeStart[p+1];++j)
			//	printf("%.8lf %.8lf ",coordNet[j << 1],coordNet[(j << 1)+1]);
		}
		
		if (rr > 1) ++single;
		//printf("-1 -1\n");
	}
	
	fclose(input);
	printf("%d\n",countE);
	printf("%d\n",single);
	
	
	
	
	
	
	/*FILE* input = fopen("./DBTrajectory_Spatial.txt","rb");
	
	int tNumber = readInt(input);
	printf("%d\n",tNumber);
	
	int roadList[10000];
	double timeList[10000];
	
	for (i = 0; i < tNumber; ++i) {
		int nNumber = readInt(input);
		printf("%d %d\n",nNumber,nNumber);
		int j;
		for (j = 0; j < nNumber; ++j) {
			//int timeStamp = readInt(input);
			int road = readInt(input);
			roadList[j] = road;
			timeList[j] = j*100;//(double)timeStamp;
			//printf("%d:%d ",thisSide[road],road);
		}
		timeList[j] = j*100;
		
		for (j = 0; j < nNumber; ++j) {
			int k;
			//int p = last[roadList[j]];
			//while (p > -1) {
			//	if (other[p]==roadList[j+1]) break;
			//	p = pre[p];
			//}
			int p = roadList[j];
			for (k = edgeStart[p]; k < edgeStart[p+1];++k)
				printf("%.8lf %.8lf %d ",coordNet[k << 1],coordNet[(k << 1)+1],(int)(len[p] * 10 / (timeList[j+1]-timeList[j])));
		}
		
		printf("-1 -1\n");
	}
	
	fclose(input);*/
	
	
	
	/*
	FILE* input = fopen("./DBTrajectory_Spatial.txt","rb");
	int tNumber = readInt(input);
	printf("0\n");
	printf("%d\n",tNumber);
	fclose(input);
	FILE* input2 = fopen("./DBTrajectory_Temporal.txt","rb");
	for (i = 0; i < 100; ++i) {
		printf("1 1\n");
		int spNumber = readInt(input2);
		int j;
		for (j = 0; j < spNumber; ++j) {
			double x = readDouble(input2), y = readDouble(input2);
			printf("%.8lf %8lf ",x,y);
		}
		printf("-1 -1\n");
	}
	fclose(input2);
	*/
	
	
	
	
	/*
	resetCellSize();

	//Build Index
	_beginthread(createGirdIndex, 0, NULL);

	//Read Test Files
	int i;
	for (i = 0; i < dataNumber; ++i) {
		loadSequence(i,inPathSet[i]);
		inputLoaded[i] = 1;
	}

	while (doneFlag < 2) Sleep(25);

	outputResult();
	*/
	return 0;
}
