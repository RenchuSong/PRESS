//最短路压缩后的二次压缩
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define MAXN 200000000

using namespace std;

int *son,*righting, *value, *father, *fail, *depth, counting, *L, *R, *F, *map, *rvsmap, *hfmdepth; 
int e = 0, p1, p2, q;

int path[100000];
int trie[100000];
int answer[100000];
int anslen[100000];

char* out;

//--------------------------------
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
//----------------------------------------------
//某节点是否存在某个值的儿子
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
	
	son = (int*)malloc(MAXN*sizeof(int));
	righting = (int*)malloc(MAXN*sizeof(int));
	value = (int*)malloc(MAXN*sizeof(int));
	father = (int*)malloc(MAXN*sizeof(int));
	fail = (int*)malloc(MAXN*sizeof(int));
	depth = (int*)malloc(MAXN*sizeof(int));
	//counting = (int*)malloc(MAXN*sizeof(int));
	map = (int*)malloc(MAXN*sizeof(int));
	rvsmap = (int*)malloc(MAXN*sizeof(int));
	L = (int*)malloc(MAXN*sizeof(int));
	R = (int*)malloc(MAXN*sizeof(int));
	F = (int*)malloc(MAXN*sizeof(int));
	out = (char*)malloc(MAXN*sizeof(char));
	hfmdepth = (int*)malloc(MAXN*sizeof(int));
	
	long bbb = clock();

	//Trie
	FILE* fp = fopen(argv[1],"r");
	fscanf(fp,"%d",&e);
	for (int i = 0; i <= e; ++i) {
		fscanf(fp,"%d%d%d%d%d",&son[i],&righting[i],&father[i],&value[i],&counting);
	}
	fclose(fp);
	
	//AC自动机fail指针
	fp = fopen(argv[2],"r");
	for (int i = 0; i <= e; ++i) {
		fscanf(fp,"%d%d",&fail[i],&depth[i]);
	}
	fclose(fp);
	
	//Halfman树编码
	fp = fopen(argv[3],"r");
	int halfRoot;
	hfmdepth[halfRoot] = 0;
	fscanf(fp,"%d",&halfRoot);
	for (int i = 0; i <= halfRoot; ++i) {
		int id;
		fscanf(fp,"%d",&id);
		fscanf(fp,"%d%d%d",&L[id],&R[id],&map[id]);
		if (L[id] > -1) { F[L[id]] = id;hfmdepth[L[id]] = hfmdepth[id] + 1;}
		if (R[id] > -1) {F[R[id]] = id;hfmdepth[R[id]] = hfmdepth[id] + 1;}

		rvsmap[map[id]] = id;
	}
	fclose(fp);
	
	
	
	//输出结果
	FILE* oup = fopen(argv[5],"w");
	
	//读入数据，并压缩
	fp = fopen(argv[4],"rb");
	int tjNumber = readInt(fp);
	fprintf(oup,"%d\n",tjNumber);
	
	for (int i = 0; i < tjNumber; ++i) {
		if (i % 100 == 99) printf("+");
		
		int spNumber = readInt(fp);
		for (int j = 0; j < spNumber; ++j)
			path[j] = readInt(fp);
		
		int pt = 0;
		/*
		for (int j = 0; j < spNumber; ++j) {
			int next = containSon(pt,path[j]);
			int mm = 214748364,mr,tpt;

			if (next != -1) {
				int r = j - depth[next];
				if (r < 0) 
					mr = hfmdepth[rvsmap[next]];
				else
					mr = anslen[r] + hfmdepth[rvsmap[next]];
				if (mr < mm) {
					mm = mr;
					tpt = next;
					anslen[j] = mm;
				}
			}


			while (pt > 0) {
				pt = fail[pt];
				next = containSon(pt,path[j]);

				if (next != -1) {
					int r = j - depth[next];
					if (r < 0) 
						mr = hfmdepth[rvsmap[next]];
					else
						mr = anslen[r] + hfmdepth[rvsmap[next]];
					if (mr < mm) {
						mm = mr;
						tpt = next;
						anslen[j] = mm;
					}
				}

			}
			if (mm < 214748364) {
				pt = tpt;
				trie[j] = pt;
			}
		}*/

		for (int j = 0; j < spNumber; ++j) {
			int next = containSon(pt,path[j]);
			while (pt > 0 && next == -1) {
				pt = fail[pt];
				next = containSon(pt,path[j]);
			}
			if (next != -1) {
				pt = next;
				trie[j] = next;
			}
		}

		int tp = spNumber - 1;
		int ans = 0, aa = 0;
		while (tp >= 0) {
			int pt = rvsmap[trie[tp]];
			
			answer[aa++] = trie[tp];

			while (pt != halfRoot) {
				int tmp = F[pt];
				if (L[tmp] == pt)
					out[ans++] = '0';
				else
					out[ans++] = '1';
				pt = tmp;
			}
			tp -= depth[trie[tp]];
			//++ans;
		}

		
		for (int j = ans - 1; j >= 0; --j) fprintf(oup,"%c",out[j]);
		fprintf(oup,"\n");

	}
	fclose(fp);
	
	fclose(oup);
	
	FILE* ff = fopen("./SpatialTime-Greedy.txt","a+");
	fprintf(ff,"%s %ld\n",argv[5],clock() - bbb);
	fclose(ff);

	return 0;
}