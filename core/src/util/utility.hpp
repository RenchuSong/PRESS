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
#include <vector>

std::string readFile(const char* path);
bool fileExists(const char* name);
bool createFolder(const std::string& dirName);
bool listDirectory(const std::string& dirName, std::vector<std::string>& fileNames);
bool clearDirectory(const std::string& dirName);

#endif /* utility_hpp */
