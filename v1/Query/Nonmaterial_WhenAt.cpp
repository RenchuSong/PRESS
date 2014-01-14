/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
*/
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
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

//============
int nodeIndex[1000000][2];		//0：文件   1：偏移
int spSize;						//到多少个点有最短路
int spPre[1000000];				//每个点的上一条边,-1表示不存在
int path[100000];				//记录原始SPATIAL路径
int ansSize;					//压缩后实体个数
int ansList[100000][3];			//压缩后的轨迹，测试解压用

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];
double accLen[1000000];

int save[10000];
int writePath(double* roadLen, double x, double y, double * result, int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
		if (save[count] == -1) return 0;
		node2 = thisSide[save[count]];
		++count;
	}
	double sideLen;
	int i;
	for (i = count-1; i >=0; --i) {
		//printf("%d ",save[i]);
		int road = save[i];
		double dist = nodeToEdgeDistanceAndNodeSide(x,y,road,&sideLen,roadLen);
		if (dist < 0.1) return road;
	}
		//fout << save[i] << " ";
}

//最短路
void loadSPAll2() {
	int t;
	int s;
	int f;
	int c = 0;

	SPLINK = new int*[30000];

	for (t = 0; t < 30000; ++t){
		SPLINK[t] = new int[30000];
	}
	for (t = 0; t < 30000; ++t)
		for (s = 0; s < 30000; ++s) 
			SPLINK[t][s] = -1;
	char fileName[100]="./ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		fileName[17] = (f / 10) + 48;
		fileName[18] = (f % 10) + 48;
		//cout << fileName << endl;
		//--
		//printf("%d %s\n",nodeId,fileName);
		FILE* file = fopen(fileName,"rb");
		
		//点数
		int number = readInt(file);
		int j;
		for (j = 0; j < number; ++j) {
			//id
			int id = readInt(file);
			//MAP[id] = c;
			//个数
			int many = readInt(file);
			int i;
			//memset(spPre,255,sizeof(spPre));
			for (i = 0; i < many; ++i){
				int q = readInt(file);
				SPLINK[MAP[id]][MAP[q]] = readInt(file);
			}
		}
		fclose(file);
	}
	
}

//街道
int streetNumber;
int road2Street[2000000][2];	//i号路段的街道编号和街道偏移
int street2road[50000][200];	//i号街道j号偏移所对应路段

void loadStreet() {
	memset(street2road,255,sizeof(street2road));
	memset(road2Street,255,sizeof(road2Street));
	FILE* file = fopen("./Street/Street.txt","r");
	while (1) {
		int number;
		fscanf(file,"%d",&number);
		if (number == -1) break;

		int q = 0;
		while (1) {
			int p;
			fscanf(file,"%d",&p);
			street2road[number][q] =p;
			road2Street[p][0]=number;
			road2Street[p][1] = q;
			++q;
			if (p ==-1) break;
		}
	}
	fclose(file);
}

//频繁轨迹
int fre_pre[200000], fre_last[60000], fre_other[200000][2];//0:哪个子轨迹 1：子轨迹中第几个项
int fre_number, item = 0;

//0：在哪个子轨迹 1：起始偏移 2：终止偏移
int tmp_item[200000][3],tmp_mm[200000][3];
int tmp_num, tmp_num2;
int fre[10000][200];

void loadFrequency() {
	FILE* file = fopen("./Subtrajectory/sub_final.txt","r");
	memset(fre_last,255,sizeof(fre_last));
	fscanf(file,"%d",&fre_number);
	int i;
	for (i = 0; i < fre_number; ++i) {
		int num;
		fscanf(file,"%d",&num);
		int j,e;
		for (j = 0; j < num; ++j) {
			fscanf(file, "%d",&e);
			fre[i][j] = e;
			fre_other[item][0] = i;
			fre_other[item][1] = j;
			fre_pre[item] = fre_last[e];
			fre_last[e] = item;
			++item;
		}
	}
	
	fclose(file);
}

int answerSize;
double answer[100000][3];		//id x,y
int dist[100000],stamp[100000];
int edge[100000];
double acc[100000];


int trNumber;
int tid[200000];
double x[200000],y[200000];

