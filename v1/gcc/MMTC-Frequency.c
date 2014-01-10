/**
	核心程序，进行MMTC的SPATIAl部分的压缩的程序
		之用路网进行划分
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

int ansList[1000000];
int path[1000000];
int ansSize;

int main(int argc, char *argv[])
{
	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}
	//Load Road Network from File
	loadData(argv[1]);
	
	//载入频繁序列信息
	loadFrequency();
	
	long bbb = clock();
	
	//1 读入轨迹文件
	FILE* file = fopen(argv[2],"rb");
	
	int tjNumber = readInt(file);	//轨迹条数
	
	FILE* result = fopen(argv[3],"wb");	//用街道编号压缩
	writeInt(result,tjNumber);
	
	int i;
	for (i = 0; i < tjNumber; ++i) {
		int spNumber = readInt(file);	//路径段数
		
		//--
		//printf("%d\n",spNumber);
		ansSize = 0;
		
		int j;
		for (j = 0; j < spNumber; ++j)
			path[j] = readInt(file);	//路径上的边
		
		tmp_num = 0;
		int p = fre_last[path[0]];
		while (p > -1) {
			tmp_item[tmp_num][0] = fre_other[p][0];
			tmp_item[tmp_num][1] = fre_other[p][1];
			tmp_item[tmp_num][2] = fre_other[p][1];
			++tmp_num;
			p = fre_pre[p];
		}
		
		//for (j = 0; j < tmp_num; ++j) printf("%d-%d-%d\t",tmp_item[j][0],tmp_item[j][1],tmp_item[j][2]);
		//puts("");
		//if (tmp_num == 0) ansList[ansSize++] = path[0];
		
		int nowEdge;
		int preEdge = 0;
		for (nowEdge = 1; nowEdge < spNumber; ++nowEdge) {
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
				if (tmp_num == 0 || tmp_item[0][1] == tmp_item[0][2]) {	//孤立道路
					ansList[ansSize++] = path[preEdge];
					//===
					//printf("Edge:%d\t",path[preEdge]);
					//printf("%d ",path[preEdge]);
				}else {				//一条子轨迹
					ansList[ansSize++] = (1 << 31) | ((tmp_item[0][0] % 32768) * 65536) + ((tmp_item[0][1] % 256) * 256) + (tmp_item[0][2] % 256);
					//===
					//if (tmp_item[0][1]!=tmp_item[0][2])
					//printf("Sub:%d^%d-%d\t",tmp_item[0][0],tmp_item[0][1],tmp_item[0][2]);
					//int k;
					//for (k = tmp_item[0][1]; k<= tmp_item[0][2]; ++k)
					//	printf("%d ",fre[tmp_item[0][0]][k]);
				}
				
				tmp_num = 0;
				int p = fre_last[path[nowEdge]];
				while (p > -1) {
					tmp_item[tmp_num][0] = fre_other[p][0];
					tmp_item[tmp_num][1] = fre_other[p][1];
					tmp_item[tmp_num][2] = fre_other[p][1];
					++tmp_num;
					p = fre_pre[p];
				}
				
				preEdge = nowEdge;
			}else {	//可以继续延长
				tmp_num = tp;
				for (j = 0; j < tmp_num; ++j) {
					tmp_item[j][0] = tmp_mm[j][0];
					tmp_item[j][1] = tmp_mm[j][1];
					tmp_item[j][2] = tmp_mm[j][2];
				}
			}
		}
		
		if (tmp_num == 0 || tmp_item[0][1] == tmp_item[0][2]) {	//孤立道路
			ansList[ansSize++] = path[preEdge];
			//===
			//printf("Edge:%d\n",path[preEdge]);
			//printf("%d ",path[preEdge]);
		}else {				//一条子轨迹
			ansList[ansSize++] = (1 << 31) | ((tmp_item[0][0] % 32768) * 65536) + ((tmp_item[0][1] % 256) * 256) + (tmp_item[0][2] % 256);
			//===
			//printf("Sub:%d^%d-%d\n",tmp_item[0][0],tmp_item[0][1],tmp_item[0][2]);
			//int k;
			//for (k = tmp_item[0][1]; k<= tmp_item[0][2]; ++k)
			//	printf("%d ",fre[tmp_item[0][0]][k]);
		}
		//puts("");
		for (j = 0; j < ansSize; ++j) writeInt(result,ansList[j]);//	printf("%d ",ansList[j]);
		
		//调试用
		if (i % 100 ==99) printf("+");
		if (i % 10000 ==9999) printf("\n");
	}
	
	fclose(result);
	fclose(file);
	FILE* fpp = fopen(argv[4],"a+");
	fprintf(fpp,"%ld\n",clock() - bbb);
	fclose(fpp);
	return 0;
}
