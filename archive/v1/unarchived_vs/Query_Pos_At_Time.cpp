//查询某时刻每条轨迹在哪里，返回坐标对集合

#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

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


//计算几何
/**
 * Get Distance between two points
 */
double circleDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}

//给定相对于某条边的偏移，返回经纬度实数值
void getPosition(int edgeId, double bias, double* resX, double* resY) {
	int i;
	for (i=edgeStart[edgeId];i<edgeStart[edgeId+1];++i){
		double x=coordNet[i << 1],y=coordNet[(i << 1)+1],x2=coordNet[(i << 1)+2],y2=coordNet[(i << 1)+3];
		double dist=circleDistance(x,y,x2,y2);
		if (dist<bias) bias -= dist;
		else {
			double rate = bias / dist;
			*resX = x + (x2 - x) * rate;
			*resY = y + (y2 - y) * rate;
			return;
		}
	}
}

int answerSize;
double answer[100000][3];		//id x,y
int dist[100000],stamp[100000];
int edge[100000];
double acc[100000];

int main(int argc, char *argv[]){

	if (argc < 6) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	char sss[20] = "";
	loadData(sss);
	
	//空间和时间文件
	FILE* spatial = fopen(argv[2],"rb");
	FILE* temporal = fopen(argv[3],"rb");
	
	//输出文件
	FILE* result = fopen(argv[4],"w");
	
	//查询的时间
	char* ttt = argv[5];
	int i;
	int timeStamp = 0;
	for (i = 0; i < strlen(ttt); ++i) timeStamp = timeStamp * 10 + ttt[i] - 48;
	printf("查询时间：%d\n",timeStamp);
	
	long begin = clock();
	
	int traNumber = readInt(temporal);
	readInt(spatial);
	
	answerSize = 0;
	
	for (i = 0; i < traNumber; ++i) {
		//puts(inPath[i]);
		int timeLine = readInt(temporal);
		int j;
		
		for (j = 0; j < timeLine; ++j) { 
			dist[j] = readInt(temporal);
			stamp[j] = readInt(temporal);
		}
		
		int spaLine = readInt(spatial);
		for (j = 0; j < spaLine; ++j) {
			edge[j] = readInt(spatial);
			acc[j] = readDouble(spatial);
		}
		
		if (stamp[0] <= timeStamp && stamp[timeLine - 1] >= timeStamp) {
			int accTime = 0;
			for (j = 0; j < timeLine-1; ++j)
				if (stamp[j] <= timeStamp && stamp[j+1] >= timeStamp) break;
			double relaDist = dist[j] + (dist[j+1] - dist[j])*(timeStamp - stamp[j]) / (stamp[j+1] - stamp[j]);
			for (j = 0; j < spaLine; ++j)
				if (relaDist > acc[j]) relaDist -= acc[j]; else {
					answer[answerSize][0] = i;
					getPosition(edge[j], relaDist, &answer[answerSize][1], &answer[answerSize][2]);
					++answerSize;
					break;
				}
		}
		
		//调试用
		//if (i % 1000 ==999) printf("+");
		//if (i % 10000 ==9999) printf("\n");
	}
	//puts("");
	
	long end = clock();
	printf("用时：%.4lfs\n",(end - begin)/1000.0);
	
	fprintf(result,"%d\n",answerSize);
	for (i = 0; i < answerSize; ++i) 
		fprintf(result,"%d %lf %lf\n",(int)answer[i][0],answer[i][1],answer[i][2]);
	
	fclose(result);
	fclose(spatial);
	fclose(temporal);
	
	return 0;
}
