#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAXN 95000000

using namespace std;

int suffix[100];

int son[MAXN], righting[MAXN], value[MAXN], counting, father[MAXN],fail[MAXN],depth[MAXN]; 
int queue[MAXN];
int e = 0,p,q;

int containSon(int f, int v) {
	int pt = son[f];
	while (pt != -1) {
		if (value[pt] == v) return pt;
		pt = righting[pt];
	}
	return -1;
}

int main(int argc, char* argv[]) {
	int num = 0;
	
	FILE* fp = fopen(argv[1],"r");

	fscanf(fp,"%d",&e);
	for (int i = 0; i <= e; ++i)
		fscanf(fp,"%d%d%d%d%d",&son[i],&righting[i],&father[i],&value[i],&counting);
	
	p = 0; q = 0;
	int pt = son[0];
	while (pt != -1) {
		queue[++q] = pt;
		fail[queue[q]] = 0;
		depth[queue[q]] = 1;
		pt = righting[pt];
	}
	puts("here");
	while (p !=q) {
		p++;
		pt = son[queue[p]];
		while (pt != -1) {
			queue[++q] = pt;
			depth[pt] = depth[queue[p]] + 1;
			int f = fail[father[pt]];
			int next = containSon(f,value[pt]);
			while (f != 0 && next < 0) {
				f = fail[f];
				next = containSon(f,value[pt]);
			}
			if (next < 0) fail[pt] = 0; else
				fail[pt] = next;
			
			pt = righting[pt];
		}
	}
	
	FILE* oup = fopen(argv[2],"w");
	//fprintf(oup,"%d\n",e);
	for (int i = 0; i <= e; ++i) {
		fprintf(oup,"%d\t%d\n",fail[i],depth[i]);
	}
	fclose(oup);
	fclose(fp);
	return 0;
}