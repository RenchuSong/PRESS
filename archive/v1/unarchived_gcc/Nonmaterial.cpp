/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
		之用路网进行划分
*/
#include <iostream>
#include <algorithm>
#include <vector>

#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>

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

//============
int nodeIndex[1000000][2];		//0：文件   1：偏移
int spSize;						//到多少个点有最短路
int spPre[1000000];				//每个点的上一条边,-1表示不存在
int path[100000];				//记录原始SPATIAL路径
double timeStamp[100000];		//每个道路的起点时间戳
int ansSize;					//压缩后实体个数
int ansList[10000];			//压缩后的轨迹，测试解压用
//============

int save[10000];
void writePath(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = spPre[node2];
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i)
		printf("%d ",save[i]);
}

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

int dataNumber;
char inPath1[20000][100];
char inPath2[20000][100];
char inPath3[20000][100];
char outPath[20000][100];

double tem_time[100000], tem_dist[100000];

void getInputPathSet1(char* path)
{	
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath1[dataNumber],path);
		strcat(inPath1[dataNumber],"/");
		strcat(inPath1[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

int segCount;
int hs[50000];
double cx1[50000],cx2[50000],cy1[50000],cy2[50000];
int cstreet[50000];

int ansCount;
int ansStreet[50000];
double ansx[50000],ansy[50000];
double acc[100000];
double getSlope(double x1, double y1, double x2, double y2)
{
	//未考虑垂直情况!
	if ((x2 - x1)*(x2-x1)<1e-8) return 0;
	return (y2 - y1) / (x2 - x1);
}
vector<int> turnSegs;
void nonmateralized()
{
	if (segCount == 1)
	{
		ansCount = 1;
		return;
	}

	turnSegs.clear();
	turnSegs.push_back(0);

	double slopeUB = 1e10;
	double slopeLB = -1e10;

	//取第一条线段为初始基准线段
	int baseSegID = 0;
	double r1, r2;	//基准线段的有效区间[r1,r2]
	if (hs[baseSegID] == 0)
	{
		r1 = cx1[0];
		r2 = cx2[0];
	}
	else
	{
		r1 = cy1[0];
		r2 = cy2[0];
	}
	
	for (int currSegID=1; currSegID!=segCount; ++currSegID)
	{
		//当前处理线段与基准线段不在同一street的情况：发生转折
		if (cstreet[currSegID] != cstreet[baseSegID])
		{
			int lastSegID = currSegID - 1;
			if (lastSegID != baseSegID)
			{
				turnSegs.push_back(lastSegID);
			}
			turnSegs.push_back(currSegID);

			baseSegID = currSegID;
			if (hs[currSegID] == 0)
			{
				r1 = cx1[currSegID];
				r2 = cx2[currSegID];
			}
			else
			{
				r1 = cy1[currSegID];
				r2 = cy2[currSegID];
			}

			slopeUB = 1e10;
			slopeLB = -1e10;
			continue;
		}

		//计算当前线段与基准线段的斜率上下界
		double currSlopeUB;
		double currSlopeLB;
		if (hs[baseSegID] == 1)
		{
			if (hs[currSegID] == 0)
			{
				currSlopeUB = getSlope(cx1[baseSegID], r1, cx1[currSegID], cy1[currSegID]);
				currSlopeLB = getSlope(cx2[baseSegID], r2, cx2[currSegID], cy2[currSegID]);
			}
			else
			{
				currSlopeUB = getSlope(cx1[baseSegID], r1, cx2[currSegID], cy2[currSegID]);
				currSlopeLB = getSlope(cx2[baseSegID], r2, cx1[currSegID], cy1[currSegID]);
			}
		}
		else
		{
			if (hs[currSegID] == 0)
			{
				currSlopeUB = getSlope(r2, cy2[baseSegID], cx1[currSegID], cy1[currSegID]);
				currSlopeLB = getSlope(r1, cy1[baseSegID], cx2[currSegID], cy2[currSegID]);
			}
			else
			{
				currSlopeUB = getSlope(r2, cy2[baseSegID], cx2[currSegID], cy2[currSegID]);
				currSlopeLB = getSlope(r1, cy1[baseSegID], cx1[currSegID], cy1[currSegID]);
			}
			
		}

		//斜率空间求交
		if ((currSlopeUB<slopeLB||currSlopeLB>slopeUB))	//需要发生转折
		{
			//将两条直线表示成y=kx+b的形式，通过基准线段的两个端点求b的值
			double bUB, bLB;
			if (hs[baseSegID] == 1)
			{
				bUB = r1 - slopeUB * cx1[baseSegID];
				bLB = r2 - slopeLB * cx2[baseSegID];
			}
			else
			{
				bUB = cy2[baseSegID] - slopeUB * r2;
				bLB = cy1[baseSegID] - slopeLB * r1;
			}

			//求最后一条线段的有效区间，转折发生在最后一条线段
			int lastSegID = currSegID - 1;
			if (hs[lastSegID] == 0)
			{
				double xUB = (cy1[lastSegID] - bUB) / slopeUB;
				double xLB = (cy1[lastSegID] - bLB) / slopeLB;
				if (xUB < xLB)
				{
					swap(xUB, xLB);
				}
				double valid_x1 = (xLB > cx1[lastSegID])?xLB:cx1[lastSegID];
				double valid_x2 = (cx2[lastSegID] > xUB)?xUB:cx2[lastSegID];

				turnSegs.push_back(lastSegID);
				r1 = valid_x1;
				r2 = valid_x2;
			}
			else
			{
				double yUB = slopeUB * cx1[lastSegID] + bUB;
				double yLB = slopeLB * cx1[lastSegID] + bLB;
				if (yUB < yLB)
				{
					swap(yUB, yLB);
				}
				double valid_y1 = (yLB > cy1[lastSegID])?yLB:cy1[lastSegID];
				double valid_y2 = (yUB < cy2[lastSegID])?yUB:cy2[lastSegID];
				
				turnSegs.push_back(lastSegID);
				r1 = valid_y1;
				r2 = valid_y2;
			}

			//将发生转折的线段作为新的基准线段
			baseSegID = lastSegID;
			currSegID--;
			slopeUB = 1e10;
			slopeLB = -1e10;
			
		}
		else
		{
			if (currSegID == segCount - 1)
			{
				turnSegs.push_back(currSegID);
				break;
			}

			if (currSlopeUB < slopeUB)
			{
				slopeUB = currSlopeUB;
			}
			if (currSlopeLB > slopeLB)
			{
				slopeLB = currSlopeLB;
			}
		}

	}// end for

	ansCount = turnSegs.size();
	/*
	cout << turnSegs.size() << endl;
	for (int i=0; i!=turnSegs.size(); ++i)
	{
		cout << turnSegs[i] << " ";
	}
	cout << endl;*/
}

double lenll[1000000];
double compDist[100000];

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
	//Load Road Network from File
	loadData(argv[1]);
	
	//载入街道信息
	loadStreet();
	
	long bbb = clock();
	long totalTime = 0;
	
	//1 读入轨迹文件
	FILE* file = fopen(argv[2],"rb");
	
	int tjNumber = readInt(file);	//轨迹条数
	
	//2 读入空间轨迹
	FILE* temporal = fopen(argv[3],"rb");
	readInt(temporal);
	
	//3 读入路口时间戳
	getInputPathSet1(argv[4]);
	
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
	
	
	FILE* result = fopen(argv[7],"wb");	//用街道编号压缩
	writeInt(result,tjNumber);
	//printf("%d\n",tjNumber);
	int out = 0;
	double deviation = 0;
	int oriTotal = 0, compTotal = 0;
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数
		
		//--
		//printf("%d\n",spNumber);
		ansSize = 0;
		
		int j;
		for (j = 0; j < spNumber; ++j) {
			path[j] = readInt(file);				//路径上的边
			acc[j] = readDouble(file);
			lenll[path[j]] = acc[j];
		}
		for (j = 1; j < spNumber; ++j) acc[j] += acc[j-1];
		
		totalTime += clock() - bbb;
		
		
		FILE* fp = fopen(inPath1[i],"r");			//路径的道口与时间戳
		for (j = 0; j < spNumber; ++j) 
			fscanf(fp,"%d %lf",&path[j],&timeStamp[j]);
		fclose(fp);
		
		bbb = clock();
		
		int tNumber = readInt(temporal);			//路径的Temporal
		for (j = 0; j < tNumber; ++j) {
			tem_dist[j] = (double)readInt(temporal);
			tem_time[j] = (double)readInt(temporal);
		}
		
		int nowStreet = road2Street[path[0]][0];	//起始道路
		int nowBias = road2Street[path[0]][1];	//起始偏移
		
		//--
		ansList[ansSize] = 0;
		++ansSize;
		//writeInt(result,path[0]);
		
		int nowEdge = 1;
		while (nowEdge < spNumber-1) {
			++nowBias;
			if (street2road[nowStreet][nowBias]!=path[nowEdge]) {
				nowStreet = road2Street[path[nowEdge]][0];	//道路
				nowBias = road2Street[path[nowEdge]][1];	//偏移
				//--
				ansList[ansSize] = nowEdge;
				++ansSize;
				//writeInt(result,path[nowEdge]);			
			}
			++nowEdge;
		}
		
		nowStreet = road2Street[path[nowEdge]][0];	//道路
		nowBias = road2Street[path[nowEdge]][1];	//偏移

		ansList[ansSize] = nowEdge;
		++ansSize;
		//writeInt(result,path[nowEdge]);	
		segCount = 0;	//段数
		
		//puts("here");
		//for (j = 0; j < ansSize; ++j) printf("[%d]\t",ansList[j]);
		
		int left = 0, right = 0;
		while (left < ansSize && right < tNumber) {
			if (timeStamp[ansList[left]] < tem_time[right]) {
				hs[segCount] = 0;
				cx1[segCount] = timeStamp[ansList[left]];
				cy1[segCount] = (ansList[left] == 0 ? 0 : acc[ansList[left]-1]);
				cstreet[segCount] = ansList[left];
				++segCount;++left;
			}else {
				hs[segCount] = 1;
				cx1[segCount] = tem_time[right];
				cy1[segCount] = tem_dist[right];
				cstreet[segCount] = cstreet[segCount-1];
				++segCount;++right;
			}
		}
		//puts("here");
		while (left < ansSize) {
			hs[segCount] = 0;
			cx1[segCount] = timeStamp[ansList[left]];
			cy1[segCount] = (ansList[left] == 0 ? 0 : acc[ansList[left]-1]);
			cstreet[segCount] = left;
			++segCount;++left;
		}
		while (right < tNumber) {
			hs[segCount] = 1;
			cx1[segCount] = tem_time[right];
			cy1[segCount] = tem_dist[right];
			cstreet[segCount] = cstreet[segCount-1];
			++segCount;++right;
		}
		
		for (j = 0; j < segCount; ++j){
			//printf("[%d %.4lf %.4lf %d]\t",hs[j],cx1[j],cy1[j],cstreet[j]);
			//puts("");
			
			
			if (hs[j] == 0) {
				cx2[j] = cx1[j] + tolerance2;
				cx1[j] -= tolerance2;
				cy2[j] = cy1[j];
			}
			if (hs[j] == 1) {
				cy2[j] = cy1[j] + tolerance;
				cy1[j] -= tolerance;
				cx2[j] = cx1[j];
			}
			//if (cstreet[j]> cstreet[j-1]+1)
			//printf("[%.4lf %.4lf %.4lf %.4lf %d]\t",cx1[j],cy1[j],cx2[j],cy2[j],cstreet[j]);
		}
		//puts("");
		
		//cout << spNumber << "\t";
		nonmateralized();
		bbb = clock();
		
		for (j = 0;j < segCount; ++j) {
			cx1[j] = (cx1[j] + cx2[j]) / 2;
			cy1[j] = (cy1[j] + cy2[j]) / 2;
		}
		
		for (j = 0; j < turnSegs.size()-1; ++j){
			int k;
			for (k = turnSegs[j]; k < turnSegs[j+1]; ++k) {
				if (cx1[turnSegs[j+1]] - cx1[turnSegs[j]] > 0)
					compDist[k] = cy1[turnSegs[j]]+(cy1[turnSegs[j+1]] - cy1[turnSegs[j]])*(cx1[k] - cx1[turnSegs[j]])/(cx1[turnSegs[j+1]] - cx1[turnSegs[j]]);
				else
					compDist[k] = cy1[turnSegs[j]];
			}
		}	
		
		double t1 = 0, t2 = 0;
		int c1 = 0, c2 = 0;
		double maxDiv = 0;
		for (j = 0; j < segCount-1; ++j) if (cy1[j] != compDist[j]){
			double x1, x2, y1, y2;
			while (c1 < spNumber-1 && t1 + lenll[path[c1]] <= cy1[j]) {
				t1 += lenll[path[c1]];
				++c1;
			}
			while (c2 < spNumber-1 && t2 + lenll[path[c2]] <= compDist[j]) {
				t2 += lenll[path[c2]];
				++c2;
			}
			getPosition(path[c1], cy1[j] - t1, &x1, &y1);
			getPosition(path[c2], compDist[j] - t2, &x2, &y2);
			
			double tmp = circleDistance(x1,y1,x2,y2);
			if (tmp < tolerance && tmp > maxDiv) maxDiv = tmp;;
			//printf("%d %d  [%d %d %d %d] %lf\n",c1,c2,thisSide[path[c1]],other[path[c1]],thisSide[path[c2]],other[path[c2]],);
		}
		deviation += maxDiv;
		/*if (cx1[j] > cx1[j-1]){
			int nowStreet = road2Street[cstreet[turnSegs[j]]][0];	//起始道路
			int nowBias = road2Street[cstreet[turnSegs[j]]][1];	//起始偏移
			
			double distance = fabs(cy1[j]);
			int q = 0;
			while (distance < cy1[j+1] && q < 10) {
				++q;
				//if (i > 200)
				//printf("%d",q);
				if (q >= 10) break;
				int road = street2road[nowStreet][nowBias];
				//printf("%d ",road);
				writeInt(result,road);
				distance += fabs(lenll[road])+1;
				++nowBias;	
			}
		}
		*/
		
		totalTime += clock() - bbb;
		bbb = clock();
		
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
		oriTotal += spNumber + tNumber;
		compTotal += turnSegs.size();
	}
	/*
	FILE* fff = fopen(argv[8],"a+");
	fprintf(fff,"%d\n",out);
	fclose(fff);
	*/
	
	fclose(temporal);
	fclose(file);
	//FILE* fpp = fopen(argv[8],"a+");
	//fprintf(fpp,"%d %d %.5lf\n",oriTotal, compTotal,deviation / tjNumber);
	//fclose(fpp);
	return 0;
}
