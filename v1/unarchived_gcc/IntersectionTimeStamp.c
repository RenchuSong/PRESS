#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>

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


int roadSet[6000][100];
double probMul[6000][100];
int preLink[6000][100];

double velocity = 10;

//int tmpList[10000],roadList[10000],timeList[10000],spCount[10000];
//double prob[10000];

char inPath[20000][100];
char inPath2[20000][100];

int path[10000000];
double accDist[1000000];

int ansSize = 0;
int timeList[1000000];//0:时间	1:距离
double distList[1000000];


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

int Tf[70007] ;
int Tg[70007] ;
int Tstack[70007] ;
//bool Tvis[70007] ; 

/*void spfa(int i){
     memset(Tf , 0x3f , sizeof(Tf)) ;
     Mxn = Tf[0] ; 
     Tf[i] = 0 ;
  //   memset(Tg ,0 , sizeof(Tg)) ;
     int p = 0 , q = 1 ; Tstack[1] = i ;
     memset(Tvis , 0, sizeof(Tvis)) ;
     while (p != q){
           p = p % 70000 + 1 ;
           int x = stack[p] ;
           for (int t = last[x] ; t != 0 ; t = pre[t]){
               int y = other[t] ; 
               if (Tf[y] > Tf[x] + 1){
                  Tf[y] = Tf[x] + 1 ;
                  Tg[y] = x ;
                  if (!Tvis[q]){
                     q = q % 70000 + 1 ;
                     Tstack[q] = y ;
                     Tvis[q] = 1 ;
                  } 
               }
           }
           Tvis[x] = 0 ; 
     }
}*/
            
//const int AKN = 70007 ;
  
#define AKN 80007 
           
int Tra[AKN] ;
double Trc[AKN] ; 
int Gra[AKN] ;
double Grb[AKN] ;
double Gx[AKN] , Gy[AKN] ;
int Gst[AKN] ;
double lask , rask ;

double GDist(int x , int y){
       int st = x + 1 , ed = y ;
       double tot = 0 ;
      // if (st > ed) puts("AKAISB*N");
       while (st < ed){
               nodeToEdgeDistanceAndNodeSide(0, 0 , Tra[st] , &lask , &rask);
               tot += rask ;
               st ++ ;
       //        printf("%d %d\n" , st , ed);
       }
       return tot ;
}

int dataNumber;
char inPath1[20000][100];
char inPath2[20000][100];
char inPath3[20000][100];
char outPath[20000][100];

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

