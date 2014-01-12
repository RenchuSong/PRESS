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
int path[100000];
double stamp[100000];

double query[100000][2];

int main(int argc, char *argv[]){

	//Load Road Network from File
	loadData(argv[1]);
	
	//时空数据文件
	FILE* st = fopen(argv[2],"rb");
	//结果
	FILE* result = fopen(argv[3],"w");
	
	FILE* fpp = fopen("./MMTCWhenAtTime.txt","w");
	int when = 13112826;
	
	FILE* loc = fopen("./5.txt","r");
	int tn;
	fscanf(loc,"%d",&tn);
	int i;
	for (i = 0; i < tn; ++i) {
		int d;
		fscanf(loc,"%d",&d);
		fscanf(loc,"%lf %lf",&query[d][0],&query[d][1]);
	}
	
	for (i = 0; i < 10000; ++i) {
		printf("%d\t",i);
		int src;
		long t = clock();
		int flag;
		int ans1;double ans2;
		for (src = 0; src < 1000; ++src) {
			int spNumber = readInt(st);
			//printf("%d\t",spNumber);
			int j;
			for (j = 1; j < spNumber; ++j) {
				path[j] = readInt(st);
				stamp[j] = readDouble(st);
			}
			
			double minp = 1e100;
			int p;
			double sideLen, roadLen;
			for (j = 1; j < spNumber; ++j) {
				double tS, tR;
				double tmp = nodeToEdgeDistanceAndNodeSide(query[i][0],query[i][1],path[j],&tS,&tR);
				if (tmp < minp) {
					minp = tmp;
					sideLen = tS;roadLen = tR;
					p = j;
				}
				if (tmp > 10*minp) break;
			}
			flag = 0;
			if (p < spNumber - 1 && (stamp[p] + (stamp[p+1] - stamp[p])*sideLen/roadLen) > 0) {
				flag = 1;
				ans1 = i;
				ans2 = stamp[p] + (stamp[p+1] - stamp[p])*sideLen/roadLen;
				//printf("%d %lf\n",i,stamp[p] + (stamp[p+1] - stamp[p])*sideLen/roadLen);
			}
		}
		if (flag) {
			fprintf(result,"%d %lf\n",ans1,ans2);
		}
		fprintf(fpp,"%ld\n",clock() - t);
	}
	
	fclose(fpp);
	fclose(loc);
	fclose(st);
	fclose(result);
	return 0;
}
