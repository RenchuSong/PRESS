//
//  main.cpp
//  press
//
//  Created by Renchu Song on 17/1/16.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#include <iostream>
#include "utility/filereader.h"
#include "utility/bitvector.h"

int main(int argc, const char * argv[]) {
  auto f = std::make_shared<FileReader>("/Users/songrenchu/Develop/PRESS/press/press/test/file1.txt", false);
  std::cout << f->EndOfFile() << std::endl;
  std::cout << f->NextChar() << " " << f->NextShort() << " " << f->NextInt() << " " << f->NextDouble() << std::endl;
  std::cout << f->EndOfFile() << std::endl;
  
  BitVector bv;
  bv.Append(true);
  bv.Append(true);
  bv.Append(false);
  bv.Append(true);
  bv.Append(false);
  bv.Append(false);
  bv.Append(false);
  bv.Append(true);
  bv.Append(false);
  
  bv.Show();
  FileWriter* fw = new FileWriter ("/Users/songrenchu/Develop/PRESS/press/press/test/file2.txt", true);
  bv.Store(*fw);
  delete fw;
  FileReader* fr = new FileReader ("/Users/songrenchu/Develop/PRESS/press/press/test/file2.txt", true);
  BitVector bv2 (*fr);
  delete fr;
  bv2.Show();
  bv2.Append(false);
  bv2.Append(false);
  bv2.Append(true);
  bv2.Append(true);
  bv2.Append(false);
  bv2.Append(true);
  bv2.Append(true);
  bv2.Show();
  FileWriter* fw2 = new FileWriter ("/Users/songrenchu/Develop/PRESS/press/press/test/file3.txt", true);
  bv2.Store(*fw2);
  delete fw2;
  FileReader* fr2 = new FileReader ("/Users/songrenchu/Develop/PRESS/press/press/test/file3.txt", true);
  BitVector bv3 (*fr2);
  bv3.Show();
  delete fr2;
  
  
  return 0;
}
