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

std::string readFile(char* path) {
  std::ifstream input(path);
  std::stringstream sstr;
  while(input >> sstr.rdbuf());
  return sstr.str();
}
