//
//  file_writer.hpp
//  press-v3
//
//  Created by Renchu Song on 10/29/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef file_writer_hpp
#define file_writer_hpp

#include <iostream>

class FileWriter {
  FILE* fp;
  bool binary;

public:
  FileWriter(char* fileName, bool binary);
  FileWriter(char* fileName, char* options);
  bool fileOpened();
  bool writeChar(const char& value);
  bool writeShort(const short& value);
  bool writeInt(const int& value);
  bool writeLong(const long long& value);
  bool writeFloat(const float& value);
  bool writeDouble(const double& value);
  bool writeString(const char*& value);
  bool isBinary();
  bool writeSeparator();
  bool writeEol();
  ~FileWriter();
};

#endif /* file_writer_hpp */
