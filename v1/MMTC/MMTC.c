#include <stdio.h>
#include <time.h>
#include "define2.h"
#include "file.h"
#include "geometry.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>

double min(double x , double y){
       if (x < y) return x ; return y ;
}
double max(double x , double y){
       if (x > y) return x ; return y ;
}

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

double ppppp;

int roadSet[6000][100];
double probMul[6000][100];
int preLink[6000][100];

double velocity = 10;

//int tmpList[10000],roadList[10000],timeList[10000],spCount[10000];
//double prob[10000];

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

int Tstack[30007] ;
int Tvis[30007] ; 
double Tlen[30007] ; 
short des[20801][20801] ; 
int pw[30007] ;
short rew[20801] ; 



#define AKN 80007 
           
int Tra[AKN] ;
double Trc[AKN] ; 
int Gra[AKN] ;
double Grb[AKN] ;
double Gx[AKN] , Gy[AKN] ;
int Gst[AKN] ;
double lask , rask ;
int Mxn ; 



void fspfa(){
     int w ; 
     for (w = 0 ; w <= 20800 ; w++){
         int j ;
         for (j = 0 ; j <= 20800 ; j++) rew[j] = 21000 ;
         rew[w] = 0 ;
         int p = 0 , q = 1 ; Tstack[1] = w ; Tvis[w] = 1 ; 
         memset(Tvis , 0, sizeof(Tvis)) ;
         while (p != q){
               p = p % 70000 + 1 ;
               int x = Tstack[p] ;
               int t ;
               for (t = last[x] ; t != 0 ; t = pre[t]){
               int y = other[t] ; 
               if (rew[y] > rew[x] + 1){
                  rew[y] = rew[x] + 1 ;
                  if (!Tvis[y]){
                     q = q % 70000 + 1 ;
                     Tstack[q] = y ;
                     Tvis[y] = 1 ;
                  } 
               }
           }
           Tvis[x] = 0 ; 
         }
         for (j = 0 ; j <= w ; j++) des[w][j] = rew[j] ; 
        // if (w <=200){
        //       for (j = 0 ; j <= 200 ; j++) printf("%d " , rew[j]);printf("\n");
        // }
         //if (w > 20800) system("pause");
         //if (w % 1000 == 0) printf("%d\n" , w);
     }         
}


int Tf[30007] ;
int Tg[30007] ;

void spfa(int i){
     memset(Tf , 0x3f , sizeof(Tf)) ;
     int Mxn = Tf[0] ; 
     Tf[i] = 0 ;
  //   memset(Tg ,0 , sizeof(Tg)) ;
     int p = 0 , q = 1 ; Tstack[1] = i ; Tvis[i] = 1 ;
     memset(Tvis , 0, sizeof(Tvis)) ;
     while (p != q){
           p = p % 70000 + 1 ;
           int x = Tstack[p] ;
           int t ;
           for (t = last[x] ; t != 0 ; t = pre[t]){
               int y = other[t] ; 
               if (Tf[y] > Tf[x] + 1){
                  Tf[y] = Tf[x] + 1 ;
                  Tg[y] = x ;
                  nodeToEdgeDistanceAndNodeSide(0 , 0 ,t ,&lask ,&rask);
                  Tlen[y] = rask ; 
                  if (!Tvis[y]){
                     q = q % 70000 + 1 ;
                     Tstack[q] = y ;
                     Tvis[y] = 1 ;
                  } 
               }
           }
           Tvis[x] = 0 ; 
     }
}
       
int Tbx[30001] ;
double Tatime[30001] ;
double Tbtime[30001] ; 
       
void get_time_values(int st , int ed , double cxtime , double dxtime){
       int cnt = ed ;
       double tot = 0 ;
       while (cnt != st){
             tot += Tlen[cnt] ;
             cnt = Tg[cnt] ;
       }
       cnt = ed ; 
       double tmp = 0 ;
       int cx = 0 ; 
       while (cnt != st){
             cnt = Tg[cnt] ;
             double ttime = cxtime - (dxtime * tmp / tot);
             tmp += Tlen[cnt] ; 
             Tatime[++cx] = ttime ;
       }
}

double Gdl[40001] ; 
double Fdl[40001] ;

