//
//  file_processor.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 7/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#include "file_processor.h"


//File Reader
class FileReader {
	FILE* fp;
	bool binary;
	
public:
	FileReader(char* fileName, bool binary) {
		this->binary = binary;
		fp = fopen(fileName, binary ? "rb" : "r");
	}
	
	FileReader(char* fileName, char* options, bool binary) {
		this->binary = binary;
		fp = fopen(fileName, options);
	}
	
	//read a char
	char nextChar() {
		if (this->binary) {
			if (fread(char2Binary.data,sizeof(unsigned char), 1,fp) < 1) return EOF;
			return char2Binary.value;
		} else {
			char temp;
			fscanf(fp, "%c", &temp);
			return temp;
		}
	}
	
	//read a short int
	short nextShort() {
		if (this->binary) {
			if (fread(short2Binary.data,sizeof(unsigned char), 2,fp) < 2) return EOF;
			return short2Binary.value;
		} else {
			short temp;
			fscanf(fp, "%hd", &temp);
			return temp;
		}
	}
	
	//read an int
	int nextInt() {
		if (this->binary) {
			if (fread(int2Binary.data,sizeof(unsigned char), 4,fp) < 4) return EOF;
			return int2Binary.value;
		} else {
			int temp;
			fscanf(fp, "%d", &temp);
			return temp;
		}
	}
	
	//read a double
	double nextDouble() {
		if (this->binary) {
			if (fread(double2Binary.data,sizeof(unsigned char), 8,fp) < 8) return EOF;
			return double2Binary.value;
		} else {
			double temp;
			fscanf(fp, "%lf", &temp);
			return temp;
		}
	}
	
	bool isBinary() {
		return this->binary;
	}
	
	~FileReader() {
		fclose(fp);
	}
};

//File Writer
class FileWriter {
	FILE* fp;
	bool binary;
	
public:
	FileWriter(char* fileName,  bool binary) {
		this->binary = binary;
		fp = fopen(fileName, binary ? "wb" : "w");
	}
	
	FileWriter(char* fileName, char* options, bool binary) {
		this->binary = binary;
		fp = fopen(fileName, options);
	}
	
	//write a char
	void writeChar(char value) {
		if (this->binary) {
			char2Binary.value = value;
			fwrite(char2Binary.data,sizeof(unsigned char), 1,fp);
		} else {
			fprintf(fp, "%c", value);
		}
	}
	
	//write a short int
	void writeShort(short value) {
		if (this->binary) {
			short2Binary.value = value;
			fwrite(short2Binary.data,sizeof(unsigned char), 2,fp);
		} else {
			fprintf(fp, "%hd", value);
		}
	}
	
	//write an int
	void writeInt(int value) {
		if (this->binary) {
			int2Binary.value = value;
			fwrite(int2Binary.data,sizeof(unsigned char), 4,fp);
		} else {
			fprintf(fp, "%d", value);
		}
	}
	
	//write a double
	void writeDouble(double value) {
		if (this->binary) {
			double2Binary.value = value;
			fwrite(double2Binary.data,sizeof(unsigned char), 8,fp);
		} else {
			fprintf(fp, "%lf", value);
		}
	}
	
	bool isBinary() {
		return this->binary;
	}
	
	~FileWriter() {
		fclose(fp);
	}
};
