//
//  main.cpp
//  press
//
//  Created by Renchu Song on 17/1/16.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#include <iostream>
#include "utility/filereader.h"

int main(int argc, const char * argv[]) {
  auto f = std::make_shared<FileReader>("/Users/songrenchu/Develop/PRESS/press/press/test/file1.txt", false);
  std::cout << f->EndOfFile() << std::endl;
  std::cout << f->NextChar() << " " << f->NextShort() << " " << f->NextInt() << " " << f->NextDouble() << std::endl;
  return 0;
}
