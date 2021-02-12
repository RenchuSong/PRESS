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
#include <string>

std::string readFile(const char* path);
bool fileExists(const char* name);
bool createFolder(const std::string& dirName);
bool listDirectory(const std::string& dirName, std::vector<std::string>& fileNames);
bool clearDirectory(const std::string& dirName);
std::string vecStringToJSONString(const std::vector<std::string>& data);
std::string toJSONString(const std::string& str);

template<typename T> std::string vecToJSONString(const std::vector<T>& data) {
  std::string result("[");
  auto len = data.size();
  if (len > 0) {
    result += data.at(0).toJSONString();
  }
  for (auto i = 1; i < len; ++i) {
    result += "," + data.at(i).toJSONString();
  }
  result += "]";
  return result;
}

template<typename T> std::string vecPrimitiveToJSONString(const std::vector<T>& data) {
  std::string result("[");
  auto len = data.size();
  if (len > 0) {
    result += std::to_string(data.at(0));
  }
  for (auto i = 1; i < len; ++i) {
    result += "," + std::to_string(data.at(i));
  }
  result += "]";
  return result;
}

#endif /* utility_hpp */
