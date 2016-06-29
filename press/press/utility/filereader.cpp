//
//  filereader.cpp
//  press
//
//  Created by Renchu Song on 28/6/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#include "filereader.h"
#include "binary.h"

// Construct file reader
FileReader::FileReader (std::string fileName, bool isBinary) {
  this->isBinary = isBinary;
  fp = fopen (fileName.c_str(), isBinary ? "rb" : "r");
  if (fp == NULL) {
    std::cerr << "File open failed" << std::endl;
  }
}

// End of file
bool FileReader::EndOfFile () {
  return fp && feof (fp);
}

// Read a char
char FileReader::NextChar () {
  if (!fp) return 0;
  
  if (this->isBinary) {
    fread (char2Binary.data, sizeof (unsigned char), 1, fp);
    return char2Binary.value;
  } else {
    char temp;
    fscanf (fp, "%c", &temp);
    return temp;
  }
}

// Read a short
short FileReader::NextShort () {
  if (!fp) return 0;
  
  if (this->isBinary) {
    fread (short2Binary.data, sizeof (unsigned char), 2, fp);
    return short2Binary.value;
  } else {
    short temp;
    fscanf (fp, "%hd", &temp);
    return temp;
  }
}

// Read an int
int FileReader::NextInt () {
  if (!fp) return 0;
  
  if (this->isBinary) {
    fread (int2Binary.data, sizeof (unsigned char), 4, fp);
    return int2Binary.value;
  } else {
    int temp;
    fscanf (fp, "%d", &temp);
    return temp;
  }
}

// Read a double
double FileReader::NextDouble () {
  if (!fp) return 0;
  
  if (this->isBinary) {
    fread (double2Binary.data, sizeof (unsigned char), 8, fp);
    return double2Binary.value;
  } else {
    double temp;
    fscanf (fp, "%lf", &temp);
    return temp;
  }
}

// Read a string (no longer than len)
std::string FileReader::NextString (int len) {
  if (!fp) return "";
  
  char* temp = new char[len + 1];
  int i = 0;
  if (this->isBinary) {
    for (i = 0; i < len; ++i) {
      if (!fread (char2Binary.data, sizeof (unsigned char), 1, fp)) {
        break;
      }
      temp[i] = char2Binary.value;
      if (!temp[i]) {
        break;
      }
    }
  } else {
    for (i = 0; i < len; ++i) {
      if (fscanf (fp, "%c", &temp[i]) == EOF || !temp[i]) {
        break;
      }
    }
  }
  temp[i] = 0;
  std::string str(temp, i);
  delete temp;
  return str;
}

// Is binary file
bool FileReader::IsBinary() {
  return this->isBinary;
}

// Destructor
FileReader::~FileReader() {
  fclose (fp);
}
