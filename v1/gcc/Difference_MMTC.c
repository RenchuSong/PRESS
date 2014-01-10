//查询某时刻每条轨迹在哪里，返回坐标对集合

#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

//计算几何
double circleDistance(double lat1,double long1,double lat2,double long2)
{
	double deltaLat=lat1-lat2;
	double deltaLong=(long2-long1)*cos(lat1 * PI180);
	return LENGTH_PER_RAD*sqrt(deltaLat*deltaLat+deltaLong*deltaLong);
}

double oriX[100000],oriY[100000],comX[100000],comY[100000];

int main(int argc, char *argv[]){
	
	FILE* precise = fopen(argv[1],"r");
	FILE* prox = fopen(argv[2],"r");
	
	//误差允许的上界
	char* ttt = argv[3];
	int i;
	int tolerance = 0;
	for (i = 0; i < strlen(ttt); ++i) tolerance = tolerance * 10 + ttt[i] - 48;
	
	int dataNumber,del;
	fscanf(precise,"%d",&dataNumber);
	//fscanf(prox,"%d",&del);

	double max = 0,all = 0;
	for (i = 0; i < dataNumber; ++i) {
		fscanf(precise,"%d",&del);
		fscanf(precise,"%lf %lf",&oriX[del],&oriY[del]);
	}
	
	for (i = 0; i < 9749; ++i) {
		//printf("%d\t",i);
		fscanf(prox,"%d",&del);
		fscanf(prox,"%lf %lf",&comX[del],&comY[del]);
		//printf("%lf %lf\n",comX[del],comY[del]);
		double tmp = circleDistance(oriX[del],oriY[del],comX[del],comY[del]);
		printf("%d  %lf\n",del, tmp);
	}
	
	fclose(precise);
	fclose(prox);
	return 0;
}
