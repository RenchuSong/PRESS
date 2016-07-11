//
//  filewriter.h
//  press
//
//  Created by Renchu Song on 5/7/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#ifndef filewriter_h
#define filewriter_h

#include "binary.h"

/**
 * The class wrapper for writing data to file
 * - fp:                              file pointer
 * - isBinary:                        whether writing file in binary format or not
 *
 * - FileWriter (fileName, isBinary): Open file specified via filename,
 *                                    and specify in binary format or not
 * - WriteChar ():                    write next one char
 * - WriteShort ():                   Write next short
 * - WriteInt ():                     Write next int
 * - WriteDouble():                   Write next double
 * - IsBinary():                      Return whether the file is binary file
 */
class FileWriter {
private:
  FILE* fp;
  bool isBinary;
  union Char2Binary char2Binary;
  union Short2Binary short2Binary;
  union Int2Binary int2Binary;
  union Double2Binary double2Binary;
  
public:
  FileWriter (std::string fileName, bool isBinary);
  void WriteChar (char value);
  void WriteShort (short value);
  void WriteInt (int value);
  void WriteDouble (double value);
  bool IsBinary ();
  ~FileWriter ();
};

#endif /* filewriter_h */
