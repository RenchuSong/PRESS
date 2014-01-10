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
int next[100000];


double compDist[100000];
double lenll[1000000];
int path[100000];				//记录原始SPATIAL路径

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

int main(int argc, char *argv[])
{
	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	
	long bbb = clock();
	
	//载入路网数据
	loadData(argv[1]);
	
	
	FILE* spatial = fopen(argv[2],"rb");
	readInt(spatial);
	//1 读入时间轨迹文件
	FILE* temporal = fopen(argv[3],"rb");
	
	int tjNumber = readInt(temporal);	//轨迹条数
	
	FILE* result = fopen(argv[4],"a+");	//压缩结果
	
	//writeInt(result,tjNumber);
	
	//时间同步路网距离误差允许的上界
	char* ttt = argv[5];
	int i;
	int tolerance = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance = tolerance * 10 + ttt[i] - 48;
	printf("%d\n",tolerance);
	
	//路网同步时间距离
	ttt = argv[6];
	int tolerance2 = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance2 = tolerance2 * 10 + ttt[i] - 48;
	printf("%d\n",tolerance2);
	
	
	
	int distAll = 0;
	
	double upper, downer;
	double deviation = 0;
	int oriTotal = 0, compTotal = 0;
	
	for (i = 0; i < tjNumber; ++i) {
		int j;
		int sNumber = readInt(spatial);	//路径段数
		
		for (j = 0; j < sNumber; ++j) {
			path[j] = readInt(spatial);				//路径上的边
			lenll[path[j]] = readDouble(spatial);
		}
		
		int spNumber = readInt(temporal);	//路径段数
		oriTotal += spNumber;
		
		for (j = 0; j < spNumber; ++j) {
			dist[j] = readInt(temporal);	
			time[j] = readInt(temporal);	
			//printf("%lf\t",dist[j]);
		}
		
		//for (j = 0; j < 20; ++j) printf("%d:[%d %d]          ",j,time[j]-2678400,dist[j]);puts("");
		
		distAll += dist[spNumber - 1];
		
		memset(valid,0,sizeof(valid));
		valid[0] = valid[spNumber - 1] = 1;
		
		int preNode = 0; upper = 1e100, downer = -1e100;
		while (preNode < spNumber - 1) {
			//printf("%d %d\t",preNode, spNumber);
			int flag = 1;
			for (j = preNode + 1; j < spNumber; ++j) {
				double d = (double)(dist[j] - dist[preNode]) / (time[j] - time[preNode]);
				if (d < downer || d > upper) {
					flag = 0;break;
				}
				d = (double)(dist[j] - dist[preNode] - tolerance) / (time[j] - time[preNode]);
				if (d > downer) downer = d;
				d = (double)(dist[j] - dist[preNode] + tolerance) / (time[j] - time[preNode]);
				if (d < upper) upper = d;
				
				d = (double)(dist[j] - dist[preNode])/ (time[j] - time[preNode] + tolerance2);
				if (d > downer) downer = d;
				if (time[j] - time[preNode] > tolerance2) {
					d = (double)(dist[j] - dist[preNode])/ (time[j] - time[preNode] - tolerance2);
					if (d < upper) upper = d;
				}

			}
			if (!flag) {
				valid[j-1] = 1;
				preNode = j-1;
				upper = 1e100; downer = -1e100;
			}else break;
		}

		int ansSize = 0;
		for (j = 0; j < spNumber; ++j) if (valid[j]) ++ansSize;
		compTotal += ansSize;
		
		next[spNumber-1] = spNumber-1;
		for (j = spNumber - 2; j >= 0; --j)
			if (valid[j+1]) next[j] = j+1; else next[j] = next[j+1];
		
		for (j = 0; j < spNumber-1; ++j) if (valid[j]) {
			compDist[j] = dist[j];
			int k;
			if (time[next[j]] - time[j] > 0)
				for (k = j + 1; k < next[j]; ++k) {
					compDist[k] = dist[j]+(double)(dist[next[j]] - dist[j])*(time[k] - time[j])/(time[next[j]] - time[j]);
				}
			else
				compDist[k] = dist[k];
		}
		
		//for (j = 0; j < spNumber-1; ++j) printf("%lf\n",dist[j]-compDist[j]); 
		
		double t1 = 0, t2 = 0;
		int c1 = 0, c2 = 0;
		double maxDiv = 0;
		for (j = 0; j < spNumber-1; ++j) if (dist[j] != compDist[j]){
			double x1, x2, y1, y2;
			while (c1 < sNumber-1 && t1 + lenll[path[c1]] <= dist[j]) {
				t1 += lenll[path[c1]];
				++c1;
			}
			while (c2 < sNumber-1 && t2 + lenll[path[c2]] <= compDist[j]) {
				t2 += lenll[path[c2]];
				++c2;
			}
			getPosition(path[c1], dist[j] - t1, &x1, &y1);
			getPosition(path[c2], compDist[j] - t2, &x2, &y2);
			
			double tmp = circleDistance(x1,y1,x2,y2);
			if (tmp < tolerance && maxDiv < tmp) maxDiv = tmp;
			
			//printf("%d %d  [%d %d %d %d] %lf\n",c1,c2,thisSide[path[c1]],other[path[c1]],thisSide[path[c2]],other[path[c2]], tmp);
		}
		
		deviation += maxDiv;
		
		//printf("[%d %d]",spNumber,ansSize);
		//puts("");
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(spatial);
	fclose(temporal);

	fprintf(result,"%d %d %.5lf\n",oriTotal+29768960, compTotal+5887488,deviation / tjNumber);
	fclose(result);
	
	
	return 0;
}