int main(int argc, char *argv[])
{
	if (argc < 6) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	char sss[20] = "";
	loadData(sss);
	
	int nodeUseful = 0;
	int i;

	for (i = 0; i < nodeNumber; ++i) {
		//nodeInRegion[i] = 1;
		MAP[i] = nodeUseful;
		++nodeUseful;
	}
	
	//============载入相关文件=====================
	//全内存载入最短路
	//loadSPAll2();
	//载入街道编号
	loadStreet();
	//载入频繁轨迹
	//loadFrequency();
	
	//空间和时间文件
	FILE* spatial = fopen(argv[2],"rb");
	FILE* temporal = fopen(argv[3],"rb");
	
	//输出文件
	FILE* result = fopen(argv[4],"w");
	
	//查询的时间
	char* ttt = argv[5];
	int timeStamp = 0;
	for (i = 0; i < strlen(ttt); ++i) timeStamp = timeStamp * 10 + ttt[i] - 48;
	printf("查询时间：%d\n",timeStamp);
	
	
	
	int traNumber = readInt(temporal);
	readInt(spatial);
	
	double sideLen,roadLen;

	answerSize = 0;
	


	
	FILE* f5 = fopen("5.txt","r");
	fscanf(f5,"%d",&trNumber);
	for (i = 0; i < trNumber; ++i)
		fscanf(f5,"%d %lf %lf",&tid[i],&x[i],&y[i]);
	for (i = 1; i < trNumber; ++i)
		if (tid[i] < tid[i-1]) printf("%d\n",i);
	int nowProcess = 0;

	
	answerSize = 0;
	int test = 0;
	
	long begin = clock();
	double ansTTT = 0;
	for (i = 0; i < traNumber; ++i) {
		//puts(inPath[i]);
		int timeLine = readInt(temporal);
		int j;
		begin = clock();
		for (j = 0; j < timeLine; ++j) { 
			dist[j] = readInt(temporal);		//距离，整数
			stamp[j] = readInt(temporal);		//时间戳，整数
		}
		ansTTT += clock() - begin;
		begin = clock();
		int spaLine = readInt(spatial);
		for (j = 0; j < spaLine; ++j) {
			edge[j] = readInt(spatial);			//轨迹中边编号
			acc[j] = readDouble(spatial);		//边的起点路口距离轨迹起点的轨迹距离
		}
		ansTTT += (clock() - begin);
		//printf("%d %d\n",dist[0],dist[timeLine-1]);
		
		if (tid[nowProcess] < i) ++nowProcess;
		if (tid[nowProcess] == i) {
			++test;
			double sideLen, roadLen;
			double tmp = 1e100;
			int q;
			double d = 0;
			//puts("1");
			for (j = 0; j < spaLine-1; ++j) {
				double x1 = coordNode[(thisSide[edge[j]])*2];
				double x2 = coordNode[(thisSide[edge[j+1]])*2];
				double y1 = coordNode[(thisSide[edge[j]])*2+1];
				double y2 = coordNode[(thisSide[edge[j+1]])*2+1];
				if (x1 > x2) {
					x1 += x2;
					x2 = x1 - x2;
					x1 = x1 - x2;
				}
				if (y1 > y2) {
					y1 += y2;
					y2 = y1 - y2;
					y1 = y1 - y2;
				}
				if (x1 <= x[nowProcess] && x2 >= x[nowProcess] && y1 <= y[nowProcess] && y2 >= y[nowProcess]) {
					//puts("2");
					int flag = 0;
					int road;

					ansList[j][0] = (path[j] >> 28) & 15;
					
					//printf("[%d %d %d]\t",ansList[j][0],ansList[j][1],ansList[j][2]);
					bool flag2 = false;
					//puts("4");

						//printf("[2] ");
						int nowStreet = road2Street[ansList[j][1]][0];	//起始道路
						int nowBias = road2Street[ansList[j][1]][1];	//起始偏移
						//puts("5");
						for (int k = 0; k< ansList[j][2]; ++k) {
							road = street2road[nowStreet][nowBias];
							double dist = nodeToEdgeDistanceAndNodeSide(x[nowProcess],y[nowProcess],road,&sideLen,&roadLen);
							//printf("%d ",road);
							accLen[j] += roadLen;
							if (dist < 0.1) {
								flag2 = true;
								break;
							}
							++nowBias;
							//puts("6");
						}
						
					//if (flag2) break;else d += acc[j];
					break;
				} else d += acc[j];

				

			}
			
			
			
			//printf("%lf\n",d);
			for (j = 0; j < timeLine -1; ++j)
				if (dist[j] <= d && dist[j+1] >=d) {
					answer[answerSize][0] = i;
					answer[answerSize][1] = stamp[j] + (double)(stamp[j+1] - stamp[j])*(d - dist[j]) / (dist[j+1] - dist[j]);
					++answerSize;
					break;
				}
			
			}
			ansTTT += (clock() - begin) / 5.0;

	}
	//puts("");
	printf("%d\n",test);
	long end = clock();
	FILE* fpp = fopen(argv[6],"w");
	fprintf(fpp,"%ld\n",(long)ansTTT);
	fclose(fpp);
	
	/*

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
			path[j] = readInt(spatial);
			acc[j] = readDouble(spatial);
		}
		//printf("%d:[%d %d]\n",timeStamp,stamp[0],stamp[timeLine - 1]);
		if (stamp[0] <= timeStamp && stamp[timeLine - 1] >= timeStamp) {
			int road;
			int accTime = 0;
			for (j = 0; j < timeLine-1; ++j)
				if (stamp[j] <= timeStamp && stamp[j+1] >= timeStamp) break;
			double relaDist = dist[j] + (dist[j+1] - dist[j])*(timeStamp - stamp[j]) / (stamp[j+1] - stamp[j]);
			for (j = 0; j < spaLine; ++j)
				if (relaDist > acc[j]) relaDist -= acc[j]; else {
					//extract trajectory
					
					int flag = 0;
					
					accLen[j] = 0;

					ansList[j][0] = (path[j] >> 28) & 15;
					if (ansList[j][0] == 15) ansList[j][0] = 2; else
					if (ansList[j][0] == 14) ansList[j][0] = 3; else
						ansList[j][0] = 1;

					if (ansList[j][0] == 1) {
						ansList[j][1] = (path[j] >> 16) & 65535;
						ansList[j][2] = path[j] & 65535;
					}
					if (ansList[j][0] == 2) {
						ansList[j][1] = (path[j] >> 8) & 65535;
						ansList[j][2] = path[j] & 255;
					}
					if (ansList[j][0] == 3) {
						ansList[j][1] = path[j] & ((1 << 27)-1);
					}
					//printf("[%d %d %d]\t",ansList[j][0],ansList[j][1],ansList[j][2]);
					
					if (ansList[j][0] == 3) {
						//printf("[3] ");
						int left = (ansList[j][1] >> 7) & 127;
						int right = ansList[j][1] & 127;
						for (int k = left; k <= right; ++k) {
							//printf("%d ",fre[(ansList[j][1] >> 14 ) & 32767][k]);
							road = fre[(ansList[j][1] >> 14 ) & 32767][k];
							nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
							//printf("%d ",road);
							accLen[j] += roadLen;
							if (accLen[j] >= relaDist) {
								relaDist -= (accLen[j] - roadLen);
								flag = 1;
								break;
							}
						}
					}
					if (ansList[j][0] == 1) {
						//printf("[1] ");
						//printf("%d ",ansList[j][1]);
						road = ansList[j][1];
						nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
							//printf("%d ",road);
						accLen[j] += roadLen;
						if (accLen[j] >= relaDist) {
							relaDist -= (accLen[j] - roadLen);
							flag = 1;
							break;
						}else
							road = writePath(&relaDist,&accLen[j],other[ansList[j][1]],other[ansList[j][2]]);
					}
					if (ansList[j][0] == 2) {
						//printf("[2] ");
						int nowStreet = road2Street[ansList[j][1]][0];	//起始道路
						int nowBias = road2Street[ansList[j][1]][1];	//起始偏移
						for (int k = 0; k< ansList[j][2]; ++k) {
							road = street2road[nowStreet][nowBias];
							nodeToEdgeDistanceAndNodeSide(0,0,road,&sideLen,&roadLen);
							//printf("%d ",road);
							accLen[j] += roadLen;
							if (accLen[j] >= relaDist) {
								relaDist -= (accLen[j] - roadLen);
								flag = 1;
								break;
							}
							++nowBias;
						}
					}
					break;
				}

				answer[answerSize][0] = i;
				getPosition(road, relaDist, &answer[answerSize][1], &answer[answerSize][2]);
				++answerSize;
		}
		
		//调试用
		//if (i % 1000 ==999) printf("+");
		//if (i % 10000 ==9999) printf("\n");
	}
	//puts("");
	

	long end = clock();
	FILE* fpp = fopen(argv[6],"w");
	fprintf(fpp,"%ld\n",end - begin);
	fclose(fpp);

	*/
	
	fprintf(result,"%d\n",answerSize);
	for (i = 0; i < answerSize; ++i) 
		fprintf(result,"%d %lf %lf\n",(int)answer[i][0],answer[i][1],answer[i][2]);
	
	fclose(result);
	fclose(spatial);
	fclose(temporal);

	return 0;
}
