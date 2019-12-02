//
//  binary.hpp
//  press-v3
//
//  Created by Renchu Song on 12/1/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef binary_hpp
#define binary_hpp

#include <vector>

#include "./file_reader.hpp"
#include "./file_writer.hpp"

class Binary {
private:
  size_t length;
  std::vector<char> binary;

public:
  Binary(const std::vector<bool>& binary);
  Binary(size_t length, const std::vector<char>& binary);
  Binary(FileReader& binaryReader);
  size_t getLength();
  const std::vector<char>& getBinary();
  void getBitArray(std::vector<bool>& result);
  bool getBitByIndex(size_t index);
  void store(FileWriter& binaryWriter);
  void print();
  ~Binary();
};

#endif /* binary_hpp */
