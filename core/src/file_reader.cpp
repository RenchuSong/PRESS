//
//  file_reader.cpp
//  press-v3
//
//  Created by Renchu Song on 10/27/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include "file_reader.hpp"
#include "data_units.hpp"

FileReader::FileReader(char* fileName, bool binary) {
  this->binary = binary;
  fp = fopen(fileName, binary ? "rb" : "r");
}

FileReader::FileReader(char* fileName, char* options) {
  this->binary = (strchr(options, 'b') != NULL);
  fp = fopen(fileName, options);
}

// Check if file is opened.
bool FileReader::fileOpened() {
  return fp != NULL;
}

// Read a char.
char FileReader::nextChar() {
  if (binary) {
    if (fread(char2Binary.data, sizeof(unsigned char), 1, fp) < 1) {
      return EOF;
    }
    return char2Binary.value;
  } else {
    char tmp;
    if (fscanf(fp, "%c", &tmp) == EOF) {
      return EOF;
    }
    return tmp;
  }
}

// Read a short int.
short FileReader::nextShort() {
  if (binary) {
    if (fread(short2Binary.data, sizeof(unsigned char), 2, fp) < 2) {
      return EOF;
    }
    return short2Binary.value;
  } else {
    short tmp;
    if (fscanf(fp, "%hd", &tmp) == EOF) {
      return EOF;
    }
    return tmp;
  }
}

// Read an int.
int FileReader::nextInt() {
  if (binary) {
    if (fread(int2Binary.data, sizeof(unsigned char), 4, fp) < 4) return EOF;
    return int2Binary.value;
  } else {
    int tmp;
    if (fscanf(fp, "%d", &tmp) == EOF) {
      return EOF;
    }
    return tmp;
  }
}

// Read a long.
long long FileReader::nextLong() {
  if (binary) {
    if (fread(long2Binary.data, sizeof(unsigned char), 8, fp) < 8) return EOF;
    return long2Binary.value;
  } else {
    long long tmp;
    if (fscanf(fp, "%lld", &tmp) == EOF) {
      return EOF;
    }
    return tmp;
  }
}

// Read a float.
float FileReader::nextFloat() {
  if (binary) {
    if (fread(float2Binary.data, sizeof(unsigned char), 4, fp) < 4) return EOF;
    return float2Binary.value;
  } else {
    float tmp;
    if (fscanf(fp, "%f", &tmp) == EOF) {
      return EOF;
    }
    return tmp;
  }
}

// Read a double.
double FileReader::nextDouble() {
  if (binary) {
    if (fread(double2Binary.data, sizeof(unsigned char), 8, fp) < 8) return EOF;
    return double2Binary.value;
  } else {
    double tmp;
    if (fscanf(fp, "%lf", &tmp) == EOF) {
      return EOF;
    }
    return tmp;
  }
}

// Read a string with maximal length.
char* FileReader::nextString(int maxLen) {
  char* temp = new char[maxLen];
  if (binary) {
    int i = 0;
    while (fread(char2Binary.data, sizeof(unsigned char), 1, fp) > 0) {
      temp[i] = (char)char2Binary.value;
      if (temp[i++] == 0 || i > 254) {
        break;
      }
    }
    if (i == 0) {
      return NULL;
    }
    temp[i] = 0;
    return temp;
  } else {
    if (fscanf(fp, "%s", temp) == EOF) {
      return NULL;
    }
    return temp;
  }
}

// If the file is binary format.
bool FileReader::isBinary() {
  return binary;
}

FileReader::~FileReader() {
  fclose(fp);
}

