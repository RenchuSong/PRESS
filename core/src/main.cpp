//
//  main.cpp
//  press-v3
//
//  Created by Renchu Song on 5/13/18.
//  Copyright © 2018 Renc. All rights reserved.
//

#include <iostream>
#include "file_reader.hpp"

int main(int argc, const char * argv[]) {
  // insert code here...
  std::cout << "Hello, World!\n";
  FileReader fp("/Users/songrenchu/Develop/test.txt", false);
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
