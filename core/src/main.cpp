//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 5/13/18.
//  Copyright © 2018 Renc. All rights reserved.
//

#include <iostream>
#include "utils/file_reader.hpp"
#include "utils/file_writer.hpp"


int main(int argc, const char * argv[]) {
  FileWriter fw("/Users/songrenchu/Develop/test2.txt", false);
  fw.writeChar('a');
  fw.writeInt(123);
//  char a[4];
//  a = "ABC";
//  a[3] = '\0';
//  fw.writeString(a);
  fw.writeDouble(3.456);
  fw.writeChar(',');
  fw.writeLong(23423);
  fw.writeChar(',');
  fw.writeShort(2);
  
  
  
  
  // insert code here...
  FileReader fp("/Users/songrenchu/Develop/test.txt", (char *)"r");
  std::cout << fp.isBinary() << std::endl;
  std::cout << fp.fileOpened() << std::endl;
  
  
  std::cout << fp.nextInt() << std::endl;
  std::cout << fp.nextChar() << std::endl;
  std::cout << fp.nextLong() << std::endl;
  std::cout << fp.nextChar() << std::endl;
  std::cout << fp.nextDouble() << std::endl;
  std::cout << fp.nextChar() << std::endl;
  std::cout << fp.nextString(1024) << std::endl;
  
  
  return 0;
}
