#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "define.h"
#include "geometry.h"

#define SIGMAZ 4.591689
//N2_SIGMAZ2 = -2 * SIGMAZ * SIGMAZ
#define N2_SIGMAZ2 -0.0237151062104234
//SQR_2PI_SIGMAZ = sqrt(2 * PI) * SIGMAZ
#define SQR_2PI_SIGMAZ 0.0868835586212075


double prob1(double dist){
	return exp(dist * dist * N2_SIGMAZ2) * SQR_2PI_SIGMAZ;
}


void coreDP(int dataId, long double dp[][500], int dpSize[], int dpEdge[][500], double dpLeft[][500], double dpLen[][500], int dpPre[][500]){
	double leftDist,dist,roadLen,theDP[1000];
	int i,j,k,result[1000],resNum;
	
	int *IndexPointer = (dataId & 1)? DijkstraIndex: DijkstraIndex2;
	
	int oriGrid=inWhichGrid(nodeSequence[dataId][0],nodeSequence[dataId][1]);
	getRelateEdgeSet(oriGrid,result,&resNum);
	dpSize[0]=0;

	long double midPro = -1e10;
	for (i = 0; i < resNum; ++i) {
		dist = nodeToEdgeDistanceAndNodeSide(nodeSequence[dataId][0],nodeSequence[dataId][1], result[i], &leftDist, &roadLen);
		if (dist > 50.0) continue;
		dpEdge[0][dpSize[0]]=result[i];
		dpLeft[0][dpSize[0]]=leftDist;
		dpLen[0][dpSize[0]]=roadLen;
		dp[0][dpSize[0]]=prob1(dist);
		if (dp[0][dpSize[0]] > midPro) midPro=dp[0][dpSize[0]];
		++dpSize[0];
	}
	
	int tmpCount = 0;
	for (i = 0; i < dpSize[0]; ++i)
		if (dp[0][i] > 1e-150){
			if (tmpCount<i){
				dpEdge[0][tmpCount] = dpEdge[0][i];
				dpLeft[0][tmpCount] = dpLeft[0][i];
				dpLen[0][tmpCount] = dpLen[0][i];
				dp[0][tmpCount] = dp[0][i];
			}
			++tmpCount;
		}
	dpSize[0]=tmpCount;
	for (i = 0; i < dpSize[0]; ++i) dp[0][i] /= midPro;

	int st;
	long double maxPr = -1e10;
	
	//记录断头的位置
	int cutFlag=0;
	long double BT=(long double)BETA_ARR[rate[dataId]];
	int tTime = nowTime[dataId];
	for (i = 1; i < tTime; ++i) {
		dpSize[i]=0;
		int tpN = 0, n2 = 0;
		double tmpLen;
		int tmpGrid=inWhichGrid(nodeSequence[dataId][i << 1],nodeSequence[dataId][(i << 1) + 1]);
		
		if (tmpGrid!=oriGrid){
			getRelateEdgeSet(tmpGrid,result,&resNum);
			oriGrid=tmpGrid;
		}
		
		for (j = 0; j < resNum; ++j) {
			dist = nodeToEdgeDistanceAndNodeSide(nodeSequence[dataId][i << 1],nodeSequence[dataId][(i << 1)+1],result[j],&leftDist,&roadLen);
			if (dist > 50.0) continue;
			dpEdge[i][dpSize[i]] = result[j];
			dpLeft[i][dpSize[i]] = leftDist;
			dpLen[i][dpSize[i]] = roadLen;
			dp[i][dpSize[i]] = prob1(dist);
			++dpSize[i];
		}
		
		double shortPathLength;
		
		for (j = 0; j < dpSize[i]; ++j) {

			midPro = -1.0;
			double pathDist;
			long double tmp;
			double s2;
			for (k = 0; k < dpSize[i-1]; ++k) {
				if (dpEdge[i - 1][k] == dpEdge[i][j])
					pathDist = fabs(dpLeft[i][j] - dpLeft[i - 1][k]);
				else
					pathDist = dpLeft[i][j] + dpLen[i - 1][k] - dpLeft[i - 1][k] + H_GetShortestPathLength(other[dpEdge[i - 1][k]], thisSide[dpEdge[i][j]], (double)(50 * (timeSequence[dataId][i] - timeSequence[dataId][i - 1])), IndexPointer);
				double driveLength = circleDistance(nodeSequence[dataId][(i << 1) - 2],nodeSequence[dataId][(i << 1) - 1],nodeSequence[dataId][(i << 1)],nodeSequence[dataId][(i << 1) + 1]);
				
				tmp=exp(-fabs((long double)driveLength-(long double)pathDist)/BT)/BT;
				
				tmp*=len[dpEdge[i][j]]/cost[dpEdge[i][j]];
				

				if (dp[i-1][k]*tmp>midPro){
					midPro=dp[i-1][k]*tmp;
					dpPre[i][j]=k;
				}
			}
			dp[i][j]*=midPro;
		}
		
		
		if (dpSize[i] > 10) {
			tmpCount=0;
			for (j = 0; j < dpSize[i]; ++j)
				if (dp[i][j] > 1e-50) {
					if (tmpCount<j){
						dpEdge[i][tmpCount] = dpEdge[i][j];
						dpLeft[i][tmpCount] = dpLeft[i][j];
						dpLen[i][tmpCount] = dpLen[i][j];
						dpPre[i][tmpCount] = dpPre[i][j];
						dp[i][tmpCount] = dp[i][j];
					}
					++tmpCount;
				}
			dpSize[i]=tmpCount;
		}
		midPro=-1e10;
		for (j=0;j<dpSize[i];++j)
			if (dp[i][j]>midPro) midPro=dp[i][j];
		
		
		//Case 2 概率断掉（断头路，路径的特殊情况） 重新开始一段
		if (midPro<1e-100){
			int ii;
			for (ii=0;ii<dpSize[i-1];++ii)
				if (maxPr<dp[i-1][ii]){
					maxPr=dp[i-1][ii];
					st=ii;
				}
			for (ii=i-1;ii>=cutFlag;--ii){
				ans[dataId][ii]=dpEdge[ii][st];
				st=dpPre[ii][st];
			}
			cutFlag=i;
			dpSize[i]=0;
			
			tmpGrid=inWhichGrid(nodeSequence[dataId][i*2],nodeSequence[dataId][i*2+1]);
			if (tmpGrid!=oriGrid){
				getRelateEdgeSet(tmpGrid,result,&resNum);
				oriGrid=tmpGrid;
			}
			
			for (j=0;j<resNum;++j){
				dist=nodeToEdgeDistanceAndNodeSide(nodeSequence[dataId][i << 1],nodeSequence[dataId][(i << 1)+1],result[j],&leftDist,&roadLen);
				if (dist>50) continue;
				dpEdge[i][dpSize[i]]=result[j];
				dpLeft[i][dpSize[i]]=leftDist;
				dpLen[i][dpSize[i]]=roadLen;
				dp[i][dpSize[i]]=prob1(dist);
				++dpSize[i];
			}
		}else
		for (j=0;j<dpSize[i];++j) {
			dp[i][j]/=midPro;
		}
	}
	maxPr=-1e10;
	for (i=0;i<dpSize[nowTime[dataId]-1];++i)
		if (maxPr<dp[nowTime[dataId]-1][i]){
			maxPr=dp[nowTime[dataId]-1][i];
			st=i;
		}
	for (i=nowTime[dataId]-1;i>=cutFlag;--i){
		ans[dataId][i]=dpEdge[i][st];
		st=dpPre[i][st];
	}	
}
