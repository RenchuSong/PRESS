#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>

int main(int argc, char *argv[]){

	loadData("./");
	
	double maxX = -1e100, maxY = -1e100, minX = 1e100, minY = 1e100;
	
	int i,j,k,l,m,n;
	
	for (i = 0; i < nodeNumber; ++i) {
		if (coordNode[i*2] > maxX)  maxX = coordNode[i*2];
		if (coordNode[i*2] < minX)  minX = coordNode[i*2];
		
		if (coordNode[i*2+1] > maxY)  maxY = coordNode[i*2+1];
		if (coordNode[i*2+1] < minY)  minY = coordNode[i*2+1];
		
	}
	
	double dx = maxX - minX, dy = maxY - minY;
	
	printf("%lf %lf %lf %lf\n",minX, minY, minX + dx / 2, minY + dy/2);
	printf("%lf %lf %lf %lf\n",minX + dx / 2, minY + dy/2, minX + dx, minY + dy);
	printf("%lf %lf %lf %lf\n",minX + dx / 2, minY, minX + dx, minY + dy/2);
	printf("%lf %lf %lf %lf\n",minX, minY+ dy/2, minX + dx / 2, minY + dy);
	printf("%lf %lf %lf %lf\n",minX + dx / 4, minY + dy/4, minX + dx*3/4, minY + dy*3/4);
	
	return 0;
}
