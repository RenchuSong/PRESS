//
//  file_reader.hpp
//  press-v3
//
//  Created by Renchu Song on 10/26/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef file_reader_hpp
#define file_reader_hpp

#include <iostream>

class FileReader {
  FILE* fp;
  bool binary;
  
public:
  FileReader(const char* fileName, bool binary);
  FileReader(const char* fileName, char* options);
  bool fileOpened() const;
  char nextChar();
  short nextShort();
  int nextInt();
  long long nextLong();
  float nextFloat();
  double nextDouble();
  char* nextString(int maxLen = 256);
  bool isBinary() const;
  ~FileReader();
};

#endif /* file_reader_hpp */
