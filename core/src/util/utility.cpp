//
//  utility.cpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "utility.hpp"

#include <sstream>
#include <fstream>
#include <sys/stat.h>

std::string readFile(const char* path) {
  std::ifstream input(path);
  std::stringstream sstr;
  while(input >> sstr.rdbuf());
  return sstr.str();
}

bool fileExists(const char* name) {
  struct stat buffer;
  return (stat (name, &buffer) == 0);
}
