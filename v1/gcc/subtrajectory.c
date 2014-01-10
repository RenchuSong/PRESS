/**
	找频繁子轨迹
*/
#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
int ansList[10000];			//压缩后的轨迹，测试解压用
//============

int save[10000];

int streetNumber;
int road2Street[2000000][2];	//i号路段的街道编号和街道偏移
int street2road[50000][200];	//i号街道j号偏移所对应路段

//频繁子轨迹的频繁度下界
#define MIN_FRE 2
//训练集中所有轨迹的路段总个数
#define EDGE_NUMBER 25000000
//训练集中轨迹条数
#define TRA_NUMBER 500000
//记录每个街道编号的2^i长度的频繁子轨迹以哪个路段终止。如果不存在，则为-1
int subtrajectory[600000][15];
//记录每个街道编号的2^i长度的频繁子轨迹右儿子在哪里开始
int submid[600000][15];

//记录每个街道编号的2^i长度的轨迹所经过的轨迹集合。这里用滚动数组来记录
int subpre[2][EDGE_NUMBER], subother[2][EDGE_NUMBER], sublast[2][600000];
int count[600000];
int edge_number = 0;

//临时记录的数组
int arr[TRA_NUMBER];

int father[600000][10],left[600000][10],right[600000][10],brother[600000],sister[600000];
//合并
int top[600000][2];
int visit[600000];
	
void output(int x, int y) {
	if (y > 0) {
		output(left[x][y],y-1);
		output(right[x][y],y-1);
	}
	else
	printf("%d ",x);
}

