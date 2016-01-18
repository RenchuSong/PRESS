#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

int suffix[100];

int son[95000000], righting[95000000], value[95000000], counting[95000000], father[95000000]; 
int e = 0;

int main(int argc, char* argv[]) {
	
	int num = 0;
	char* ttt = argv[1];
	for (int i = 0; i < strlen(ttt); ++i) num = num * 10 + ttt[i] - 48;
	printf("%d\n",num);
	
	FILE* fp = fopen(argv[2],"r");
	
	memset(son,255,sizeof(son));
	memset(righting,255,sizeof(righting));
	
	int t = 0;
	while (1) {
		int flag = 1;
		for (int i = 0; i < num; ++i) {
			if (fscanf(fp,"%d",&suffix[i]) == EOF) {
				flag = 0;break;
			}
		}
		if (!flag) break;
		++t;
		int pt = 0;
		for (int i = 0; i < num; ++i) {
			//还没孩子，那么直接加个孩子
			if (son[pt] == -1) {
				++e;
				son[pt] = e;
				father[e] = pt;
				pt = e;
				value[pt] = suffix[i];	//值
				++counting[pt];
			}else {
				int find = 0;
				int tmp = son[pt],last;
				while (tmp!=-1 && value[tmp]!=suffix[i]) {
					last = tmp;
					tmp = righting[tmp];
				}
				//没找到，还是要加儿子
				if (tmp == -1) {
					++e;
					righting[last] = e;
					father[e] = pt;
					pt = e;
					value[pt] = suffix[i];
					++counting[pt];
				}else {	//找到了，往下走
					pt = tmp;
					++counting[pt];
				}
			}
		}
		if (t % 10000 == 0) printf("%d\n",t);
	}
	
	//补充未加入的道路作为儿子
	for (int i = 1; i < 55892; ++i) {
		int tmp = son[0],last;
		while (tmp!=-1 && value[tmp]!=i) {
			last = tmp;
			tmp = righting[tmp];
		}
		//没找到，还是要加儿子
		if (tmp == -1) {
			++e;
			righting[last] = e;
			father[e] = 0;
			value[e] = i;
			counting[e] = 0;
		}
	}
	
	FILE* oup = fopen(argv[3],"w");
	fprintf(oup,"%d\n",e);
	for (int i = 0; i <= e; ++i) {
		fprintf(oup,"%d\t%d\t%d\t%d\t%d\n",son[i],righting[i],father[i],value[i],counting[i]);
	}
	fclose(oup);
	fclose(fp);
	return 0;
}