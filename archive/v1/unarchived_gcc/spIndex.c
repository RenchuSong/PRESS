#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main() {
	
	char fileName[30]="./ShortestPath/SP00.txt";
	FILE* file;
	
	int i,j,k;
	
	for (i = 0; i < 39; ++i) {
		fileName[17] = (i / 10) + 48;
		fileName[18] = (i % 10) + 48;
		file = fopen(fileName,"rb");
		//文件包含的点数
		int number = readInt(file);
		long count = 4;
		for (j = 0; j < number; ++j) {
			//点Id
			int id = readInt(file);
			count += 4;
			//点Id，文件号，偏移字节
			printf("%d %d %d\n",id,i,count);
			//有多少个点
			int many = readInt(file);
			//偏移
			count += 4 + 8*many;
			fseek(file,count,0);
		}
		
		fclose(file);
	}
	
	return 0;
}