//查询某时刻每条轨迹在哪里，返回坐标对集合

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

/**
 *description:计算球面一点到一条路径的距离
 *param: double double 点坐标  int 边标号  double* 最近点距离边的前端点距离  double* 道路长度
 *return:地球面距离，单位米
 */
double nodeToEdgeDistanceAndNodeSide(double nodeX,double nodeY,int edgeId,double *sideLen,double *roadLen){
	int i,j;
	double tmpSideLen=0,tmpRoadLen=0;
	double result=1e80,tmp=0;
	for (i=edgeStart[edgeId];i<edgeStart[edgeId+1];++i){
		double x=coordNet[i << 1],y=coordNet[(i << 1)+1],x2=coordNet[(i << 1)+2],y2=coordNet[(i << 1)+3];
		double dist=circleDistance(x,y,nodeX,nodeY);
		if (dist<result){
			result=dist;
			tmpSideLen=tmp;
		}
		if (i<edgeStart[edgeId+1]-1) {
			double vecX1=x2-x,vecY1=y2-y,
					vecX2=nodeX-x,vecY2=nodeY-y,
					vecX3=nodeX-x2,vecY3=nodeY-y2;
			if (vecX1*vecX2+vecY1*vecY2>0 && -vecX1*vecX3-vecY1*vecY3>0 && (vecX1!=0 || vecY1!=0)){
				double rate=((nodeX-x2)*vecX1+(nodeY-y2)*vecY1)/(-vecX1*vecX1-vecY1*vecY1);
				double nearX=rate*x+(1-rate)*x2,nearY=rate*y+(1-rate)*y2;
				double dist=circleDistance(nearX,nearY,nodeX,nodeY);
				if (dist<result){
					result=dist;
					tmpSideLen=tmp+circleDistance(x,y,nearX,nearY);
				}
			}
			tmpRoadLen+=circleDistance(x,y,x2,y2);
		}
		tmp+=circleDistance(x,y,x2,y2);
	}

	*sideLen=tmpSideLen;
	*roadLen=tmpRoadLen;
	return result;
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

int trNumber;
int tid[200000];
double x[200000],y[200000];

int main(int argc, char *argv[]){

	if (argc < 6) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	loadData(argv[1]);
	
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
	
	FILE* f5 = fopen("5.txt","r");
	fscanf(f5,"%d",&trNumber);
	for (i = 0; i < trNumber; ++i)
		fscanf(f5,"%d %lf %lf",&tid[i],&x[i],&y[i]);
	for (i = 1; i < trNumber; ++i)
		if (tid[i] < tid[i-1]) printf("%d\n",i);
	int nowProcess = 0;
	
	long begin = clock();
	
	int traNumber = readInt(temporal);
	readInt(spatial);
	
	answerSize = 0;
	int test = 0;
	
	for (i = 0; i < traNumber; ++i) {
		//puts(inPath[i]);
		int timeLine = readInt(temporal);
		int j;
		
		for (j = 0; j < timeLine; ++j) { 
			dist[j] = readInt(temporal);		//距离，整数
			stamp[j] = readInt(temporal);		//时间戳，整数
		}
		
		int spaLine = readInt(spatial);
		for (j = 0; j < spaLine; ++j) {
			edge[j] = readInt(spatial);			//轨迹中边编号
			acc[j] = readDouble(spatial);		//边的起点路口距离轨迹起点的轨迹距离
		}
		
		//printf("%d %d\n",dist[0],dist[timeLine-1]);
		
		if (tid[nowProcess] < i) ++nowProcess;
		if (tid[nowProcess] == i) {
			++test;
			double sideLen, roadLen;
			double tmp = 1e100;
			int q;
			for (j = 0; j < spaLine; ++j) {
					double mm = nodeToEdgeDistanceAndNodeSide(x[nowProcess],y[nowProcess],edge[j],&sideLen,&roadLen);
					if (mm < 0.1) {
						tmp = mm;
						q = j;
						break;
					}
				}
			double d = sideLen; if (q > 0) d += acc[q-1]; 
			//printf("%lf\n",d);
			for (j = 0; j < timeLine -1; ++j)
				if (dist[j] <= d && dist[j+1] >=d) {
					answer[answerSize][0] = i;
					answer[answerSize][1] = stamp[j] + (double)(stamp[j+1] - stamp[j])*(d - dist[j]) / (dist[j+1] - dist[j]);
					++answerSize;
					break;
				}
			
		}
	}
	//puts("");
	printf("%d\n",test);
	long end = clock();
	FILE* fpp = fopen(argv[6],"w");
	fprintf(fpp,"%ld\n",end - begin);
	fclose(fpp);
	
	fprintf(result,"%d\n",answerSize);
	for (i = 0; i < answerSize; ++i) 
		fprintf(result,"%d %lf\n\n",(int)answer[i][0],answer[i][1]);
	
	fclose(result);
	fclose(spatial);
	fclose(temporal);
	
	return 0;
}
