#include <stdio.h>
#include <stdlib.h>

typedef union AB AB;

union AB {
	double d;
	unsigned char c[8];
};

int a[100000][100000];

int main() {
	/*
	AB s;
	//s.d = 1.874;
	FILE* f = fopen("r.txt","rb");
	fread(s.c, sizeof(unsigned char),8,f);
	printf("%.8lf",s.d);*/
	char f[20] = "dfdfdfdfd";
	f[5] = '1';
	printf("%s\n",f);
	return 0;
}