double get_MDL(int st , int ed , int st1 , int ed1){
     int i ; 
     int wtot = 0 ;
     int cnt = ed ;
       while (cnt != st){
             Tbx[++wtot] = cnt ; 
             cnt = Tg[cnt] ;
           //  wtot ++ ;
       }
     for (i = wtot - 1 ; i >= 1 ; i--){
         Tbtime[i] = Tatime[wtot - i] ; 
     }
	  for (i = (wtot - 1) / 2 ; i >= 1 ; i--){
		int t = Tbx[i] ; Tbx[i] = Tbx[wtot-i]  ; Tbx[wtot-i] = t ;
      } 
   //  printf("%d %d %d\n" , wtot , st1 , ed1);
     for (i = 1 ; i < wtot ; i++){
         int j ; 
          for (j = st1 + 1 + i - 1 ; j < ed1 ; j++) Fdl[j] = 10000000 ;
		  Fdl[ed - 1] = 0;
         for (j = st1 + 1 + i - 1 ; j < ed1 ; j++){
             if (j - st1 == i)
             Gdl[j] = 100000000 ;
             else Gdl[j] = Gdl[j-1] ;
             int x5 = Tbx[i] , y5 = thisSide[Tra[j]] ;
             if (x5 > y5){
                x5 = x5 ^ y5 ; y5 = y5 ^ x5 ; x5 = x5 ^ y5 ;
             } 
             Gdl[j] = min(Fdl[j-1]+fabs(atan(des[x5][y5]*1.0/(10.0) /((wtot)*1.0)))/3.1415926535 
             + fabs(  
             ( fabs(Tbtime[i]-Tbtime[i-1]) - fabs(Trc[j] - Trc[j-1]) )
                 /max(fabs(Tbtime[i]-Tbtime[i-1]) , fabs(Trc[j] - Trc[j-1]))
                /(wtot*1.0) ),Gdl[j]) ;
             //printf("%.3lf\n" , Fdl[j-1]+des[x5][y5]*1.0/((wtot + 1)*1.0) 
            // + fabs( (Tbtime[i] - Trc[j])/(wtot*1.0) ));
           // system("pause");
         }
         for (j = st1 + 1 + i - 1 ; j < ed1 ; j++)
             Fdl[j] = Gdl[j] ;   
     }
	 
	 double tttt = Fdl[ed1-1]/2.0;
	 if (tttt > 1 || tttt < 0) tttt = 0.5;
	 //if (Fdl[ed1-1]/2.0 > 1 || Fdl[ed1-1]/2.0 < 0)
     //printf("%.3lf\n" , Fdl[ed1-1]/2.0) ; //system("pause");
	 //int p = 1;
     return  ppppp*log(tttt)/log(2.0) + log( (wtot+1)/(1.0*(ed1 - st1 + 1)) )/log(2.0) ; 
}


//const int AKN = 70007 ;
  
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
 
int atop ; 
int ansx[40001] ;
double anst[40001] ;

int ttNumber = 0;


char inPath1[20000][100];
char inPath2[20000][100];
char inPath3[20000][100];

void getInputPathSet1(char* path)
{
	ttNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath1[ttNumber],path);
		strcat(inPath1[ttNumber],"/");
		strcat(inPath1[ttNumber],ent->d_name);
		++ttNumber;
	}
	closedir(dir);
}

void getInputPathSet2(char* path)
{
	ttNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath2[ttNumber],path);
		strcat(inPath2[ttNumber],"/");
		strcat(inPath2[ttNumber],ent->d_name);
		++ttNumber;
	}
	closedir(dir);
}

void getInputPathSet3(char* path)
{
	ttNumber = 0;
	struct dirent* ent = NULL;
	DIR *dir;
	dir = opendir(path);
	while((ent = readdir(dir))!= NULL){ 
		if (ent->d_name[0]=='.') continue;
		strcpy(inPath3[ttNumber],path);
		strcat(inPath3[ttNumber],"/");
		strcat(inPath3[ttNumber],ent->d_name);
		++ttNumber;
	}
	closedir(dir);
}

double ylq[40] = {0,0.0001,0.005,0.01,0.015,0.2,0.025,0.03,0.06,0.09,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,200,300};

int oriNode[50000],comNode[50000];
double oriTime[50000],comTime[50000];

