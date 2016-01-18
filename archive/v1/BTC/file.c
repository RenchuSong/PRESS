#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <io.h>  
#include <fcntl.h>  
#include <process.h>  
#include <sys/stat.h>
#include <string.h>
#include "define.h"

#define NODE_INPUT_FILE "/WA_Nodes.txt"
#define EDGE_INPUT_FILE "/WA_Edges.txt"
#define NET_INPUT_FILE "/WA_EdgeGeometry.txt"

/**
 * Power of 10
 */
static const double POWER10[50] = {
	1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
	1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
	1e20, 1e21, 1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28, 1e29,
	1e30, 1e31, 1e32, 1e33, 1e34, 1e35, 1e36, 1e37, 1e38, 1e39,
	1e40, 1e41, 1e42, 1e43, 1e44, 1e45, 1e46, 1e47, 1e48, 1e49
};

char *readBuffer;		//读文件缓存
char *t;
int nowSize = 300000000;	//文件现在大小

void FileError(char *strErr)
{
	printf("Load File Error: %s\n", strErr);
	exit(1);
}

/**
 * Get Double from String
 */
static double P_ParseDouble()
{
	double val;
	int sign = 1, power;

	if (*t == '-') {
		++t;
		sign = -1;
	} else if (*t == '+') {
		++t;
	}
	for (val = 0.0; *t >= '0' && *t <= '9'; ++t)
		val = 10.0 * val + (*t - '0');
	if (*t == '.') {
		++t;
		for (power = 1; *t >= '0' && *t <= '9'; ++t) {
			val = 10.0 * val + (*t - '0');
			++power;
		}
		val /= POWER10[power - 1];
	}
	return sign * val;
}


/**
 * Get Integer from String
 */
static int P_ParseInteger()
{
	int result = 0;

	for (; *t >= '0' && *t <= '9'; ++t)
		result = result * 10 + *t - '0';
	return result;
}

/**
 * Load Nodes from file
 */
void LoadNode(char *path)
{
	char fullPath[1000];
	strcpy(fullPath, path);
	strcat(fullPath, NODE_INPUT_FILE);
	int pointerFile = open(fullPath, O_RDONLY);
	if (pointerFile == -1)
		FileError(NODE_INPUT_FILE "can not open!");
	int size = read(pointerFile, readBuffer, nowSize);
	if (readBuffer[size - 1] != '\n')
		readBuffer[size++] = '\n';
	readBuffer[size++] = '\0';
	coordNode=(double *)malloc((originNodeNumber << 1) * sizeof(double));

	int j, k = -1;
	t = readBuffer;
	while (*t != '\0') {
		j = P_ParseInteger(); ++t;
		if (j > k) k = j;
		coordNode[j << 1] = P_ParseDouble(); ++t;
		coordNode[(j << 1) + 1] = P_ParseDouble(); ++t;
	}
	nodeNumber = k + 1;

	close(pointerFile);
}

/**
 * Load Edges from file
 */
void LoadEdge(char* path){
	char fullPath[1000];
	strcpy(fullPath,path);
	strcat(fullPath,EDGE_INPUT_FILE);
	int pointerFile = open(fullPath, O_RDONLY);
	if (pointerFile == -1)
		FileError(EDGE_INPUT_FILE "can not open!");
	int size = read(pointerFile, readBuffer, nowSize);
	if (readBuffer[size - 1] != '\n')
		readBuffer[size++] = '\n';
	readBuffer[size++] = '\0';

	int index_size = (originEdgeNumber << 1) * sizeof(int);
	pre = (int*)malloc(index_size);
	other = (int*)malloc(index_size);
	thisSide=(int*)malloc(index_size);
	cost=(int*)malloc(index_size);
	last=(int*)malloc(index_size);
	memset(last,255,index_size);

	int i, j, k = -1;
	int r1, r2, l;
	t = readBuffer;
	while (*t != '\0') {
		j = P_ParseInteger(); ++t;
		if (j > k) k = j;
		r1 = P_ParseInteger(); ++t;
		r2 = P_ParseInteger(); ++t;
		l = P_ParseInteger(); ++t;
		pre[j]=last[r1];
		last[r1]=j;
		cost[j]=l;
		other[j]=r2;
		thisSide[j]=r1;
	}
	edgeNumber = k + 1;

	close(pointerFile);
}

