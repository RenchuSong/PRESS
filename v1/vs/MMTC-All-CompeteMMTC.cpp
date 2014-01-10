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
#define mymax(x,y) ((x) > (y) ? (x) : (y))
using namespace std;

ofstream fout("E:\\TC\\OUTPUT.TXT");

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
int ansSize;					//压缩后实体个数
int ansList[100000][3];			//压缩后的轨迹，测试解压用
double acc[100000];

int SPNUM[40000];
int** SPLINK;
int MAP[2000000];

int save[10000];

int road1Number = 0;
int comRoad1[100000];
int road2Number = 0;
int comRoad2[100000];

bool scan[50000];

void writePath(int node1, int node2) {
	//printf("\n%d %d\n",node1,node2);
	int count = 0;
	while (node2!=node1) {
		save[count] = SPLINK[MAP[node1]][MAP[node2]];
		if (save[count] == -1) return;
		node2 = thisSide[save[count]];
		++count;
	}
	int i;
	for (i = count-1; i >=0; --i)
		//printf("%d ",save[i]);
		fout << save[i] << " ";
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
	char fileName[100]="F:/新加坡轨迹/ShortestPath/SP00.txt";
	
	//////
	for (f = 0; f < 21; ++f) {
		fileName[29] = (f / 10) + 48;
		fileName[30] = (f % 10) + 48;
		cout << fileName << endl;
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

int cmp(const void* a, const void* b) {
	return (*(int*)a) - (*(int*)b);
}

//频繁轨迹
int fre_pre[200000], fre_last[60000], fre_other[200000][2];//0:哪个子轨迹 1：子轨迹中第几个项
int fre_number, item = 0;

//0：在哪个子轨迹 1：起始偏移 2：终止偏移
int tmp_item[200000][3],tmp_mm[200000][3];
int tmp_num, tmp_num2;
int fre[10000][200];
int writeList[1000000];

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

//读入数据
int timing[50000];
int dist[50000];
int validing[50000];
double timeStamp[50000];
double timeMap[50000];

int main(int argc, char *argv[])
{
	
	if (argc < 3) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	//char sss[20] = "";
	
	loadData(argv[1]);
	
	//===============
	int nodeUseful = 0;
	int i;

	for (i = 0; i < nodeNumber; ++i) {
		nodeInRegion[i] = 1;
		MAP[i] = nodeUseful;
		++nodeUseful;
	}
	
	//============载入相关文件=====================
	//全内存载入最短路
	loadSPAll2();
	cout << "SP-";
	//载入街道编号
	loadStreet();
	cout << "ST-";
	//载入频繁轨迹
	loadFrequency();
	
	cout << "FR"<< endl;

	long bbb = clock();

	int tolerance, tolerance2;

	for (tolerance = 9000; tolerance < 100000; tolerance += 10000) {
		for (tolerance2 = 9000; tolerance2 < 100000; tolerance2 += 10000){
	//2 读取轨迹并处理
	FILE* file = fopen(argv[2],"rb");	//几何
	
	
	int tjNumber = readInt(file);	//轨迹条数
	cout << tjNumber << endl;

	//FILE* temporal = fopen(argv[2],"rb");
	
	//readInt(temporal);	//轨迹条数

	FILE* stamp = fopen(argv[3],"r");	//time stamp of each intersection

	//时间同步路网距离误差允许的上界
	char* ttt = argv[4];
;	/*
	int tolerance = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance = tolerance * 10 + ttt[i] - 48;
	printf("%d\n",tolerance);
	
	//路网同步时间距离
	ttt = argv[5];
	int tolerance2 = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance2 = tolerance2 * 10 + ttt[i] - 48;
	printf("%d\n",tolerance2);
	*/

	//FILE* result = fopen(argv[2],"wb");

	//writeInt(result,tjNumber);
	int total = 0, compress = 0;

	double diff = 0;

	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数
		memset(validing, 0, sizeof(validing));
		//读入这条道路
		int j;
		for (j = 0; j < spNumber; ++j) {
			path[j] = readInt(file);	//路径上的边
			acc[j] = readDouble(file);
			if (j > 0) acc[j] += acc[j-1];
		}
		int dddd;
		for (j = 0; j < spNumber; ++j) {
			fscanf(stamp,"%d",&dddd);
			fscanf(stamp,"%lf",&timeStamp[j]);
		}
		//答案大小
		ansSize = 0;
		
		//三种方式的可以继续有效的标记
		int sp = 1,st = 1, fr =1;	//最短路、街道、频繁序列
		int valid = 3;	//还有几种方式有效
		
		//第一条道路的偏移
		int nowStreet = road2Street[path[0]][0];	//起始道路
		int nowBias = road2Street[path[0]][1];	//起始偏移
		
		//第一条道路的频繁集合
		tmp_num = 0;
		int p = fre_last[path[0]];
		while (p > -1) {
			tmp_item[tmp_num][0] = fre_other[p][0];
			tmp_item[tmp_num][1] = fre_other[p][1];
			tmp_item[tmp_num][2] = fre_other[p][1];
			++tmp_num;
			p = fre_pre[p];
		}
		if (tmp_num == 0) {fr = 0;--valid;}

		//第一条道路的另一个端点
		int node = other[path[0]];	//轨迹起始点
		
		//当前的道路
		int nowEdge;
		//压缩当前这个段的起始道路
		int preEdge = 0;
		
		for (nowEdge = 1; nowEdge < spNumber; ++nowEdge) {
			
			//3：频繁项集
			if (fr == 1) {
				//后一个的项集合
				tmp_num2 = 0;
				int p = fre_last[path[nowEdge]];
				while (p > -1) {
					tmp_mm[tmp_num2][0] = fre_other[p][0];
					tmp_mm[tmp_num2][2] = fre_other[p][1];
					++tmp_num2;
					p = fre_pre[p];
				}
				//项集合求并
				int tp = 0;
				int l,r;
				for (r = 0;r < tmp_num2; ++r) 
					for (l = 0; l < tmp_num; ++l)
						if (tmp_item[l][0] == tmp_mm[r][0] && tmp_item[l][2] + 1 == tmp_mm[r][2]) {
							tmp_mm[tp][0] = tmp_mm[r][0];
							tmp_mm[tp][1] = tmp_item[l][1];
							tmp_mm[tp][2] = tmp_mm[r][2];
							++tp;
							break;
						}
				//如果没有继续维系的了，那么输出上一个路段
				if (tp == 0) {
					fr = 0;--valid;
					
					if (valid == 0) {
						validing[preEdge] = 1;
						//ansList[ansSize][0] = 3;
						//ansList[ansSize++][1] = preEdge;
						//ansList[ansSize++][1] = ((tmp_item[0][0] & 8191) << 14) | ((tmp_item[0][1] & 127) << 7) | (tmp_item[0][2] & 127);
						preEdge = nowEdge;
						
						//初始化=======
						//三种方式的可以继续有效的标记
						sp = st = fr =1;	//最短路、街道、频繁序列
						valid = 3;	//还有几种方式有效
						//第一条道路的偏移
						nowStreet = road2Street[path[nowEdge]][0];	//起始道路
						nowBias = road2Street[path[nowEdge]][1];	//起始偏移
						//第一条道路的频繁集合
						tmp_num = 0;
						int p = fre_last[path[nowEdge]];
						while (p > -1) {
							tmp_item[tmp_num][0] = fre_other[p][0];
							tmp_item[tmp_num][1] = fre_other[p][1];
							tmp_item[tmp_num][2] = fre_other[p][1];
							++tmp_num;
							p = fre_pre[p];
						}
						if (tmp_num == 0) {fr = 0;--valid;}

						//第一条道路的另一个端点
						node = other[path[nowEdge]];	//轨迹起始点
						//=============
						
						continue;
					}
					
				}else {	//可以继续延长
					tmp_num = tp;
					for (j = 0; j < tmp_num; ++j) {
						tmp_item[j][0] = tmp_mm[j][0];
						tmp_item[j][1] = tmp_mm[j][1];
						tmp_item[j][2] = tmp_mm[j][2];
					}
				}
			}

			//1：最短路判断
			if (sp == 1) {	//最短路尚有效
				int node2 = other[path[nowEdge]];
				//不是最短路
				if (SPLINK[MAP[node]][MAP[node2]]!=path[nowEdge] || other[path[nowEdge-1]]!=thisSide[path[nowEdge]]) {
					sp = 0;--valid;
					//全部结束，输出并重置
					if (valid == 0) {
						//ansList[ansSize][0] = 1;//ansList[ansSize][1] = path[preEdge];ansList[ansSize++][2] = path[nowEdge - 1];
						//ansList[ansSize++][1] = preEdge;
						validing[preEdge] = 1;
						preEdge = nowEdge;
						
						//初始化=======
						//三种方式的可以继续有效的标记
						sp = st = fr =1;	//最短路、街道、频繁序列
						valid = 3;	//还有几种方式有效
						//第一条道路的偏移
						nowStreet = road2Street[path[nowEdge]][0];	//起始道路
						nowBias = road2Street[path[nowEdge]][1];	//起始偏移
						//第一条道路的频繁集合
						tmp_num = 0;
						int p = fre_last[path[nowEdge]];
						while (p > -1) {
							tmp_item[tmp_num][0] = fre_other[p][0];
							tmp_item[tmp_num][1] = fre_other[p][1];
							tmp_item[tmp_num][2] = fre_other[p][1];
							++tmp_num;
							p = fre_pre[p];
						}
						if (tmp_num == 0) {fr = 0;--valid;}

						//第一条道路的另一个端点
						node = other[path[nowEdge]];	//轨迹起始点
						//=============
						
						continue;
					}
				}
			}
		
			//2：街道判断
			if (st == 1) {
				++nowBias;
				if (street2road[nowStreet][nowBias]!=path[nowEdge]) {
					st = 0;--valid;
					//全部结束，输出并重置
					if (valid == 0) {
						//ansList[ansSize][0] = 2;//ansList[ansSize][1] = path[preEdge];ansList[ansSize++][2] = nowEdge - preEdge;
						//ansList[ansSize++][1] = preEdge;
						validing[preEdge] = 1;
						preEdge = nowEdge;
						
						//初始化=======
						//三种方式的可以继续有效的标记
						sp = st = fr =1;	//最短路、街道、频繁序列
						valid = 3;	//还有几种方式有效
						//第一条道路的偏移
						nowStreet = road2Street[path[nowEdge]][0];	//起始道路
						nowBias = road2Street[path[nowEdge]][1];	//起始偏移
						//第一条道路的频繁集合
						tmp_num = 0;
						int p = fre_last[path[nowEdge]];
						while (p > -1) {
							tmp_item[tmp_num][0] = fre_other[p][0];
							tmp_item[tmp_num][1] = fre_other[p][1];
							tmp_item[tmp_num][2] = fre_other[p][1];
							++tmp_num;
							p = fre_pre[p];
						}
						if (tmp_num == 0) {fr = 0;--valid;}

						//第一条道路的另一个端点
						node = other[path[nowEdge]];	//轨迹起始点
						//=============
						
						continue;
					}	
				}
			
			}
		}




		validing[preEdge] = 1;
		
		//spNumber = readInt(temporal);	//路径段数
		double upper, downer;

		validing[0] = validing[spNumber - 1] = 1;
		
		for (j = 0; j < spNumber; ++j) {
			dist[j] = (int)acc[j];				//vertical
			if (j > 0 && dist[j] < dist[j-1]) dist[j] = dist[j-1];
			timing[j] = (int) timeStamp[j];		//horizon
		}
		qsort(timing,spNumber,sizeof(int),cmp);
		for (j = 1; j < spNumber; ++j)
			if (timing[j] <= timing[j-1]) timing[j] = timing[j-1] + 1;
		
		for (j = 0; j < spNumber - 1; ++j)
			if (timing[j] >= timing[j+1] || dist[j] > dist[j+1]) cout << "Fuck!" << endl;

		int preNode = 0; upper = 1e100, downer = -1e100;
		while (preNode < spNumber - 1) {
			//printf("%d %d\t",preNode, spNumber);
			
			int flag = 1;
			for (j = preNode + 1; j < spNumber; ++j) {
				if (timing[j] > timing[preNode]) {
					double d = (double)(dist[j] - dist[preNode]) / (timing[j] - timing[preNode]);
					if (d < downer || d > upper) {
						flag = 0;break;
					}
					d = (double)(dist[j] - dist[preNode] - tolerance) / (timing[j] - timing[preNode]);
					if (d > downer) downer = d;
					d = (double)(dist[j] - dist[preNode] + tolerance) / (timing[j] - timing[preNode]);
					if (d < upper) upper = d;
				
					d = (double)(dist[j] - dist[preNode])/ (timing[j] - timing[preNode] + tolerance2);
					if (d > downer) downer = d;
					if (timing[j] - timing[preNode] > tolerance2) {
						d = (double)(dist[j] - dist[preNode])/ (timing[j] - timing[preNode] - tolerance2);
						if (d < upper) upper = d;
					}
				}
				/*
				if (upper < downer) {
					flag = 0;
					break;
				}*/
			}
			if (!flag) {
				validing[j-1] = 1;
				preNode = j-1;
				upper = 1e100; downer = -1e100;
			}else break;
		}

		int ansSize = 0;
		for (j = 0; j < spNumber; ++j) if (validing[j]) ++ansSize;
		int preOne = 0,nextOne;
		for (j = 0; j < spNumber; ++j) {
			//timeMap[j] = timeStamp[j];
			//if (timeStamp[j] >= timeStamp[preNode]) {
				if (validing[j]) {
					timeMap[j] = timing[j];
					preOne = j;
					int k;
					for (k = j+1;k < spNumber; ++k) if (validing[k]) {
						nextOne = k;
						break;
					}
				}else {
					timeMap[j] = timing[preOne] + (double)(timing[nextOne] - timing[preOne]) *(dist[j]-dist[preOne])/(dist[nextOne] - dist[preOne]);
				}
				//if ( fabs(timeMap[j] - timing[j]) > 10000)
				//printf("%.4lf\t",fabs(timeMap[j]-timing[j]));
				//diff += fabs(timeMap[j] - timing[j]);
			//}
		}
		
		for (j = 0; j < spNumber - 1; ++j) {
			double d1 = fabs((double)timing[j+1] - timing[j]);
			double d2 = fabs((double)timeMap[j+1] - timeMap[j]);
			diff += fabs(d1 - d2) / mymax(d1, d2);
		}
		
		total += spNumber;
		compress += ansSize;
		//printf("%.8lf\n",(double)ansSize/spNumber);
		/*for (j = 0; j < spNumber; ++j) if (validing[j]) {
			printf("%d\t",path[j]);
			//writeInt(result,dist[j]);
			//writeInt(result,timing[j]);
		}
		puts("");*/

		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	FILE*fpp =  fopen(argv[6],"a+");
	fprintf(fpp,"%.4lf\t%.4lf\n", (double)compress/total, 1 - diff / (total - tjNumber)/2);
	fclose(file);
	fclose(fpp);
	fclose(stamp);
	}
	}
	
	//FILE* fpp =fopen(argv[6],"a+");
	//fprintf(fpp,"%ld\n",clock() - bbb);
	//fclose(fpp);
	return 0;
}