int main(int argc, char *argv[]){
	int aaa = 0;

/*	if (argc < 4) {
		puts("Argument Exception!");
		return 1;
	}*/
	//Load Road Network from File
	//loadData(argv[1]);
	loadData("./");
	
	fspfa() ;	//20s
	
	getInputPathSet1(argv[1]);	//input
	getInputPathSet2(argv[2]);	//output
	getInputPathSet3(argv[3]);	//trajectory
	
	
	
	int i = 0;long bbb;
	/*char* ttt = argv[4];
	ppppp = 0;
	for (i = 0; i < strlen(ttt); ++i) ppppp = ppppp * 10 + ttt[i] - 48;
	printf("%d\n",ppppp);*/
	
	int qq;
	
	puts("Here");
	
	long timeConsume = 0;
	
	int src;
	for (src = 0; src < 40; ++src) {
	FILE* fpp = fopen(argv[5],"a+");
	ppppp = ylq[src];
	
	double compress = 0, total = 0, similar = 0;
	
	double deviation = 0;
	double traNumber = 0;
	int oriTotal = 0, comTotal = 0;
	
	for (qq = 28; qq < ttNumber; qq+=10, ++aaa) {
	FILE* fp1 = fopen(inPath3[qq] , "r" );
	FILE* fp2 = fopen(inPath2[qq] , "r" );
	FILE* fp3 = fopen(inPath1[qq] , "r" );
	
	int n = 0 ;
          int x = 0;
    while (fscanf(fp1 , "%d" , &x) != EOF){
	      Tra[++n] = x ;
    }
    n-- ;
    //Get Every Cross Time !  
	int m = 0 ;
          int  y = 0;
          double x7 = 0, z = 0;
          char ch1 = 0, ch2 = 0;
    while ( fscanf(fp2 , "%lf,%d,%lf" , &x7 ,  &y ,&z) != EOF){
	      Gra[++m] = y ;
	      Grb[m] = x7 ;
    }
    m--;
    int tmmp = 0;
    for (i = 1; i <= m ; i++)
        fscanf(fp3,"%d,%lf,%lf\n" , &tmmp , &Gx[i] , &Gy[i]);
		
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	
	bbb = clock();
	
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
    double cnt = 0, tm = 0;
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
        double x2 = 0, y2   = 0, t2  = 0, x3  = 0, y3  = 0, t3  = 0;
        if (i == 1){
             t2 = Grb[1] ; t3 = Grb[2] ;
             double cntotlen = GDist(Gst[1] , Gst[2]) ;
             nodeToEdgeDistanceAndNodeSide(Gx[2] , Gy[2] , Gra[2] , &cnt , &tm);
             double cntdlen = cnt ;
             nodeToEdgeDistanceAndNodeSide(Gx[1],Gy[1],Gra[m], &cnt , &tm) ;
             Trc[i] = t2 - fabs(t3-t2) * cnt / fabs(cntotlen + tm + cntdlen)  ;
        }
        else if (i == n + 1){
             t2 = Grb[m-1] ; t3 = Grb[m] ;
             double cntotlen = GDist(Gst[m-1] , Gst[m]);
             nodeToEdgeDistanceAndNodeSide(Gx[m-1] , Gy[m-1] , Gra[m-1] , &cnt,  &tm);
             double cntdlen = tm - cnt ; 
             nodeToEdgeDistanceAndNodeSide(Gx[m],Gy[m],Gra[m], &cnt , &tm) ;
             Trc[i] = fabs(t3-t2) * fabs(tm - cnt) / fabs(cntotlen + cnt + cntdlen) + t2 ;
        }
        else {
             t2 = Grb[j-1] ; t3 = Grb[j] ;
             double cntrlen = GDist(Gst[j-1] , i);
             double cntotlen = GDist(Gst[j-1] , Gst[j]);
             nodeToEdgeDistanceAndNodeSide(Gx[j-1] , Gy[j-1] , Gra[j-1] , &cnt,  &tm);
             double cntdlen = tm - cnt ;
             nodeToEdgeDistanceAndNodeSide(Gx[j] , Gy[j] , Gra[j] , &cnt,  &tm);
             Trc[i] = (double)fabs(t3 - t2) * fabs(cntrlen + cntdlen) /fabs (cntotlen + cntdlen + cnt) + t2 ;
        }
      }
    //  freopen("TestOut/testout0.txt", "w" , stdout);
      /*int asdfghjkl = 1 ;
      for (i = 1 ; i <= n + 1 ; i++){
          while (Gst[asdfghjkl] == i && asdfghjkl <= m ){ printf("%d %.3lf 1\n",Gra[asdfghjkl] , Grb[asdfghjkl]); asdfghjkl ++ ;} 
          printf("%d %.3lf\n" ,Tra[i] , Trc[i]);      
          puts("--------------");
      }*/
      
      // 说明： Tra是路段ID  Trc是该段路段起点的时间(double)
      // 共n个路段 n + 1 个时间戳
      
    
    Trc[1] = Trc[2] - 1.0 ;
    double tsum = 0 , rsum = 0 ;
    atop = 0 ;
    pw[1] = 1 ; int ptop = 1 ;
    for (i = 2 ; i < n ; i++) if (thisSide[Tra[i]] != other[Tra[i-1]]) pw[++ptop] = i ; 
    pw[++ptop] = n ; 
    int ix = 0; 
	double tmp ;
	deviation = 0;
    for (ix = 2 ; ix <= ptop ; ix++){
		++traNumber;
        int st = pw[ix-1] ; 
		int tpAtop = atop;
        while (st < pw[ix]){
              i = st ;
           //   printf("%d\n" , st);
              int MDL = 2147483647 ;
			  timeConsume  += clock() - bbb;
			  
              spfa(thisSide[Tra[st]]);
			  bbb = clock();
              int nextst = -1  ;
              int Tflag = 0 ;
           //   puts("firstok");
           //   if (st > 300) system("pause");
              while (i < pw[ix]){
                     i++ ;
                     if (Tf[thisSide[Tra[i]]] != Mxn) Tflag = i ;
                     else break ;
                  //   printf("%d %d\n",Tf[thisSide[Tra[i]]] , (i - st));
                     if (Tf[thisSide[Tra[i]]] < (i - st)){
                                              
                    //    printf("Got it 1\n");
                      //  if (st > 300)system("pause");
                        get_time_values(thisSide[Tra[st]] , thisSide[Tra[i]] , Trc[i]  , Trc[i] - Trc[st]) ;
                        //Into A 
                      //  printf("Got it 2\n");
                      //  if (st > 300)system("pause");
                        double pros = get_MDL(thisSide[Tra[st]] , thisSide[Tra[i]] , st , i) ;
                      //  printf("Got it 3\n");
                      //  if (st > 300)system("pause");
                  //      printf("%d\n" , i);
                       // if (st >  310) system("pause");
                       //double pros = 0 ;
                        if (pros <= MDL){
                           //setpath(i , st) ;
                           nextst = i ;
                           MDL = pros ;
						   double tttt = Fdl[i-1] /2.0;
						   if (tttt > 1 || tttt < 0) {/*printf("%lf\n",tttt);*/tttt = 0.5; }
                           tsum = tttt * (double)(Tf[thisSide[Tra[i]]]) ; 
						   //printf("%.3lf\t",tsum);
                        }
                     }
              }
           //   printf("next start: %d\n" , nextst);
            //  if (st > 200) system("pause");
              if (nextst == -1){
                  if (!Tflag){ 
                               int j ; 
                             // Add_Remain(Tflag) ;
                              for (j = i ; j < pw[ix] ; j++){
                                  ansx[++atop] = thisSide[Tra[j]] ;
                                  anst[atop] = Trc[j] ; 
                              }
                              st = pw[ix] ;
                              break ;
                           //   puts("stoped");
                            //  if (st> 200)system("pause");    
                  }
                 st ++ ;   
                 ansx[++atop] = thisSide[Tra[st-1]] ;
                 anst[atop] = Trc[st-1] ;
             // Add Remainning Points ?
              //   puts("change st");
                //  if (st > 200) system("pause");
             }
             else {
                  //Add_To_Ans((st , nextst)) ;
                  int pi = nextst - st + 1; 
                //  printf("XXX %d\n" , pi);
               //   if (st > 270) printf("%d %d %d\n" , thisSide[Tra[nextst]] , Tra[nextst] , Tg[thisSide[Tra[nextst]]]); 
                  int nowt = Tg[thisSide[Tra[nextst]]] , cnt = 0  ; 
                //  printf("%d $ %d\n" , Tra[st] , atop);
                ///  printf("%d\n" , thisSide[Tra[st]]);
                //  printf("%d\n" ,Tf[thisSide[Tra[nextst]]]);
                  cnt = 0 ;
                  rsum += tsum ; 
                  while (thisSide[Tra[st]] != nowt){
                        ansx[atop + Tf[thisSide[Tra[nextst]]] - cnt] = nowt ;
                        anst[atop + Tf[thisSide[Tra[nextst]]] - cnt] = Tatime[cnt + 1] ; 
                        nowt = Tg[nowt] ;
                //        if (st > 270){ printf("%d\n" , nowt);
                //        system("pause");}  
                        cnt ++ ;
                  }
                //  printf("%d %d\n" , cnt , Tf[thisSide[Tra[nextst]]] );
                ///  system("pause");
                  ansx[atop + 1] = thisSide[Tra[st]] ;
                  anst[atop + 1] = Trc[st] ;  
                  atop += Tf[thisSide[Tra[nextst]]] ;
                  st = nextst ;
                 // puts("change to next");
              //   if (st > 200) system("pause");
             }
			 
        }
		
		//在这里搞========================================
		for (j = pw[ix-1]; j <pw[ix]; ++j) {
			//printf("[%d %.4lf]\t",Tra[j],Trc[j]);
			//puts("");
			oriNode[j] = thisSide[Tra[j]];
			oriTime[j] = Trc[j];
		}
		for (j = tpAtop+1; j <= atop; ++j){
			//printf("[%d %.4lf]\t",ansx[j],anst[j]);
			//puts("");
			//puts("");
			comNode[j] = ansx[j];
			comTime[j] = anst[j];
		}
		tmp = 0;
		int pt = tpAtop+1;
		//for (j = 1; j <=atop; ++j) printf("%lf %lf\t\t",oriTime[j],comTime[j]);
		
		
		for (j = pw[ix-1]; j <pw[ix]; ++j) {
			while (pt < atop && comTime[pt + 1] <= oriTime[j]) ++pt;
			double t1 = comTime[pt];
			double t2;
			//printf("%d %d\t\t",pt,atop);
			if (pt < atop) {
				t2 = comTime[pt+1];
				//printf("[%lf %lf]\t",t1,t2);
				double x1 = coordNode[oriNode[j]*2];
				double y1 = coordNode[oriNode[j]*2+1];
				if (t2 > t1) {
					double rate = (oriTime[j]-t1)/(t2-t1);
					double x2 = coordNode[comNode[pt]*2] + (coordNode[comNode[pt+1]*2]-coordNode[comNode[pt]*2])*rate;
					double y2 = coordNode[comNode[pt]*2+1] + (coordNode[comNode[pt+1]*2+1]-coordNode[comNode[pt]*2+1])*rate;
					double tmp2 = circleDistance(x1,y1,x2,y2);
					tmp += tmp2;
					//printf("%.4lf\t",tmp2);
				}
			}			
		}
		
		if (pw[ix] > pw[ix-1] && tmp > 0 && tmp/(pw[ix] - pw[ix-1]) < 5000)
			deviation += tmp/(pw[ix] - pw[ix-1]);
		//===============================
		//printf("%lf\t",tmp/(pw[ix] - pw[ix-1]));
    }
		
		
		
		oriTotal += n;
		comTotal += atop;
		
		if (deviation/ptop<5000) {
			printf("%d %d %.4lf\n",n,atop,deviation/ptop);
			fprintf(fpp,"%d %d %.4lf\n",n, atop, deviation/ptop);
		}
		
		total += n + ptop;	//点数
		similar += (1- rsum / ((atop + ptop) * 1.0))*(n + ptop);
		compress += (atop+ptop)/((1.0)*(n+ptop))*(n + ptop);
		
		
		//printf("%d\t %.4lf %.4lf %d\n",qq,1- rsum / ((atop + ptop) * 1.0),(atop+ptop)/((1.0)*(n+ptop)),n + ptop);
		
	}
	timeConsume += clock() - bbb;
	
	similar /= total;
	compress /= total;
	
	fclose(fpp);
	
	}
  //  for (int i = 1 ; i <= m ; i++)
  //FILE* fopenfn = ("TestOut/1.out" , "w" , stdout);
  //  for (i = 1 ; i <= atop ; i++) fprintf(fopenfn , "%d %lf\n" , ansx[i] , anst[i]);
  //  printf("%d %d\n" , atop  , n + ptop);
  //  printf("%.3lf %.3lf\n" , (atop+ptop)/((1.0)*(n+ptop)) ,1- rsum / ((atop + ptop) * 1.0)) ; 
    //system("pause");
    //freopen("TestOut/1.out");
//	FILE* fopenfn = ("Testout/1.out" , "w" , stdout);
 //   for (i = 1 ; i <= atop ; i++) fprintf(fopenfn , "%.3lf\n" , anst[i]);
    return 0;
}