int main(int argc, char *argv[])
{
	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	loadData(argv[1]);

	
	//读入轨迹文件
	FILE* file = fopen(argv[2],"rb");
	
	int tjNumber = readInt(file);	//轨迹条数
	
	FILE* result = fopen(argv[3],"wb");	//用街道编号压缩
	
	int tra_id = 0;		//轨迹的id
	memset(subtrajectory,255,sizeof(subtrajectory));
	memset(submid,255,sizeof(submid));
	memset(sublast,255,sizeof(sublast));
	memset(count,0,sizeof(count));
	
	int i;
	for (i = 0; i < tjNumber; ++i) {
		memset(visit,0,sizeof(visit));
		++tra_id;
		int spNumber = readInt(file);	//路径段数
		//printf("%d:%d\n",i,spNumber);
		ansSize = 0;
		
		//记录原始的通过每条边的轨迹集合
		int j;
		for (j = 0; j < spNumber; ++j) {
			int e = readInt(file);	//路径上的边
			if (visit[e]) {
				++tra_id;
				memset(visit,0,sizeof(visit));
			}
			visit[e] = 1;
			if (sublast[0][e] == -1 || subother[0][sublast[0][e]] != tra_id) {
				//printf(" %d \n",e);
				++edge_number;
				subother[0][edge_number] = tra_id;
				subpre[0][edge_number] = sublast[0][e];
				sublast[0][e] = edge_number;
				++count[e];
			}
		}
		//puts("\n");
	}
	
	
	//2^0的频繁子轨迹
	for (i = 0; i < edgeNumber; ++i)
		if (count[i] >= MIN_FRE) {
			subtrajectory[i][0] = i;
		}
	
	/*int qq = 0;
	for (i = 0; i < edgeNumber; ++i)
		if (subtrajectory[i][0] > -1) {
			//printf("%d:%d %d\n",i,subtrajectory[i][0],count[i]);
			qq += count[i];
			int p = sublast[0][i];
			while (p > -1) {
				//printf("%d\t",subother[0][p]);
				p = subpre[0][p];
			}
			//puts("\n");
		}
	
	printf("%d\n",qq);
	*/
	
	//滚动上面的部分
	for (i = 1; i < 15; ++i) {	//2^i
	
		//printf("Now %d\n",i);	// 现在处理位置
		
		int now = i % 2;		//滚动数组
		int past = 1 - now;
		
		//printf("%d %d %d\n",i,past,now);
		
		int j;
		for (j = 0; j < 600000; ++j) sublast[now][j] = -1;
		
		edge_number = 0;	//构建新的邻接表
		
		
		for (j = 0; j < edgeNumber; ++j)				//对于每个在上一次的循环中活下来的起始端点
			if (subtrajectory[j][i-1] != -1) {
				
				int p = last[other[subtrajectory[j][i-1]]];		//另一个端点的连出边
				//if (i > 1) printf("%d %d %d--\t",p,sublast[past][p],subtrajectory[p][i-1]);
				
				int max_number = 0;
				int tmp,mid;
				
				while (p > -1) {								//循环每个出边
					
					int left = sublast[past][j];				//集合的最后位置
					int right = sublast[past][p];
					
					//printf("%d##\t",right);
					if (right > -1) {
						int mm = 0;
						
						//统计相同轨迹个数
						while (left > -1 && right > -1) {
							if (subother[past][left] == subother[past][right]) {
								++mm;
								left = subpre[past][left];
								right = subpre[past][right];
							}else
							if (subother[past][left] < subother[past][right]) right = subpre[past][right];
							else
							if (subother[past][left] > subother[past][right]) left = subpre[past][left];
						}
						
						//更新，保留最大的值
						if (mm > max_number) {
							max_number = mm;
							tmp = subtrajectory[p][i-1];
							mid = p;
						}
					}
					
					p = pre[p];
				}
				//printf("%d %d\t\t",j,max_number);
				//如果最大的值大于阈值，则保留，否则放弃
				if (max_number >= MIN_FRE) {
					
					subtrajectory[j][i] = tmp;
					submid[j][i] = mid;
					
					int left = sublast[past][j];				//集合的最后位置
					int right = sublast[past][mid];
					int mm = 0;
					
					//暂存临时的轨迹集合
					while (left > -1 && right > -1) {
						if (subother[past][left] == subother[past][right]) {
							arr[mm++] = subother[past][left];
							left = subpre[past][left];
							right = subpre[past][right];
						}else
						if (subother[past][left] < subother[past][right]) right = subpre[past][right];
						else
						if (subother[past][left] > subother[past][right]) left = subpre[past][left];
					}
					
					//存入邻接表
					int k;
					for (k = 0; k < mm; ++k) {
						++edge_number;
						subother[now][edge_number] = arr[k];
						subpre[now][edge_number] = sublast[now][j];
						sublast[now][j] = edge_number;
					}
					
				}
				
			}
		
	}
	
	//printf("%d\n",edgeNumber);
	
	
	for (i = 0; i < edgeNumber; ++i) {
		int j,k;
		//printf("%d:\t",i);
		for (j = 1; j < 15; ++j) 
			if (subtrajectory[i][j] > -1) {
				for (k = 0; k < j; ++k) if (subtrajectory[i][j] == subtrajectory[i][k]) {
					int l;
					for (l = j; l < 15; ++l) subtrajectory[i][l] = -1;
					break;
				}
				if (subtrajectory[i][j] == -1) break;
			}
		//puts("");
	}
	/*int tot = 0;
	for (i = 0; i < edgeNumber; ++i) {
		int j;
		for (j = 2; j < 15; ++j)
			if (subtrajectory[i][j] > -1) {
				++tot;break;
			}
	}*/
	
	//层次关系
	memset(father, 255,sizeof(father));
	for (i = 0; i < edgeNumber; ++i) {
		int j;
		for (j = 1; j < 10; ++j) if (subtrajectory[i][j] > -1){
			left[i][j] = i;
			right[i][j] = submid[i][j];
			father[i][j-1] = father[submid[i][j]][j-1] = i;
		}
	}
	
	
	int tot = 0;
	for (i = 0; i < edgeNumber; ++i) {
		int j;
		for (j = 0; j < 10; ++j) if (subtrajectory[i][j] > -1 && father[i][j] == -1) {
			top[tot][0] = i; top[tot][1] = j;
			++tot;
		}
	}
	
	/*
	for (i = 0; i < tot; ++i) {
		output(top[i][0],top[i][1]);
		printf("-1\n");
	}*/
	
	memset(brother,255,sizeof(brother));
	memset(sister,255,sizeof(sister));
	
	int j;
	for (i = 0; i < tot; ++i)
		for (j = 0; j < tot; ++j)
			if (i != j) {
				int tail = subtrajectory[top[i][0]][top[i][1]];
				int head = top[j][0];
				int p = last[other[tail]];
				int flag = 0;
				while (p > -1) {
					if (p == head) {
						flag = 1;break;
					}
					p = pre[p];
				}
				if (flag) {
					brother[i] = j;
					sister[j] = i;
				}
			}
	
	memset(visit,0,sizeof(visit));
	for (i = 0; i < tot; ++i) if (sister[i] == -1) {
		output(top[i][0],top[i][1]);visit[i] = 1;
		int tmp = i;
		while (brother[tmp] != -1 && !visit[brother[tmp]]) {
			tmp = brother[tmp];//printf("++++++++++++++++");
			output(top[tmp][0],top[tmp][1]);visit[tmp] = 1;
		}
		printf("-1\n");
	}
	for (i = 0; i < tot; ++i) if (!visit[i]) {
		output(top[i][0],top[i][1]);visit[i] = 1;
		int tmp = i;
		while (brother[tmp] != -1 && !visit[brother[tmp]]) {
			tmp = brother[tmp];printf("++++++++++++++++");
			output(top[tmp][0],top[tmp][1]);visit[tmp] = 1;
		}
		printf("-1\n");
	}
	
	fclose(result);
	fclose(file);
	
	return 0;
}
