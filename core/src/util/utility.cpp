//
//  utility.cpp
//  press-v3
//
//  Created by Renchu Song on 11/21/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "utility.hpp"

#include <dirent.h>
#include <fstream>
#include <sstream>
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

bool createFolder(const std::string& dirName) {
  return mkdir(dirName.c_str(), 0777) == 0;
}

bool listDirectory(const std::string& dirName, std::vector<std::string>& fileNames) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(dirName.c_str())) != NULL) {
    fileNames.clear();
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == '.') {
        continue;
      }
      fileNames.push_back(ent->d_name);
    }
    closedir(dir);
    return true;
  } else {
    return false;
  }
}

bool clearDirectory(const std::string& dirName) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(dirName.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == '.') {
        continue;
      }
      remove((dirName + ent->d_name).c_str());
    }
    closedir(dir);
    return true;
  } else {
    return false;
  }
}
