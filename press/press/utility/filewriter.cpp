//
//  filewriter.cpp
//  press
//
//  Created by Renchu Song on 5/7/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#include <iostream>

#include "filewriter.h"
#include "binary.h"

// Construct file writer
FileWriter::FileWriter (std::string fileName, bool isBinary) {
  this->isBinary = isBinary;
  fp = fopen (fileName.c_str(), isBinary ? "wb" : "w");
  if (fp == NULL) {
    std::cerr << "File open failed" << std::endl;
  }
}

// Write a char
void FileWriter::WriteChar (char value) {
  if (!fp) return;
  
  if (this->isBinary) {
    char2Binary.value = value;
    fwrite (char2Binary.data, sizeof (unsigned char), 1, fp);
  } else {
    fprintf (fp, "%c", value);
  }
}
// Write a short
void FileWriter::WriteShort (short value) {
  if (!fp) return;
  
  if (this->isBinary) {
    short2Binary.value = value;
    fwrite (short2Binary.data, sizeof (unsigned char), 2, fp);
  } else {
    fprintf (fp, "%hd", value);
  }
}
// Write an int
void FileWriter::WriteInt (int value) {
  if (!fp) return;
  
  if (this->isBinary) {
    int2Binary.value = value;
    fwrite (int2Binary.data, sizeof (unsigned char), 4, fp);
  } else {
    fprintf (fp, "%d", value);
  }
}
// Write a double
void FileWriter::WriteDouble (double value) {
  if (!fp) return;
  
  if (this->isBinary) {
    double2Binary.value = value;
    fwrite (double2Binary.data, sizeof (unsigned char), 8, fp);
  } else {
    fprintf (fp, "%lf", value);
  }
}

// Is binary file
bool FileWriter::IsBinary() {
  return this->isBinary;
}

// Destructor
FileWriter::~FileWriter() {
  fclose (fp);
}
