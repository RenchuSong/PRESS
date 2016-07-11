//
//  filereader.h
//  press
//
//  Created by Renchu Song on 28/6/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#ifndef filereader_h
#define filereader_h

#include <iostream>

#include "binary.h"

/**
 * The class wrapper for reading data from file
 * - fp:                              file pointer
 * - isBinary:                        whether reading file in binary format or not
 *
 * - FileReader (fileName, isBinary): Open file specified via filename,
 *                                    and specify in binary format or not
 * - EndOfFile ();                    End of file
 * - NextChar ():                     Read next one char
 * - NextShort ():                    Read next short
 * - NextInt ():                      Read next int
 * - NextDouble():                    Read next double
 * - NextString (int len):            Read next string (no longer than len, default 256)
 * - IsBinary():                      Return whether the file is binary file
 */
class FileReader {
private:
  FILE* fp;
  bool isBinary;
  union Char2Binary char2Binary;
  union Short2Binary short2Binary;
  union Int2Binary int2Binary;
  union Double2Binary double2Binary;
  
public:
  FileReader (std::string fileName, bool isBinary);
  bool EndOfFile ();
  char NextChar ();
  short NextShort ();
  int NextInt ();
  double NextDouble ();
  std::string NextString (int len = 256);
  bool IsBinary ();
  ~FileReader ();
};

#endif /* filereader_h */