void getInputPathSet2(char* path)
{	
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath2[dataNumber],path);
		strcat(inPath2[dataNumber],"/");
		strcat(inPath2[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

void getInputPathSet3(char* path)
{	
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath3[dataNumber],path);
		strcat(inPath3[dataNumber],"/");
		strcat(inPath3[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}

void getOutputPath(char* path2, char* path)
{	
	dataNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(outPath[dataNumber],path2);
		strcat(outPath[dataNumber],"/");
		strcat(outPath[dataNumber],ent->d_name);
		++dataNumber;
	}
	closedir(dir);
}
 
int main(int argc, char *argv[]){

/*	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}*/
	//Load Road Network from File
	//loadData(argv[1]);
	loadData("./");
	
	//FILE* fp1 = fopen("trajectory/output_000000.txt" , "r" );
	//FILE* fp2 = fopen("output/output_000000.txt" , "r" );
	//FILE* fp3 = fopen("input/input_000000.txt" , "r" );
	
	getInputPathSet1(argv[1]);
	getInputPathSet2(argv[2]);
	getInputPathSet3(argv[3]);
	getOutputPath(argv[4],argv[3]);
	
	int qq;
	for (qq = 0; qq < dataNumber; ++qq) {
	if (qq % 100 == 0) printf("+");
	//puts(inPath1[qq]);
	//puts(inPath2[qq]);
	//puts(inPath3[qq]);
	//puts(outPath[qq]);
	
	FILE* fp1 = fopen(inPath1[qq] , "r" );
	FILE* fp2 = fopen(inPath2[qq] , "r" );
	FILE* fp3 = fopen(inPath3[qq] , "r" );
	FILE* fp4 = fopen(outPath[qq],"w");
	
	int n = 0 ;
          int x ;
    while (fscanf(fp1 , "%d" , &x) != EOF){
	      Tra[++n] = x ;
    }
    n-- ;
    //Get Every Cross Time !  
	int m = 0 ;
          int  y ;
          double x7 , z ;
          char ch1 , ch2 ;
    while ( fscanf(fp2 , "%lf,%d,%lf" , &x7 ,  &y ,&z) != EOF){
	      Gra[++m] = y ;
	      Grb[m] = x7 ;
    }
    m--;
    int tmp , i ;
    for (i = 1; i <= m ; i++)
        fscanf(fp3,"%d,%lf,%lf\n" , &tmp , &Gx[i] , &Gy[i]);
    int m1 = 0 ;
    for (i = 1 ; i <= m ; i++)
        if (Gra[i] != -1){
           Gra[++m1] = Gra[i] ;
           Grb[m1] = Grb[i] ;
           Gx[m1] = Gx[i] ;
           Gy[m1] = Gy[i] ;
        }
    m = m1 ;
    int j = 1 ;
    double cnt , tm ;
    int top = 0 ;
   
    for (i = 1 ; i <= n ; i++){
        if (Tra[i] != Gra[j]) continue ;
        while (Gra[j] == Tra[i] && j <= m){
              Gst[j] = i ;
              j++ ;
        }
    }
    
    j = 1 ; 
    for (i = 1 ; i <= n + 1 ; i++){
        while (i > Gst[j] && j <= m)j ++ ; 
        double x2 , y2  , t2 , x3 , y3 , t3 ;
        if (i == 1){
             t2 = Grb[1] ; t3 = Grb[2] ;
             double cntotlen = GDist(Gst[1] , Gst[2]) ;
             nodeToEdgeDistanceAndNodeSide(Gx[2] , Gy[2] , Gra[2] , &cnt , &tm);
             double cntdlen = cnt ;
             nodeToEdgeDistanceAndNodeSide(Gx[1],Gy[1],Gra[m], &cnt , &tm) ;
             Trc[i] = t2 - fabs(t3-t2) * cnt / fabs( cnt+ cntotlen + tm + cntdlen)  ;
        }
        else if (i == n + 1){
             t2 = Grb[m-1] ; t3 = Grb[m] ;
             double cntotlen = GDist(Gst[m-1] , Gst[m]);
             nodeToEdgeDistanceAndNodeSide(Gx[m-1] , Gy[m-1] , Gra[m-1] , &cnt,  &tm);
             double cntdlen = tm - cnt ; 
             nodeToEdgeDistanceAndNodeSide(Gx[m],Gy[m],Gra[m], &cnt , &tm) ;
             Trc[i] = fabs(t3-t2) * fabs(tm - cnt) / fabs( tm - cnt + cntotlen + tm + cntdlen) + t2 ;
        }
        else {
             t2 = Grb[j-1] ; t3 = Grb[j] ;
             double cntrlen = GDist(Gst[j-1] , i);
             double cntotlen = GDist(Gst[j-1] , Gst[j]);
             nodeToEdgeDistanceAndNodeSide(Gx[j-1] , Gy[j-1] , Gra[j-1] , &cnt,  &tm);
             double cntdlen = tm - cnt ;
             nodeToEdgeDistanceAndNodeSide(Gx[j] , Gy[j] , Gra[j] , &cnt,  &tm);
             Trc[i] = (double)fabs(t3 - t2) * fabs(cntrlen + cntdlen) / fabs(cntotlen + cntdlen + cnt) + t2 ;
        }
      }
      //freopen("TestOut/testout0.txt", "w" , stdout);
   /*   int asdfghjkl = 1 ;
      for (i = 1 ; i <= n + 1 ; i++){
          while (Gst[asdfghjkl] == i && asdfghjkl <= m ){ printf("%d %.3lf 1\n",Gra[asdfghjkl] , Grb[asdfghjkl]); asdfghjkl ++ ;} 
          printf("%d %.3lf\n" ,Tra[i] , Trc[i]);      
          puts("--------------");
      }*/
      
      // 说明： Tra是路段ID  Trc是该段路段起点的时间(double)
      // 共n个路段 n + 1 个时间戳
      //下面是输出 可以看一下 
	  //if (Trc)
      for (i = 1 ; i <= n+1 	 ; i++){
          if (Trc[i-1] > Trc[i]) printf("%d:%lf %lf\n",i,Trc[i-1],Trc[i]);
          fprintf(fp4,"%d %.3lf\n" , qq,Tra[i] , Trc[i]) ;
      }
	  puts("");
	  fclose(fp1);fclose(fp2);fclose(fp3);fclose(fp4);
	}
	return 0;
}
