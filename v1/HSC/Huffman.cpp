#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAXN 200000000

using namespace std;

int suffix[100];

int son,righting, value, father, fail,* depth, *counting, *L, *R, *F, *map; 
int *queue,*v;
int e = 0, p1, p2, q;

void quicksort(int l, int r) {
	int g, t, x, y;
	x = l; y = r;
	g = counting[(x+y) / 2];
	while (x <= y) {
		while (counting[x] < g) ++x;
		while (counting[y] > g) --y;
		if (x <= y) {
			t = counting[x];
			counting[x] = counting[y];
			counting[y] = t;
			t = map[x];
			map[x] = map[y];
			map[y] = t;
			++x; --y;
		}
	}
	
	if (l < y) quicksort(l,y);
	if (x < r) quicksort(x,r);
}

int main(int argc, char* argv[]) {
	int num = 0;
	
	FILE* fp = fopen(argv[1],"r");
	//son = (int*)malloc(MAXN*sizeof(int));
	//righting = (int*)malloc(MAXN*sizeof(int));
	//value = (int*)malloc(MAXN*sizeof(int));
	//father = (int*)malloc(MAXN*sizeof(int));
	//fail = (int*)malloc(MAXN*sizeof(int));
	//depth = (int*)malloc(MAXN*sizeof(int));
	counting = (int*)malloc(MAXN*sizeof(int));
	map = (int*)malloc(MAXN*sizeof(int));
	L = (int*)malloc(MAXN*sizeof(int));
	R = (int*)malloc(MAXN*sizeof(int));
	F = (int*)malloc(MAXN*sizeof(int));
	queue = (int*)malloc(MAXN*sizeof(int));
	v = (int*)malloc(MAXN*sizeof(int));

	fscanf(fp,"%d",&e);
	for (int i = 0; i <= e; ++i) {
		fscanf(fp,"%d%d%d%d%d",&son,&righting,&father,&value,&counting[i]);
		map[i] = i;
	}
	fclose(fp);
	puts("H");
	/*
	fp = fopen(argv[2],"r");
	fr (int i = 0; i <=e; ++i) {
		fscanf(fp,"%d%d",&depth[i],&fail);
		
	}
	fclose(fp);
	*/
	
	quicksort(0,e);
	//for (int i = 0; i < e; ++i) printf("%d\t",counting[i]);puts("");
	puts("R");
	
	int te = 0;
	p1 = p2 = 0;
	for (int i = 0; i < e; ++i) {
		
		int mid = 214748364;
		int mode;
		if (p1 <= e-1) {
			int tmp = counting[p1] + counting[p1+1];
			if (tmp < mid) {
				mid = tmp;
				mode = 1;
			}
		}
		if (p1 <=e && p2 < te) {
			int tmp = counting[p1] + v[p2];
			if (tmp < mid) {
				mid = tmp;
				mode = 2;
			}
		}
			
		if (p2 < te - 1) {
			int tmp = v[p2] + v[p2+1];
			if (tmp < mid) {
				mid = tmp;
				mode = 3;
			}
		}
		
		v[te++] = mid;
		if (mode == 1) {
			int f = e + te;
			L[f] = p1;
			R[f] = p1+1;
			p1 += 2;
		}
		//puts("+");
		if (mode == 2) {
			int f = e + te;
			L[f] = p1;
			R[f] = e+p2+1;
			++p1;++p2;
		}
		if (mode == 3) {
			int f = e + te;
			L[f] = e+p2+1;
			R[f] = e+p2+2;
			p2 += 2;
		}
		
	}
	
	FILE* oup = fopen(argv[2],"w");
	//fprintf(oup,"%d\n",e);
	fprintf(oup,"%d\n",te+e);
	
	for (int i = te; i > 0; --i)
		fprintf(oup,"%d\t%d\t%d\t%d\n",e+i,L[e+i],R[e+i],-1);
	for (int i = e; i>=0; --i)
		fprintf(oup,"%d\t%d\t%d\t%d\n",i,-1,-1,map[i]);
	
	fclose(oup);
	
	return 0;
}