/**
 * Load Edge Geometry from file
 */
void LoadNet(char* path)
{
	char fullPath[1000];
	strcpy(fullPath,path);
	strcat(fullPath,NET_INPUT_FILE);
	int pointerFile = open(fullPath, O_RDONLY);
	if (pointerFile == -1)
		FileError(NET_INPUT_FILE "can not open!");
	int size = read(pointerFile, readBuffer, nowSize);
	if (readBuffer[size - 1] != '\n') {
		readBuffer[size++] = '\n';
	}
	readBuffer[size++] = '\0';

	double x,y;
	edgeStart=(int*)malloc(originEdgeNumber * sizeof(int));
	coordNet=(double*)malloc((originLineNumber << 1) * sizeof(double));
	len=(double*)malloc(originLineNumber * sizeof(double));

	int i = 0, j, k, m = 0;
	t = readBuffer;

	//separate the first loop from others to simplfy the loop
	{
		j = P_ParseInteger(); ++t;
		for (; *t != '^'; ++t); ++t;
		for (; *t != '^'; ++t); ++t;
		len[j] = P_ParseDouble(); ++t;
		edgeStart[j] = m;
		{
			x = P_ParseDouble(); ++t;
			y = P_ParseDouble();
			coordNet[m << 1] = x;
			coordNet[(m << 1) + 1] = y;
			minLat = maxLat = x;
			minLong = maxLong = y;
			++m;
		}
		while (*(t++) == '^') {
			x = P_ParseDouble(); ++t;
			y = P_ParseDouble();
			coordNet[m << 1] = x;
			coordNet[(m << 1) + 1] = y;
			if (x < minLat)
				minLat = x;
			else if (x > maxLat)
				maxLat = x;
			if (y < minLong)
				minLong = y;
			else if (y > maxLong)
				maxLong = y;
			++m;
		}
	}
	while (*t != '\0') {
		j = P_ParseInteger(); ++t;
		for (; *t != '^'; ++t); ++t;
		for (; *t != '^'; ++t); ++t;
		len[j] = P_ParseDouble();
		edgeStart[j] = m;
		while (*(t++) == '^') {
			x = P_ParseDouble(); ++t;
			y = P_ParseDouble();
			coordNet[m << 1] = x;
			coordNet[(m << 1) + 1] = y;
			if (x < minLat)
				minLat = x;
			else if (x > maxLat)
				maxLat = x;
			if (y < minLong)
				minLong = y;
			else if (y > maxLong)
				maxLong = y;
			++m;
		}
	}
	++j;
	edgeStart[j] = m;
	close(pointerFile);
	lineNumber = m;
}

/**
 * Load Test File
 */
void loadSequence(int dataId,char* str){
	int tRate = 0;
	int pointerFile = open(str, O_RDONLY);
	if (pointerFile == -1) {
		FileError("test file can not open!");
	}
	int size = read(pointerFile, readBuffer, nowSize);

	if (readBuffer[size - 1] != '\n') {
		readBuffer[size++] = '\n';
	}
	readBuffer[size++] = '\0';
	int i = 2, j = 1;
	t = readBuffer;
	timeSequence[dataId][0] = P_ParseInteger(); ++t;
	nodeSequence[dataId][0] = P_ParseDouble(); ++t;
	nodeSequence[dataId][1] = P_ParseDouble(); ++t;
	while (*t != '\0') {
		timeSequence[dataId][j] = P_ParseInteger(); ++t;
		nodeSequence[dataId][i++] = P_ParseDouble(); ++t;
		nodeSequence[dataId][i++] = P_ParseDouble(); ++t;
		tRate += (timeSequence[dataId][j] - timeSequence[dataId][j - 1]);
		++j;
	}
	nowTime[dataId] = j;
	if (j < 2) j = 2;
	tRate /= (j - 1);
	if (tRate < 1)
		tRate = 1;
	else if (tRate > 30)
		tRate = 30;
	rate[dataId] = tRate;

	close(pointerFile);
}

/**
 * Load Road Network Data
 */
void loadData(char *path)
{
	readBuffer = (char *)malloc(nowSize);
	LoadNode(path);
	LoadEdge(path);
	LoadNet(path);
}

void FreeBuffer()
{
	free(readBuffer);
}
