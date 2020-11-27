//
//  file_writer.cpp
//  press-v3
//
//  Created by Renchu Song on 10/29/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cstring>

#include "binary_converter.hpp"
#include "file_writer.hpp"

extern union Char2Binary char2Binary;
extern union Short2Binary short2Binary;
extern union Int2Binary int2Binary;
extern union Long2Binary long2Binary;
extern union Float2Binary float2Binary;
extern union Double2Binary double2Binary;

FileWriter::FileWriter(const char* fileName, bool binary) {
  this->binary = binary;
  fp = fopen(fileName, binary ? "wb" : "w");
}

FileWriter::FileWriter(const char* fileName, char* options) {
  this->binary = (strchr(options, 'b') != NULL);
  fp = fopen(fileName, options);
}

// Check if file is opened.
bool FileWriter::fileOpened() const {
  return fp != NULL;
}

// Write a char.
bool FileWriter::writeChar(const char& value) {
  size_t bytesWriten = 0;
  if (binary) {
    char2Binary.value = value;
    bytesWriten = fwrite(char2Binary.data, sizeof(unsigned char), 1, fp);
  } else {
    bytesWriten = fprintf(fp, "%c", value);
  }
  return bytesWriten == 1;
}

// Write a short int.
bool FileWriter::writeShort(const short& value) {
  size_t bytesWriten = 0;
  if (binary) {
    short2Binary.value = value;
    bytesWriten = fwrite(short2Binary.data, sizeof(unsigned char), 2, fp);
  } else {
    bytesWriten = fprintf(fp, "%hd", value);
  }
  return bytesWriten == 2;
}

// Write an int.
bool FileWriter::writeInt(const int& value) {
  size_t bytesWriten = 0;
  if (binary) {
    int2Binary.value = value;
    bytesWriten = fwrite(int2Binary.data, sizeof(unsigned char), 4, fp);
  } else {
    bytesWriten = fprintf(fp, "%d", value);
  }
  return bytesWriten == 4;
}

// Write a long.
bool FileWriter::writeLong(const long long& value) {
  size_t bytesWriten = 0;
  if (binary) {
    long2Binary.value = value;
    bytesWriten = fwrite(long2Binary.data, sizeof(unsigned char), 8, fp);
  } else {
    bytesWriten = fprintf(fp, "%lld", value);
  }
  return bytesWriten == 8;
}

// Write a float.
bool FileWriter::writeFloat(const float& value) {
  size_t bytesWriten = 0;
  if (binary) {
    float2Binary.value = value;
    bytesWriten = fwrite(float2Binary.data, sizeof(unsigned char), 4, fp);
  } else {
    bytesWriten = fprintf(fp, "%f", value);
  }
  return bytesWriten == 4;
}

// Write a double.
bool FileWriter::writeDouble(const double& value) {
  size_t bytesWriten = 0;
  if (binary) {
    double2Binary.value = value;
    bytesWriten = fwrite(double2Binary.data, sizeof(unsigned char), 8, fp);
  } else {
    bytesWriten = fprintf(fp, "%lf", value);
  }
  return bytesWriten == 8;
}

// Write a string.
bool FileWriter::writeString(const char*& value) {
  for (auto i = 0; i <= strlen(value); i++) {
    if (!writeChar(value[i])) {
      return false;
    }
  }
  return true;
}

// Write a separator. For binary, we don't need a separator, so will do nothing.
bool FileWriter::writeSeparator() {
  if (binary) {
    return true;
  } else {
    return fprintf(fp, "%c", ' ') == 1;
  }
}

// Write an eol.For binary, we don't need an eol, so will do nothing.
bool FileWriter::writeEol() {
  if (binary) {
    return true;
  } else {
    return fprintf(fp, "%c", '\n') == 1;
  }
}

// If the file is binary format.
bool FileWriter::isBinary() const {
  return binary;
}

FileWriter::~FileWriter() {
  fclose(fp);
}
