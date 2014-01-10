#include <math.h>
#include <stdlib.h>
#include "define.h"
#include "geometry.h"

#define arch(x) ((x)*PI/180)											//角度转弧度
#define stand(x) ((x)<0?((x)>-180?-(x):(x)+360):((x)>180?360-(x):(x)))	//角度归正到小角度

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
 *description:计算球面一点到一条路径的距离,并且将第三个参数置成边起点到最近点的距离
 *param:int 点标号,int 边标号
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

/**
 * Compute the shortest path length between two Nodes（Dijkstra Algorithm）
 * node1：the beginning node ID
 * node2：the ending node ID
 * maxLen：a threshold value（once current shortest path length is longer than it, return INACCESSIBLE）
 */

#define N_DIJKSTRA_NODE 10000
#define INACCESSIBLE 1e40
#define NONE -1

struct H_DijkstraNode {
	int id;
	double distance;
};


void FixUp(struct H_DijkstraNode d[], int k, int *DijkstraIndex)
{
	int m;
	int t_id = d[k].id;
	double t_dist = d[k].distance;
	for (m = (k - 1) / 2; k > 0 && d[m].distance > t_dist; k = m, m = (m - 1) / 2) {
		d[k].id = d[m].id;
		d[k].distance = d[m].distance;
		DijkstraIndex[d[k].id] = k;
	}
	d[k].id = t_id;
	d[k].distance = t_dist;
	DijkstraIndex[d[k].id] = k;
}

void FixDownR(struct H_DijkstraNode d[], int k, int N, int *DijkstraIndex)
{
	int j;
	int t_id = d[k].id;
	double t_dist = d[k].distance;
	while (1) {
		j = 2 * k + 1;
		if (j + 1 < N && d[j].distance > d[j + 1].distance)
			++j;
		else if (j >= N)
			break;
		if (t_dist < d[j].distance) break;
		d[k].id = d[j].id;
		d[k].distance = d[j].distance;
		DijkstraIndex[d[k].id] = k;
		k = j;
	}
	d[k].id = t_id;
	d[k].distance = t_dist;
	DijkstraIndex[d[k].id] = k;
}

double H_GetShortestPathLength(int node1, int node2, double maxLen, int *DijkstraIndex)
{
	if (node1 == node2) return 0.0;

	const int NodeCount = nodeNumber;
	int nHeap, nSelected, p;
	struct H_DijkstraNode d[N_DIJKSTRA_NODE + 1];

	d[N_DIJKSTRA_NODE].id = node1;
	d[N_DIJKSTRA_NODE].distance = 0.0;
	DijkstraIndex[node1] = N_DIJKSTRA_NODE;
	nSelected = 1;
	for (nHeap = 0, p = last[node1]; p != NONE; ++nHeap, p = pre[p]) {
		d[nHeap].id = other[p];
		d[nHeap].distance = len[p];
		DijkstraIndex[d[nHeap].id] = nHeap;
	}
	int k;
	for (k = nHeap / 2 - 1; k >= 0; --k)
		FixDownR(d, k, nHeap, DijkstraIndex);

	int nLoop;
	int distNodeID, distNodeIndex;
	int minNowId;
	double minNowDistance;
	for (nLoop = 0; nLoop < NodeCount; ++nLoop) {

		if (nHeap <= 0 || d[0].distance > maxLen)
			return INACCESSIBLE;
		if (d[0].id == node2)
			return d[0].distance;

		d[N_DIJKSTRA_NODE - nSelected].id = minNowId = d[0].id;
		d[N_DIJKSTRA_NODE - nSelected].distance = minNowDistance = d[0].distance;
		DijkstraIndex[minNowId] = N_DIJKSTRA_NODE - nSelected++;
		--nHeap;
		d[0].distance = d[nHeap].distance;
		d[0].id = d[nHeap].id;
		DijkstraIndex[d[nHeap].id] = 0;
		FixDownR(d, 0, nHeap, DijkstraIndex);
		p = last[minNowId];

		for (; p != NONE; p = pre[p]) {
			distNodeID = other[p];
			distNodeIndex = DijkstraIndex[distNodeID];
			if (distNodeIndex > N_DIJKSTRA_NODE - nSelected && d[distNodeIndex].id == distNodeID)
				continue;
			else if (distNodeIndex != -1 && distNodeIndex < nHeap && d[distNodeIndex].id == distNodeID) {
				if (d[distNodeIndex].distance > minNowDistance + len[p]) {
						d[distNodeIndex].distance = minNowDistance + len[p];
						FixUp(d, distNodeIndex, DijkstraIndex);
				}
			} else {
				d[nHeap].id = distNodeID;
				d[nHeap].distance = minNowDistance + len[p];
				DijkstraIndex[distNodeID] = nHeap;
				++nHeap;
				FixUp(d, nHeap - 1, DijkstraIndex);
			}
		}
	}
	return INACCESSIBLE;
}
