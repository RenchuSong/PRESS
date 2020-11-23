//
//  utility.hpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef utility_hpp
#define utility_hpp

#include <iostream>

std::string readFile(const char* path);
bool fileExists(const char* name);
bool createFolder(const std::string& dirName);

#endif /* utility_hpp */
